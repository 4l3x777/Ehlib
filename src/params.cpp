#include "params.h"
#include "utils.h"
#include "ehlib.h"

#include <fstream>
#include <iterator>

#include "mimetype/mime_type.h"

void ehlib::params_handlers::Params(ehlib::UrlRequestParams p)
{
	//assemble parameters from map
	std::string final_params = "";
	int pcount = 0;
	for (auto elem : p.p)
	{
		pcount++;
		if (elem.first != "")
        {
			if (pcount > 1) final_params += '&';
			final_params += url_encode(elem.first) + '=' + url_encode(elem.second);
		}
	}
	ehlib::thread_structures::toSource.params = "?" + final_params;
}

void ehlib::params_handlers::Params(ehlib::RequestUrl u)
{
	ehlib::thread_structures::toSource.url = u.adr;
}

void ehlib::params_handlers::Params(ehlib::RequestProxy p)
{
	ehlib::thread_structures::toSource.Proxy = p;
	ehlib::thread_structures::toSource.proxy_use = true;
}

void ehlib::params_handlers::Params(ehlib::RequestUserAgent usr)
{
	ehlib::thread_structures::toSource.user_agent = usr.usr;
}

void ehlib::params_handlers::Params(ehlib::RequestHeaders h)
{
	ehlib::thread_structures::toSource.Header.hdr.insert(h.hdr.begin(), h.hdr.end());
}

void ehlib::params_handlers::Params(ehlib::RequestMethod m)
{
	ehlib::thread_structures::toSource.method = m.method;
}

void ehlib::params_handlers::Params(ehlib::RequestUpload f)
{
	if (f.file.substr(0, 10) == "file_path:")
    {
		std::string file_path = f.file.substr(10);
		// read entire file into string
		if (std::ifstream inputstream{ file_path, std::ios::in | std::ifstream::binary })
        {
			std::string finput;
			std::istreambuf_iterator<char> iter(inputstream);
			copy(iter, std::istreambuf_iterator<char>{}, back_inserter(finput));
			ehlib::thread_structures::toSource.post_data = finput;
		}
	}
	else ehlib::thread_structures::toSource.post_data = f.file;
}

void ehlib::params_handlers::Params(ehlib::PostRequestBody b)
{
	ehlib::thread_structures::toSource.post_data = b.body;
}

void ehlib::params_handlers::Params(ehlib::BasicAuth auth)
{
	std::string Authorization = "Authorization";
	std::string Basic = "Basic ";
	std::string basicauth = auth.usr + ':' + auth.pwd;
	basicauth = base64_encode(basicauth);
	if (ehlib::thread_structures::toSource.Header.hdr.find(Authorization) != ehlib::thread_structures::toSource.Header.hdr.end())
    {
		ehlib::thread_structures::toSource.Header.hdr[Authorization] = Basic + basicauth;
	}
	else
    {
		ehlib::thread_structures::toSource.Header.hdr.insert(std::pair<std::string, std::string>(Authorization, Basic + basicauth));
	}
}

void ehlib::params_handlers::Params(ehlib::BearerAuth token)
{
	std::string Authorization = "Authorization";
	std::string Bearer = "Bearer ";
	if (ehlib::thread_structures::toSource.Header.hdr.find(Authorization) != ehlib::thread_structures::toSource.Header.hdr.end())
    {
		ehlib::thread_structures::toSource.Header.hdr[Authorization] = Bearer + token.token;
	}
	else
    {
		ehlib::thread_structures::toSource.Header.hdr.insert(std::pair<std::string, std::string>(Authorization, Bearer + token.token));
	}
}

void ehlib::params_handlers::Params(ehlib::RequestOptions o)
{
	for (auto elem : o.options)
    {
		if (elem.first.find("auto-redirect") != std::string::npos)
        {
			if ((elem.second.find("off") != std::string::npos) || (elem.second.find("false") != std::string::npos))
            {
				ehlib::thread_structures::toSource.auto_redirect_use = false;
			}
			if ((elem.second.find("on") != std::string::npos) || (elem.second.find("true") != std::string::npos))
            {
				ehlib::thread_structures::toSource.auto_redirect_use = true;
			}
		}
	}
}

void ehlib::params_handlers::Params(ehlib::RequestTimeout timeout)
{
	ehlib::thread_structures::toSource.timeout = timeout.timeout;
}

void ehlib::params_handlers::Params(ehlib::RequestMultipartUpload mp)
{
	//assemble multipart post payload from map
	std::string final_postdata = "";
	std::string boundary = generate_random_boundary();
	ehlib::thread_structures::toSource.Header.hdr["content-type"] = "multipart/form-data; boundary=" + boundary;
	std::string fname, ctype, file_path;
	int pcount = 0;
	for (auto elem : mp.mp)
	{
		pcount++;

		if (elem.first.substr(0, 10) != "file_path:")
        { // if theres no file_path: prefix, assume this is not a file

			final_postdata += "--" + boundary + "\r\n";
			final_postdata += "Content-Disposition: form-data; name=\"" + (elem.first) + "\"\r\n";
			final_postdata += "Content-Type: text/plain\r\n\r\n";
			final_postdata += (elem.second) + "\r\n";
		}
		else { //if we're sending a file

			fname = elem.second.substr(elem.second.find_last_of("/\\") + 1); //used for filename
			ctype = "application/octet-stream"; //used as default for file content-type

			//try to see if we can match the MIME type, by getting the file extension
			if (fname.find_last_of('.') != std::string::npos) { //use find_last_of to ignore dots in filename
				std::string extension = fname.substr(fname.find_last_of('.'));
				if (MimeTypes::getType(extension.c_str()) != NULL) {
					ctype = MimeTypes::getType(extension.c_str());
				}
			}

			final_postdata += "--" + boundary + "\r\n";
			final_postdata += "Content-Disposition: form-data; name=\"" + fname + "\"; filename=\"" + fname + "\"\r\n";
			final_postdata += "Content-Type: " + ctype + "\r\n\r\n";


			file_path = elem.first.substr(10);
			// read entire file into string
			if (std::ifstream inputstream{ file_path, std::ios::in | std::ifstream::binary })
			{
				std::string finput;
				std::istreambuf_iterator<char> iter(inputstream);
				copy(iter, std::istreambuf_iterator<char>{}, back_inserter(finput));
				final_postdata += finput + "\r\n"; //add to postdata
			}
		}

		if (pcount == mp.mp.size())
		{
			final_postdata += "--" + boundary + "--";
		}
	}
	ehlib::thread_structures::toSource.post_data = final_postdata;
}