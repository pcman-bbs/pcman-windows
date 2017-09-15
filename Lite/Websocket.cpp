#include "Websocket.h"
#include "ConnIO.h"

#include <atomic>
#include <string>
#include <mutex>
#include <thread>

#include <libwebsockets.h>

#include "Shlwapi.h"

int HandlerCallbackFunc(
	struct lws *wsi, enum lws_callback_reasons reason,
	void *user, void *in, size_t len);

static const struct lws_protocols protocols[] = {
	{
		"telnet",
		&HandlerCallbackFunc,
		0,
		4096,
	},
	{ NULL, NULL, 0, 0 } /* end */
};

class CWebsocketManager
{
public:
	CWebsocketManager();
	~CWebsocketManager();

	lws* Connect(struct lws_client_connect_info* info);
	void RemoveConn();
	void ServiceLoop();
	struct lws_context *GetContext() { return context; }

	static CWebsocketManager& Instance() { return singleton; }

private:
	struct lws_context_creation_info ctx_info;
	struct lws_context *context;

	std::mutex mu;
	bool has_service_thread;
	int num_active;

	static CWebsocketManager singleton;

	void AddConn();
};

CWebsocketManager CWebsocketManager::singleton;

CWebsocketManager::CWebsocketManager() :
	has_service_thread(false), num_active(0)
{
	lws_set_log_level(
#ifdef _DEBUG
		LLL_NOTICE |
#endif
		LLL_INFO | LLL_ERR | LLL_WARN | LLL_CLIENT,
		[](int level, const char* line) {
		OutputDebugStringA(line);
	});

	memset(&ctx_info, 0, sizeof(ctx_info));
	ctx_info.port = CONTEXT_PORT_NO_LISTEN;
	ctx_info.protocols = protocols;
	ctx_info.options =
		LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT |
		LWS_SERVER_OPTION_JUST_USE_RAW_ORIGIN;
	context = lws_create_context(&ctx_info);
}

CWebsocketManager::~CWebsocketManager()
{
	lws_context_destroy(context);
	context = nullptr;
}

lws* CWebsocketManager::Connect(struct lws_client_connect_info* info)
{
	AddConn();
	info->context = context;
	lws* lws = lws_client_connect_via_info(info);
	// Restart current poll, so new fd can be added.
	lws_cancel_service(context);
	return lws;
}

void CWebsocketManager::AddConn()
{
	std::lock_guard<std::mutex> lk(mu);
	++num_active;
	if (!has_service_thread) {
		std::thread(&CWebsocketManager::ServiceLoop, this).detach();
		has_service_thread = true;
	}
}

void CWebsocketManager::RemoveConn()
{
	std::lock_guard<std::mutex> lk(mu);
	if (!--num_active) {
		lws_cancel_service(context);
	}
}

void CWebsocketManager::ServiceLoop()
{
	OutputDebugString("CWebsocketManager: service loop started\n");
	while (true) {
		lws_service(context, 10000);

		{
			std::lock_guard<std::mutex> lk(mu);
			if (!num_active) {
				has_service_thread = false;
				break;
			}
		}
	}
	OutputDebugString("CWebsocketManager: service loop ended\n");
}

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

	void CheckSecurity();
	bool IsSecure() const override { return is_secure_; }
	std::string Description() const override { return cipher_; }

	int OnCallback(
		struct lws *wsi, enum lws_callback_reasons reason,
		void* user, void *in, size_t len);

	struct UserData
	{
	public:
		UserData(const std::shared_ptr<CWebsocket>& websocket)
			: websocket_(websocket) {}

		CWebsocket& websocket() { return *websocket_; }

	private:
		std::shared_ptr<CWebsocket> websocket_;
	};

private:
	CAddress address_;
	// Host header to send. It will contain port number if it's not the default.
	CString host_header_;
	std::shared_ptr<CConnEventDelegate> delegate_;
	std::atomic<bool> is_secure_;
	std::string cipher_;

	struct lws_client_connect_info cli_info;

	std::mutex mu_;
	struct lws* lws_;

	std::atomic<bool> shutdown_;
};

std::shared_ptr<CConnIO> CreateWebsocket(
	const CAddress& address,
	std::shared_ptr<CConnEventDelegate> delegate)
{
	return std::make_shared<CWebsocket>(address, delegate);
}

int HandlerCallbackFunc(
	struct lws *wsi, enum lws_callback_reasons reason,
	void *user, void *in, size_t len)
{
	switch (reason) {
	case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
		lwsl_notice("dumb: LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS\n");
		TCHAR module_path[MAX_PATH], buf[MAX_PATH];
		if (GetModuleFileName(nullptr, module_path, sizeof(module_path) / sizeof(TCHAR)) > 0 &&
			PathRemoveFileSpec(module_path) &&
			PathCombine(buf, module_path, "cacert.pem")) {
			if (!SSL_CTX_load_verify_locations((SSL_CTX *)user, buf, nullptr)) {
				lwsl_err("SSL_CTX_load_verify_locations error\n");
			}
		}
		return 0;
	}

	// Some callbacks are called with other lws. :(
	void* data = lws_wsi_user(wsi);
	if (data == nullptr)
		return 0;

	return reinterpret_cast<CWebsocket::UserData*>(data)
		->websocket().OnCallback(wsi, reason, user, in, len);
}

CWebsocket::CWebsocket(
	const CAddress& address,
	std::shared_ptr<CConnEventDelegate> delegate)
	: address_(address)
	, delegate_(delegate)
	, is_secure_(false)
{
}

bool CWebsocket::Connect()
{
	memset(&cli_info, 0, sizeof(cli_info));
	cli_info.address = address_.Server();
	cli_info.ssl_connection = (address_.Protocol() == "wss");

	cli_info.port = address_.Port();
	cli_info.path = address_.Path();

	if (cli_info.port != address_.DefaultPort())
		host_header_.Format(_T(":%d"), cli_info.port);
	host_header_ = address_.Server() + host_header_;
	cli_info.host = host_header_;
	cli_info.origin = "app://pcman";
	cli_info.ietf_version_or_minus_one = -1;
	cli_info.protocol = "telnet";
	cli_info.userdata = new UserData(shared_from_this());
	lws_ = CWebsocketManager::Instance().Connect(&cli_info);

	return true;
}

int CWebsocket::OnCallback(
	struct lws *wsi, enum lws_callback_reasons reason,
	void* user, void *in, size_t len)
{
	switch (reason) {
	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		lwsl_info("lws[%p]: LWS_CALLBACK_CLIENT_ESTABLISHED\n", wsi);
		CheckSecurity();
		delegate_->OnConnect(true);
		break;

	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		lwsl_info("lws[%p]: LWS_CALLBACK_CLIENT_CONNECTION_ERROR\n", wsi);
		delegate_->OnConnect(false);
		break;

	case LWS_CALLBACK_CLOSED:
		lwsl_info("lws[%p]: LWS_CALLBACK_CLOSED\n", wsi);
		delegate_->OnClose();
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
		delegate_->OnReceive(in, len);
		break;

	case LWS_CALLBACK_WSI_DESTROY:
		lwsl_info("lws[%p]: LWS_CALLBACK_WSI_DESTROY\n", wsi);
		{
			std::lock_guard<std::mutex> lk(mu_);
			lws_ = nullptr;
		}
		CWebsocketManager::Instance().RemoveConn();
		// No more callbacks will be sent.
		delete reinterpret_cast<UserData*>(user);
		break;

	case LWS_CALLBACK_OPENSSL_PERFORM_SERVER_CERT_VERIFICATION:
		is_secure_ = len;  // preverify_ok
		break;

	case LWS_CALLBACK_CLIENT_WRITEABLE:
		if (shutdown_.load()) {
			lws_close_reason(lws_, LWS_CLOSE_STATUS_NORMAL, (unsigned char*)"bye", 3);
			return -1;
		}
		break;
	}
	return 0;
}

void CWebsocket::CheckSecurity()
{
	SSL* ssl = lws_get_ssl(lws_);
	if (ssl == nullptr)
		return;
	cipher_ = SSL_get_cipher_name(ssl);
}

int CWebsocket::Send(const void* data, size_t length)
{
	std::lock_guard<std::mutex> lk(mu_);
	if (lws_ == nullptr)
		return -1;

	constexpr size_t kMaxSend = 4096;
	unsigned char buf[LWS_PRE + kMaxSend];
	if (length > kMaxSend)
		return -1;
	memcpy(buf + LWS_PRE, data, length);
	lws_write(lws_, buf + LWS_PRE, length, LWS_WRITE_BINARY);
	return length;
}

void CWebsocket::Shutdown()
{
	std::lock_guard<std::mutex> lk(mu_);
	if (lws_ == nullptr)
		return;
	shutdown_.store(true);
	lws_callback_on_writable(lws_);
	// Signal waiting service thread for changes.
	lws_cancel_service_pt(lws_);
}

void CWebsocket::Close()
{
	Shutdown();
}