#include "sentinel/backend/HTTPBackend.h"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

namespace sentinel
{

const HTTPBackend&
HTTPBackend::get()
{
    static HTTPBackend backend;
    return backend;
}

std::string
HTTPBackend::escapeString(const std::string& inStr) const
{
    char* curlStr = curl_easy_escape(_curl, inStr.data(), inStr.size());
    const std::string retStr(curlStr);
    curl_free(curlStr);
    return retStr;
}

std::string
HTTPBackend::performGETRequest(const std::string& host, const std::string& target) const
{
    // Copied from Boost Beast quick start:
    // https://www.boost.org/doc/libs/1_70_0/libs/beast/doc/html/beast/quick_start/http_client.html
    // The io_context is required for all I/O
    net::io_context ioc;

    // These objects perform our I/O
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    // Look up the domain name
    auto const results = resolver.resolve(host.data(), "80");

    // Make the connection on the IP address we get from a lookup
    stream.connect(results);

    // Set up an HTTP GET request message
    http::request<http::string_body> req{http::verb::get, target.data(), 11};
    req.set(http::field::host, host.data());
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send the HTTP request to the remote host
    http::write(stream, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::string_body> res;

    // Receive the HTTP response
    http::read(stream, buffer, res);

    // Gracefully close the socket
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    return res.body();
}

std::string
HTTPBackend::performGETRequestSSL(const std::string& host, const std::string& target) const
{
    // Copied from Boost Beast documentation:
    // https://www.boost.org/doc/libs/1_70_0/libs/beast/example/http/client/sync-ssl/http_client_sync_ssl.cpp
    // The io_context is required for all I/O
    net::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx(ssl::context::tlsv12_client);

    // Verify the remote server's certificate
    ctx.set_verify_mode(ssl::verify_peer);

    // These objects perform our I/O
    tcp::resolver resolver(ioc);
    beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if(! SSL_set_tlsext_host_name(stream.native_handle(), host.data()))
    {
        beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        throw beast::system_error{ec};
    }

    // Look up the domain name
    auto const results = resolver.resolve(host.data(), "443");

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(stream).connect(results);

    // Perform the SSL handshake
    stream.handshake(ssl::stream_base::client);

    // Set up an HTTP GET request message
    http::request<http::string_body> req{http::verb::get, target.data(), 11};
    req.set(http::field::host, host.data());
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send the HTTP request to the remote host
    http::write(stream, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::string_body> res;

    // Receive the HTTP response
    http::read(stream, buffer, res);

    // Gracefully close the stream
    beast::error_code ec;
    stream.shutdown(ec);
    return res.body();
}

HTTPBackend::HTTPBackend()
{
    curl_global_init(CURL_GLOBAL_ALL);
    _curl = curl_easy_init();
}

HTTPBackend::~HTTPBackend()
{
    curl_easy_cleanup(_curl);
    curl_global_cleanup();
}

}
