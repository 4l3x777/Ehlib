#include "ehlib.h"
#include "utils.h"

#include "mongoose/mongoose.h"

// unique for threads
thread_local ehlib::Request ehlib::thread_structures::toSource;
thread_local ehlib::Response ehlib::thread_structures::fromSource;

void mongoose_parsing_response(void* ev_data)
{
	// Response is received.
	struct mg_http_message* hm = (struct mg_http_message*)ev_data;
	ehlib::thread_structures::fromSource.downloaded_bytes = hm->body.len;
	ehlib::thread_structures::fromSource.raw = std::string(hm->body.ptr, hm->body.len);
	ehlib::thread_structures::fromSource.status_code = std::string(hm->uri.ptr, hm->uri.len);
	for (auto i = 0; i < MG_MAX_HTTP_HEADERS; i++)
	{
		if (std::string(hm->headers[i].name.ptr, hm->headers[i].name.len) == "Location")
		{
			ehlib::thread_structures::fromSource.url = std::string(hm->headers[i].value.ptr, hm->headers[i].value.len);
		}
		ehlib::thread_structures::fromSource.header.insert(
			{
				std::string(hm->headers[i].name.ptr, hm->headers[i].name.len),
				std::string(hm->headers[i].value.ptr, hm->headers[i].value.len)
			}
		);
	}
	ehlib::thread_structures::fromSource.sent_headers = ehlib::thread_structures::toSource.Header.hdr;
}

void mongoose_request(struct mg_connection* c)
{
	struct mg_str host = mg_url_host(ehlib::thread_structures::toSource.url.c_str());
	//prepate headers
	std::string headers;
	for (auto iter = ehlib::thread_structures::toSource.Header.hdr.begin(); iter != ehlib::thread_structures::toSource.Header.hdr.end(); ++iter)
	{
		headers += iter->first + ": " + iter->second + "\r\n";
	}
	headers += "User-Agent: " + ehlib::thread_structures::toSource.user_agent + "\r\n";

	// Send request
	int content_length = ehlib::thread_structures::toSource.post_data.size();
	ehlib::thread_structures::fromSource.downloaded_bytes = content_length;

	std::string request_template = "%s %s HTTP/1.1\r\nHost: %.*s\r\n%sContent-Length: %d\r\n\r\n";

	mg_printf(c,
		request_template.c_str(),
		ehlib::thread_structures::toSource.method.c_str(), mg_url_uri(ehlib::thread_structures::toSource.url.c_str()), (int)host.len, host.ptr, headers.c_str(), content_length);
	mg_send(c, ehlib::thread_structures::toSource.post_data.c_str(), content_length);
}

void mongoose_proxy(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
	// get response message
	if (ev == MG_EV_OPEN)
	{
		// Connection created. Store connect expiration time in c->data
		*(uint64_t*)c->data = mg_millis() + ehlib::thread_structures::toSource.timeout;
	}
	else if (ev == MG_EV_POLL)
	{
		if (mg_millis() > *(uint64_t*)c->data &&
			(c->is_connecting || c->is_resolving)
		)
		{
			mg_error(c, "Connection timeout");
		}
	}
	else if (ev == MG_EV_HTTP_MSG)
	{
		mongoose_parsing_response(ev_data);
		c->is_closing = 1;         // Tell mongoose to close this connection
		*(bool*)fn_data = true;  // Error, tell event loop to stop
	}
	else if (ev == MG_EV_CONNECT)
	{
		// Proxy TCP connection established. Send CONNECT request
		struct mg_str host = mg_url_host(ehlib::thread_structures::toSource.url.c_str());
		std::string request_template = "CONNECT %.*s:%hu HTTP/1.1\r\nHost: %.*s:%hu\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n";

		mg_printf(
			c,
			request_template.c_str(),
			(int)host.len,
			host.ptr,
			mg_url_port(ehlib::thread_structures::toSource.url.c_str()),
			(int)host.len,
			host.ptr,
			mg_url_port(ehlib::thread_structures::toSource.url.c_str()),
			ehlib::thread_structures::toSource.user_agent.c_str()
		);
	}
	else if (!ehlib::thread_structures::toSource.proxy_use && ev == MG_EV_READ)
	{
		struct mg_http_message hm;
		int n = mg_http_parse((char*)c->recv.buf, c->recv.len, &hm);
		if (n > 0)
		{
			struct mg_str host = mg_url_host(ehlib::thread_structures::toSource.url.c_str());

			MG_DEBUG(("Connected to proxy, status: %.*s", (int) hm.uri.len, hm.uri.ptr));
			auto status_code = hm.uri.len > 0 ? std::stoi(std::string(hm.uri.ptr)) : 0;

			if (status_code >= 200 && status_code < 300)
			{ 
				// CONNECT response - tunnel is established
				ehlib::thread_structures::toSource.proxy_use = true;
				
				// clear context
				mg_iobuf_del(&c->recv, 0, n);
				mg_iobuf_free(&c->send);
				mg_iobuf_free(&c->recv);
				mg_tls_free(c);
				c->is_closing = 0;

				// check ssl required
				if (mg_url_is_ssl(ehlib::thread_structures::toSource.url.c_str())) 
				{
					mg_tls_init(c, host);
				}

				// request
				mongoose_request(c);
			}
			else return mg_error(c, "Connect to proxy failed");
		}
	}
	else if (ev == MG_EV_ERROR)
	{
		ehlib::thread_structures::fromSource.err = std::string((char*)ev_data);
		struct mg_http_message* hm = (struct mg_http_message*)ev_data;
		c->is_closing = 1;         // Tell mongoose to close this connection
		*(bool*)fn_data = true;  // Error, tell event loop to stop
	}
}

void mongoose_direct(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
	if (ev == MG_EV_OPEN)
	{
		// Connection created. Store connect expiration time in c->data
		*(uint64_t*)c->data = mg_millis() + ehlib::thread_structures::toSource.timeout;
	}
	else if (ev == MG_EV_POLL)
	{
		if (mg_millis() > *(uint64_t*)c->data &&
			(c->is_connecting || c->is_resolving)
		)
		{
			mg_error(c, "Connection timeout");
		}
	}
	else if (ev == MG_EV_CONNECT)
	{
		mongoose_request(c);
	}
	else if (ev == MG_EV_HTTP_MSG)
	{
		// parsing response
		mongoose_parsing_response(ev_data);
		c->is_closing = 1;         // Tell mongoose to close this connection
		*(bool*)fn_data = true;  // Tell event loop to stop
	}
	else if (ev == MG_EV_ERROR)
	{
		ehlib::thread_structures::fromSource.err = std::string((char*)ev_data);
		c->is_closing = 1;         // Tell mongoose to close this connection
		*(bool*)fn_data = true;  // Tell event loop to stop
	}
}

void ehlib::mongoose_engine_adapter::request()
{
	try
	{
		// check request with params
		if (!ehlib::thread_structures::toSource.params.empty()) 
		{
			ehlib::thread_structures::toSource.url += ehlib::thread_structures::toSource.params;
		}

		ehlib::thread_structures::fromSource.url = ehlib::thread_structures::toSource.url;

		struct mg_mgr mgr;              // Event manager
		bool done = false;              // Event handler flips it to true
	#if defined(DEBUG)
		const char* log_level = getenv("LOG_LEVEL");  // Allow user to set log level
		if (log_level == NULL) log_level = "4";       // Default is verbose
		mg_log_set(atoi(log_level));    // Set to 0 to disable debug
	#else
		mg_log_set(0);
	#endif
		mg_mgr_init(&mgr);              // Initialise event manager

		// set dns timeout
		mgr.dnstimeout = ehlib::thread_structures::toSource.timeout;

		// TODO: Change default dns server
		//mgr.dns4.url = "udp://77.88.8.88:53";

		struct mg_tls_opts opts = 
		{
			//TODO: add support self-signed Root CA & well-known Root CA
			//.client_ca = mg_str(CA_ALL)
		};
		mg_tls_ctx_init(&mgr, &opts);

		// create client connection
		if (thread_structures::toSource.proxy_use)
		{
			// set false for check established proxy tunnel
			thread_structures::toSource.proxy_use = false;
			mg_http_connect(&mgr, thread_structures::toSource.Proxy.url.c_str(), mongoose_proxy, &done);//mg_http_connect(&mgr, toSource.url.c_str(), mongoose_proxy, &done);
		}
		else
		{
			mg_http_connect(&mgr, thread_structures::toSource.url.c_str(), mongoose_direct, &done);
		}

		auto timeout_time = mg_millis() + thread_structures::toSource.timeout;
		while (!done)
		{
			if (mg_millis() > timeout_time)
			{
				thread_structures::fromSource.err = "Connection timeout";
				break;
			}
			mg_mgr_poll(&mgr, 50);      // Event manager loops until 'done'
		}
		mg_mgr_free(&mgr);

		if (!ehlib::thread_structures::fromSource.err.empty()) return;

		if (ehlib::thread_structures::fromSource.status_code.empty()) return;

		// auto-redirect check
		if (std::stoi(thread_structures::fromSource.status_code) > 299 && std::stoi(thread_structures::fromSource.status_code) < 400)
		{
			if (ehlib::thread_structures::toSource.auto_redirect_use)
			{
				thread_structures::fromSource.redirect_count++;
				thread_structures::toSource.url = thread_structures::fromSource.url;
				request();
			}
		}
	}
	catch (std::exception e)
	{
		ehlib::thread_structures::fromSource.err = e.what();
	}
}