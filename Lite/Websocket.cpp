#include "Websocket.h"
#include "ConnIO.h"

#include <atomic>
#include <deque>
#include <string>
#include <mutex>
#include <thread>

#include <cpprest/ws_client.h>

using web::websockets::client::websocket_client;
using web::websockets::client::websocket_client_config;
using web::websockets::client::websocket_incoming_message;
using web::websockets::client::websocket_outgoing_message;
using web::websockets::client::websocket_close_status;
using web::websockets::client::websocket_message_type;

enum class WebsocketState {
	NEW = 0,
	CONNECTING = 1,
	ESTABLISHED = 2,
	CLOSED = 3,
};

class CWebsocket
	: public std::enable_shared_from_this<CWebsocket>
	, public CConnIO
{
public:
	CWebsocket(
		const CAddress& address,
		std::shared_ptr<CConnEventDelegate> delegate);
	~CWebsocket() {}
	bool Connect() override;
	int Send(const void* data, size_t length) override;
	void Shutdown() override;
	void Close() override;

	bool IsSecure() const override { return is_secure_; }
	std::string Description() const override { return ""; }

private:
	void HandleConnectDone(pplx::task<void> task);
	void StartReceive();
	void HandleReceiveDone(pplx::task<websocket_incoming_message> task);
	void HandleMessage(const websocket_incoming_message& msg);
	void HandleSendDone(pplx::task<void> task);
	void SendQueue();
	void HandleCloseError(const std::exception& ex);
	void HandleCloseNormal();

	static websocket_client_config GetClientConfig();

	CAddress address_;
	std::shared_ptr<CConnEventDelegate> delegate_;
	std::atomic<bool> is_secure_ = false;

	std::atomic<WebsocketState> state_ = WebsocketState::NEW;
	websocket_client client_;

	std::mutex mu_;
	std::deque<std::vector<uint8_t>> send_buffers_;
};

CWebsocket::CWebsocket(
	const CAddress& address,
	std::shared_ptr<CConnEventDelegate> delegate)
	: address_(address)
	, delegate_(delegate)
	, client_(GetClientConfig())
{
}

// static
websocket_client_config CWebsocket::GetClientConfig()
{
	websocket_client_config config;
	config.headers()[U("Origin")] = U("app://pcman");
	return config;
}

bool CWebsocket::Connect()
{
	assert(state_ == WebsocketState::NEW);
	state_ = WebsocketState::CONNECTING;

	std::string url = address_.URL();
	auto ws = shared_from_this();
	client_.connect(web::uri(utility::conversions::to_string_t(url))).then(
		[ws](pplx::task<void> task) {
			ws->HandleConnectDone(task);
		});
	return true;
}

void CWebsocket::HandleConnectDone(pplx::task<void> task)
{
	bool is_success = false;
	try {
		task.get();
		is_success = true;
	}
	catch (const std::exception &ex) {
		OutputDebugString(ex.what());
	}

	state_ = is_success ? WebsocketState::ESTABLISHED : WebsocketState::CLOSED;
	is_secure_ = (client_.uri().scheme() == U("wss"));

	delegate_->OnConnect(is_success);

	if (is_success)
		StartReceive();
}

void CWebsocket::StartReceive()
{
	auto ws = shared_from_this();
	client_.receive().then([ws](pplx::task<websocket_incoming_message> task) {
		ws->HandleReceiveDone(task);
	});
}

void CWebsocket::HandleReceiveDone(pplx::task<websocket_incoming_message> task)
{
	try {
		HandleMessage(task.get());
	}
	catch (const std::exception &ex) {
		HandleCloseError(ex);
	}
}

int CWebsocket::Send(const void* data, size_t length)
{
	// We drop user inputs before websocket is established.
	// So that if user smashed on keyboard during connect,
	// they won't suddenly see craps sent out after we are connected.
	if (state_ != WebsocketState::ESTABLISHED)
		return length;

	std::vector<uint8_t> buf(length);
	memcpy(&buf[0], data, length);
	bool initiate_send;
	{
		std::lock_guard<std::mutex> lk(mu_);
		initiate_send = send_buffers_.empty();
		send_buffers_.push_back(std::move(buf));
	}
	if (initiate_send)
		SendQueue();
	return length;
}

void CWebsocket::SendQueue()
{
	std::lock_guard<std::mutex> lk(mu_);
	if (send_buffers_.empty())
		return;

	websocket_outgoing_message msg;
	msg.set_binary_message(
		Concurrency::streams::container_stream<std::vector<uint8_t>>::open_istream(send_buffers_.front()));
	auto ws = shared_from_this();
	client_.send(msg).then([ws](pplx::task<void> task) {
		ws->HandleSendDone(task);
	});
}

void CWebsocket::HandleSendDone(pplx::task<void> task)
{
	try {
		task.get();
	}
	catch (const std::exception &ex) {
		HandleCloseError(ex);
		return;
	}

	{
		std::lock_guard<std::mutex> lk(mu_);
		send_buffers_.pop_front();
	}
	SendQueue();
}

void CWebsocket::Shutdown()
{
	Close();
}

void CWebsocket::Close()
{
	if (state_.exchange(WebsocketState::CLOSED) == WebsocketState::CLOSED)
		return;
	auto ws = shared_from_this();
	client_.close().then([ws](pplx::task<void> task) {
		// We don't really care about whether task is successful.
		ws->delegate_->OnClose();
	});
}

void CWebsocket::HandleCloseError(const std::exception& ex)
{
	if (state_.exchange(WebsocketState::CLOSED) == WebsocketState::CLOSED)
		return;
	OutputDebugString(ex.what());
	delegate_->OnClose();
}

void CWebsocket::HandleCloseNormal()
{
	if (state_.exchange(WebsocketState::CLOSED) == WebsocketState::CLOSED)
		return;
	delegate_->OnClose();
}

void CWebsocket::HandleMessage(const websocket_incoming_message& msg)
{
	switch (msg.message_type()) {
		case websocket_message_type::binary_message:
		{
			Concurrency::streams::container_buffer<std::vector<uint8_t>> buf;
			const size_t length = msg.length();
			auto ws = shared_from_this();
			msg.body().read(buf, length).then([ws, buf](pplx::task<size_t> task) {
				try {
					size_t n = task.get();
					const auto &vec = buf.collection();
					ws->delegate_->OnReceive(&vec[0], vec.size());
				}
				catch (const std::exception& ex) {
					OutputDebugString(ex.what());
					// Continue to receive.
					// We should get error later if connection broke.
				}
				ws->StartReceive();
			});
			break;
		}

		case websocket_message_type::close:
			HandleCloseNormal();
			break;

		default:
			StartReceive();
			break;
	}
}

std::shared_ptr<CConnIO> CreateWebsocket(
	const CAddress& address,
	std::shared_ptr<CConnEventDelegate> delegate) {
	return std::make_shared<CWebsocket>(address, delegate);
}
