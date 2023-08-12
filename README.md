<p align="center">
  <h1 align="center">EHLIB: Embedded http(s) library <br>
  </h1>
</p>

A tiny, C++ network socket wrapper (based on [Mongoose](https://github.com/cesanta/mongoose.git) and [Mbed-TLS](https://github.com/Mbed-TLS/mbedtls.git)) for http(s) requests especially for embedded devices. Inspired by the excellent [CPR library](https://github.com/whoshuu/cpr), [WNetWrap](https://github.com/hack-tramp/WNetWrap.git), it has mostly identical function names, and will likewise work with random parameter order.

## Features

|Implemented| Upcoming|
|:------------:|:----------:|
|Custom headers|Cookie support|
|Url encoded parameters|Self-signed Root CA support, All well-known Root CA support|
|Multipart upload|Change default dns server|
|File upload|Add auth proxy|
|Custom request methods|
|Basic authentication|
|Bearer authentication|
|Request timeout|
|Non-Auth Proxy support|

That is a basic GET request - for detailed examples see the documentation below.

```C++
#include <iostream>
#include "ehlib.h"
    //Tested on Postman Echo
	//GET Request
	ehlib::Response r = ehlib::request(
        ehlib::RequestUrl{"https://postman-echo.com/get"},
        ehlib::UrlRequestParams{
            {"foo1", "bar1"},
            {"foo2", "bar2"}
        },
        ehlib::RequestMethod{"GET"},
        ehlib::RequestUserAgent{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"}
    );

	std::cout << r.raw << std::endl; // basic parser
	std::cout << r.status_code << std::endl; // 200
  ```

## Usage (tested on Linux & Windows)

1. Just copy folders `include`, `lib` into your project.

2. Add static libs and include `ehlib.h` into your build.

3. That's it!

## Documentation

For now it's all here on the readme, but it will eventually be put on a different page to make navigation more user friendly.

To navigate through it use the table of contents dropdown menu.

### GET requests

Making a `GET` request with `Ehlib` is simple - the `GET` method is used by default so doesn't need to be specified:

```C++
#include "ehlib.h"
  ehlib::Response r = ehlib::request(
        ehlib::RequestUrl{"https://postman-echo.com/get"},
        ehlib::RequestUserAgent{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"}
);
```

This gives us a `Response` object which we’ve called r.

There’s a lot of useful stuff in there:

```C++
  std::cout << r.url << std::endl; // https://postman-echo.com/get

  std::cout << r.status_code <<   std::endl; // 200

  std::cout << r.header["Content-type"] << std::endl; // application/json; charset=utf-8

  std::cout << r.raw << std::endl;

/*
  {
    "args": {},
    "headers": {
      "x-forwarded-proto": "https",
      "x-forwarded-port": "443",
      "host": "postman-echo.com",
      "x-amzn-trace-id": "Root=1-64d67257-24fc86582562e80206287bbe",
      "content-length": "0",
      "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"
    },
    "url": "https://postman-echo.com/get"
  }
*/
```

To add URL-encoded parameters, add a `UrlRequestParams` object to the `request` call:

```C++
  auto r = ehlib::request(
        ehlib::RequestUrl{"https://postman-echo.com/get"},
        ehlib::UrlRequestParams{
            {"foo1", "bar1"},
            {"foo2", "bar2"}
        },
        ehlib::RequestUserAgent{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"}
  );

  std::cout << r.url << std::endl; // https://postman-echo.com/get?foo1=bar1&foo2=bar2
  std::cout << r.raw << std::endl;

/*
  {
    "args": {
      "foo1": "bar1",
      "foo2": "bar2"
    },
    "headers": {
      "x-forwarded-proto": "https",
      "x-forwarded-port": "443",
      "host": "postman-echo.com",
      "x-amzn-trace-id": "Root=1-64d674e0-26f038a10e869e521dcdbfdc",
      "content-length": "0",
      "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"
    },
    "url": "https://postman-echo.com/get?foo1=bar1&foo2=bar2"
  }
*/
```

`UrlRequestParams` is an object with a map-like interface.

You can construct it using a list of key/value pairs inside the `request` or have it outlive `request` by constructing it outside:

```C++
// Constructing it in a place
  auto r = ehlib::request(
      ehlib::RequestUrl{"http://www.httpbin.org/get"},
      ehlib::UrlRequestParams{
        {"foo1", "bar1"},
        {"foo2", "bar2"}
      },
      ehlib::RequestUserAgent{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"}
  );

  std::cout << r.url << std::endl; // https://postman-echo.com/get?foo1=bar1&foo2=bar2

  std::cout << r.raw << std::endl;

/*
  {
    "args": {
      "foo1": "bar1",
      "foo2": "bar2"
    },
    "headers": {
      "x-forwarded-proto": "https",
      "x-forwarded-port": "443",
      "host": "postman-echo.com",
      "x-amzn-trace-id": "Root=1-64d67627-0f91d09203e69af636c8fcfb",
      "content-length": "0",
      "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"
    },
    "url": "https://postman-echo.com/get?foo1=bar1&foo2=bar2"
  }
*/

 // Constructing it an outside
  auto parameters = ehlib::UrlRequestParams{
    {"foo1", "bar1"},
    {"foo2", "bar2"}
  };

  auto r_outside = ehlib::request(
    ehlib::RequestUrl{"https://postman-echo.com/get"},
    parameters
  );

  std::cout << r_outside.url << std::endl; // https://postman-echo.com/get?foo1=bar1&foo2=bar2

  std::cout << r_outside.raw << std::endl; // Same text response as above
```

### Download a file

To download the contents of the request you simply add `RequestUrl` to `request`.

File content will return into Response's `raw` field.

For example, to download the CPR library:

```C++
  auto r = ehlib::request(
      ehlib::RequestUrl{ "https://github.com/whoshuu/cpr/archive/refs/tags/1.10.4.zip" },
      ehlib::RequestUserAgent{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"}
  );

  std::cout << r.raw << std::endl; // print donwloaded file's content (our 1.10.4.zip archive bytes)
```

### POST Requests

Making a POST request must use `RequestMethod`:

```C++
  auto r = ehlib::request(
        ehlib::RequestUrl{"https://postman-echo.com/post"},
        ehlib::RequestMethod{"POST"}
  );

  std::cout << r.text << std::endl;

/*
  {
    "args": {},
    "data": {},
    "files": {},
    "form": {},
    "headers": {
      "x-forwarded-proto": "https",
      "x-forwarded-port": "443",
      "host": "postman-echo.com",
      "x-amzn-trace-id": "Root=1-64d688e8-1a56cc76092b9abd7de205d9",
      "content-length": "0",
      "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"
    },
    "json": null,
    "url": "https://postman-echo.com/post"
  }
*/
```

This sends some data `"This is raw POST data"` with use `PostRequestBody`:

```C++
  auto r = ehlib::request(
        ehlib::RequestUrl{"https://postman-echo.com/post"},
        ehlib::PostRequestBody{"This is raw POST data"},
        ehlib::RequestMethod{"POST"}
  );

  std::cout << r.text << std::endl;

/*
  {
    "args": {},
    "data": "This is raw POST data",
    "files": {},
    "form": {},
    "headers": {
      "x-forwarded-proto": "https",
      "x-forwarded-port": "443",
      "host": "postman-echo.com",
      "x-amzn-trace-id": "Root=1-64d689c1-3a281bae0158913d6a4f240a",
      "content-length": "21",
      "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36",
      "content-type": "application/json"
    },
    "json": null,
    "url": "https://postman-echo.com/post"
  }
*/
```

If the data package is large or contains a file, it’s more appropriate to use a `RequestMultipartUpload` upload.

In this example we are uploading a text files to `https://postman-echo.com/post`:

```C++
  auto r = ehlib::request(
        ehlib::RequestUrl{"https://postman-echo.com/post"},
        ehlib::RequestMultipartUpload{
            {"file_path:1.txt", "1.txt"},
            {"file_path:C:/Windows/win.ini", "win.ini"},
            {"some raw data", "2.txt"}
        },
		ehlib::RequestMethod{"POST"}
  );

  std::cout << r.text << std::endl;

/*
  {
    "args": {},
    "data": {},
    "files": {
      "1.txt": "data:application/octet-stream;base64,aGVsbG8gd29ybGQh",
      "win.ini": "data:application/octet-stream;base64,OyBmb3IgMTYtYml0IGFwcCBzdXBwb3J0DQpbZm9udHNdDQpbZXh0ZW5zaW9uc10NClttY2kgZXh0ZW5zaW9uc10NCltmaWxlc10NCltNYWlsXQ0KTUFQST0xDQo="
    },
    "form": {
      "some raw data": "2.txt"
    },
    "headers": {
      "x-forwarded-proto": "https",
      "x-forwarded-port": "443",
      "host": "postman-echo.com",
      "x-amzn-trace-id": "Root=1-64d68e5f-0e0b852e32702ddc7806ff31",
      "content-length": "513",
      "content-type": "multipart/form-data; boundary=1816741701207515830740325151",
      "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"
    },
    "json": null,
    "url": "https://postman-echo.com/post"
  }
*/
```

Notice `file_path:` prefix - this tells that is file on disk and not a key - value pair as form `{"some raw data", "2.txt"}`.

### Authentication

To use [Basic Authentication](https://en.wikipedia.org/wiki/Basic_access_authentication) which uses a username and password, just add `BasicAuth` to the call:

```C++
  auto r = ehlib::request(
        ehlib::RequestUrl{"https://postman-echo.com/basic-auth"},
        ehlib::BasicAuth {"postman", "password"},
		ehlib::RequestMethod{"GET"}
    );

  std::cout << r.text << std::endl;

/*
  {
    "authenticated": true
  }
*/

```

Authentication via an [OAuth - Bearer Token](https://en.wikipedia.org/wiki/OAuth) can be done using the `BearerAuth` authentication object:

```C++
  auto r = ehlib::request(
        ehlib::RequestUrl{"http://www.httpbin.org/bearer"},
        ehlib::BearerAuth {"ACCESS_TOKEN"},
        ehlib::RequestMethod{"GET"}
  );
std::cout << r.text << std::endl;

/*
  {
    "authenticated": true,
    "token": "ACCESS_TOKEN"
  }
*/

```

### Response Objects

A `Response` has these public fields and methods:
```C++
std::string status_code;        // The HTTP(S) status code for the request
std::string raw;                // The data of the HTTP response
std::map header;                // A map of the header fields received
std::map sent_headers;          // A map of the headers sent
std::string url;                // The effective URL of the ultimate request
std::string err;                // An error string containing the error code and a message
unsigned long uploaded_bytes;   // How many bytes have been sent to the server
unsigned long downloaded_bytes; // How many bytes have been received from the server
unsigned long redirect_count;   // How many redirects occurred
void reset(); // Reset Response object to default state
```

### Request Headers

Using `RequestHeaders` in your `request` you can specify custom headers:

```C++
  auto r = ehlib::request(
        ehlib::RequestUrl{"https://postman-echo.com/get"},
        ehlib::RequestHeaders{
            {"accept", "application/json"},
            {"master", "slave"}
        },
        ehlib::RequestMethod{"GET"},
        ehlib::RequestUserAgent{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"}
  );

  std::cout << r.text << std::endl;

/*
  {
    "args": {},
    "headers": {
      "x-forwarded-proto": "https",
      "x-forwarded-port": "443",
      "host": "postman-echo.com",
      "x-amzn-trace-id": "Root=1-64d6950a-387220f76b21c06865853326",
      "content-length": "0",
      "accept": "application/json",
      "master": "slave",
      "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"
    },
    "url": "https://postman-echo.com/get"
  }
*/

```

### Timeouts

It’s possible to set a timeout for your request if you have strict timing requirements:

```C++
	auto r = ehlib::request(
        ehlib::RequestUrl{"https://postman-echo.com/get"},
        ehlib::RequestTimeout{ 10 },
        ehlib::RequestMethod{"GET"},
        ehlib::RequestUserAgent{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"}
    );

  std::cout << r.text << std::endl; // Connection timeout
```

Setting the `RequestTimeout` option sets the maximum allowed time the connection or request operation can take in milliseconds. By default a `RequestTimeout` set in 5000 milliseconds

### Using Proxies

Setting up a proxy is easy. For example, we have tor `http proxy` on `localhost` port `9080`.
Try to connect to `https://duckduckgogg42xjoc72x3sjasowoarfbgcmvfimaftt6twagswzczad.onion/`.
This is official `duckduckgo.com` .onion mirror. To do this we will use `RequestProxy`.

```C++
  auto r = ehlib::request(
        ehlib::RequestUrl{"https://duckduckgogg42xjoc72x3sjasowoarfbgcmvfimaftt6twagswzczad.onion/"},
        ehlib::RequestTimeout{ 100000 },
        ehlib::RequestProxy{ "http://127.0.0.1:9080"},
        ehlib::RequestMethod{"GET"},
        ehlib::RequestUserAgent{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"}
  );

  std::cout << r.raw << std::endl; 
  
  /*
  <!DOCTYPE html>\n<!--[if IEMobile 7 ]> <html lang=\"en-US\" class=\"no-js iem7\"> <![endif]-->\n<!--[if lt IE 7]> <html class=\"ie6 lt-ie10 lt-ie9 lt-ie8 lt-ie7 no-js\" lang=\"en-US\"> <![endif]-->\n<!--[if IE 7]>    <html class=\"ie7 lt-ie10 lt-ie9 lt-ie8 no-js\" lang=\"en-US\"> <![endif]-->\n<!--[if IE 8]>    <html class=\"ie8 lt-ie10 lt-ie9 no-js\" lang=\"en-US\"> <![endif]-->\n<!--[if IE 9]>    <html class=\"ie9 lt-ie10 no-js\" lang=\"en-US\"> <![endif]-->\n<!--[if (gte IE 9)|(gt IEMobile 7)|!(IEMobile)|!(IE)]><!--><html class=\"no-js\" lang=\"en-US\" data-ntp-features=\"tracker-stats-widget:off\"><!--<![endif]-->\n\n<head>\n\t<meta http-equiv=\"X-UA-Compatible\" content=\"IE=Edge\" />\n<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8;charset=utf-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=1\" />\n<meta name=\"HandheldFriendly\" content=\"true\"/>\n<meta name=\"darkreader-lock\" />\n\n<link rel=\"canonical\" href=\"https://duckduckgo.com/duckduckgogg42xjoc72x3sjasowoarfbgcmvfimaftt6twagswzczad.onion\">\n\n<link rel=\"stylesheet\" ...</html>\n
*/
```

### Additional Options

|Allowed options| Upcoming|
|:------------:|:----------:|
|auto-redirect (on/off)|?|


The `RequestOptions` constructor allows you to specify additional options for the request. For example, turn `off` auto-redirects (default: `auto-redirect` turn `on`). To enable the option you can use `on`.

```C++
	auto r = ehlib::request(
        ehlib::RequestUrl{"http://ya.ru"},
        ehlib::RequestOptions { 
            {"auto-redirect", "off"}
        },
        ehlib::RequestUserAgent{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"}
  );

  std::cout << r.status_code << std::endl; // 301
```
