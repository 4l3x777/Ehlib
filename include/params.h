#pragma once

#include <string>
#include <map>

namespace ehlib
{
    class PostRequestBody {
		public:
			std::string body;
	};

	class RequestUpload {
		public:
			std::string file;
	};

	class RequestMethod {
		public:
			std::string method;
	};

	class RequestUrl {
		public:
			std::string adr;
	};

	class RequestUserAgent {
		public:
			std::string usr;
	};

	class RequestHeaders {
		public:
			std::map<std::string, std::string> hdr;
			RequestHeaders(std::initializer_list<std::pair<const std::string, std::string> > hdr) : hdr(hdr) {}
	};

	class UrlRequestParams {
		public:
			std::map<std::string, std::string> p;
			UrlRequestParams(std::initializer_list<std::pair<const std::string, std::string> > p) : p(p) {}
	};

	class RequestMultipartUpload {
		public:
			std::map<std::string, std::string> mp;
			RequestMultipartUpload(std::initializer_list<std::pair<const std::string, std::string> > mp) : mp(mp) {}
	};

	class BasicAuth {
		public:
			std::string usr;
			std::string pwd;
	};

	class BearerAuth {
		public:
			std::string token;
	};

	class RequestTimeout {
		public:
			size_t timeout;
	};

	class RequestOptions {
		public:
			std::map<std::string, std::string> options;
			RequestOptions(std::initializer_list<std::pair<const std::string, std::string> > options) : options(options) {}
	};

	class RequestProxy {
		public:
			std::string url;
			std::string usr = "";
			std::string pwd = "";
	};

	namespace params_handlers
	{
		void Params(ehlib::UrlRequestParams s);

		void Params(ehlib::RequestUrl u);

		void Params(ehlib::RequestUserAgent usr);

		void Params(ehlib::RequestHeaders h);

		void Params(ehlib::BasicAuth auth);

		void Params(ehlib::BearerAuth token);

		void Params(ehlib::RequestMethod m);

		void Params(ehlib::PostRequestBody body);

		void Params(ehlib::RequestUpload file);

		void Params(ehlib::RequestTimeout timeout);

		void Params(ehlib::RequestOptions options);

		void Params(ehlib::RequestProxy proxy);

		void Params(ehlib::RequestMultipartUpload mp);
	}

	class Request
	{
		public:
			std::string user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36";
			std::string params;
			std::string post_data;
			std::string method = "GET";
			std::string Auth;
			std::string url = "";
			ehlib::RequestProxy Proxy;
			bool auto_redirect_use = true;
			bool proxy_use = false;
			RequestHeaders Header = {};
			size_t timeout = 5000;

			void reset()
			{
				user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36";
				params = "";
				post_data = "";
				method = "GET";
				Auth = "";
				url = "";
				Header = {};
				timeout = 5000;
				proxy_use = false;
				auto_redirect_use = true;
			};
	};

	namespace map_comparator
	{
		class Comparator
		{
			public:
				bool operator() (const std::string& s1, const std::string& s2) const
				{
					std::string str1;
					std::string str2;
					for (auto& c : s1) str1 += std::tolower(c);
					for (auto& c : s2) str2 += std::tolower(c);
					return  str1 < str2;
				}
		};
	}

	class Response
	{
		public:
			std::map <std::string, std::string, map_comparator::Comparator> header;
			std::map <std::string, std::string> sent_headers;
			std::string url = "";
			std::string raw = "";
			std::string status_code = "0";
			std::string err = "";
			unsigned long uploaded_bytes = 0;
			unsigned long downloaded_bytes = 0;
			unsigned long redirect_count = 0;

			void reset()
			{
				header.clear();
				sent_headers.clear();
				url.clear();
				raw.clear();
				status_code.clear();
				err.clear();
				uploaded_bytes = 0;
				downloaded_bytes = 0;
				redirect_count = 0;
			}
	};

	//initial Request object that will be used to pass params to cpp source function
	namespace thread_structures
	{
		extern thread_local Request toSource;
		extern thread_local Response fromSource;
	}
}