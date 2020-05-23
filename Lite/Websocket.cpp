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
	void StartReceive();
	void HandleMessage(const websocket_incoming_message& msg);
	void HandleConnectResult(bool success);
	void HandleClose();
	void HandleSendDone();
	void SendQueue();

	static websocket_client_config GetClientConfig();

	CAddress address_;
	std::shared_ptr<CConnEventDelegate> delegate_;
	std::atomic<bool> is_secure_ = false;

	std::atomic<bool> shutdown_ = false;
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
	std::string url = address_.URL();
	auto ws = shared_from_this();
	client_.connect(web::uri(utility::conversions::to_string_t(url))).then(
		[ws](pplx::task<void> task) {
			try {
				task.get();
				ws->HandleConnectResult(true);
				ws->StartReceive();
			}
			catch (const std::exception &ex) {
				OutputDebugString(ex.what());
				ws->HandleConnectResult(false);
			}
		});
	return true;
}

void CWebsocket::StartReceive()
{
	auto ws = shared_from_this();
	client_.receive().then(
		[ws](pplx::task<websocket_incoming_message> task) {
			try {
				ws->HandleMessage(task.get());
			}
			catch (const std::exception &ex) {
				OutputDebugString(ex.what());
				ws->HandleClose();
			}
		});
}

int CWebsocket::Send(const void* data, size_t length)
{
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
	client_.send(msg).then(std::bind(&CWebsocket::HandleSendDone, shared_from_this()));
}

void CWebsocket::HandleSendDone()
{
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
	if (shutdown_.exchange(true))
		return;
	auto ws = shared_from_this();
	client_.close().then([ws]() {
		ws->delegate_->OnClose();
	});
}

void CWebsocket::HandleMessage(const websocket_incoming_message& msg)
{
	switch (msg.message_type()) {
		case websocket_message_type::binary_message:
		{
			Concurrency::streams::container_buffer<std::vector<uint8_t>> buf;
			const size_t length = msg.length();
			auto ws = shared_from_this();
			msg.body().read(buf, length).then([ws, buf](size_t n) {
				const auto &vec = buf.collection();
				ws->delegate_->OnReceive(&vec[0], vec.size());
				ws->StartReceive();
			});
			break;
		}

		case websocket_message_type::close:
			HandleClose();
			break;

		default:
			StartReceive();
			break;
	}
}

void CWebsocket::HandleConnectResult(bool success)
{
	is_secure_ = (client_.uri().scheme() == U("wss"));

	delegate_->OnConnect(true);
}

void CWebsocket::HandleClose()
{
	delegate_->OnClose();
}

std::shared_ptr<CConnIO> CreateWebsocket(
	const CAddress& address,
	std::shared_ptr<CConnEventDelegate> delegate) {
	return std::make_shared<CWebsocket>(address, delegate);
}
