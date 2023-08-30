/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../interface/http_interface.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/url/url_view.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

class HttpParserImpl : public IHttpParser
{
public:
    virtual Error ParseReq(Buffer buffer, HttpRequest& out_req) override
    {
        namespace beast = boost::beast;
        namespace http = boost::beast::http;

        beast::error_code ec;
        http::request_parser<http::string_body> parser;
        parser.put(boost::asio::buffer(buffer->data(), buffer->size()), ec);
        if (ec)
        {
            // error
            std::cerr << ec.message() << std::endl;
            return MakeError(-1, ec.message());
        }
        // success
        http::request<http::string_body> request = parser.get();
        boost::urls::url_view url(request.target());

        auto target = request.target();
        auto body = request.body();
        auto query = url.query();

        out_req.method = request.method_string();
        out_req.path = url.path();

        std::vector<std::string> keyValues;
        auto add_params = [&](const std::vector<std::string> &keyValues)
        {
            for (const auto &keyValue : keyValues)
            {
                std::vector<std::string> keyValueSplit;
                boost::algorithm::split(keyValueSplit, keyValue, boost::is_any_of("="));
                if (keyValueSplit.size() == 2)
                {
                    out_req.parameters[keyValueSplit[0]] = keyValueSplit[1];
                }
            }
        };

        boost::algorithm::split(keyValues, url.query(), boost::is_any_of("&"));
        add_params(keyValues);

        if ("POST" == out_req.method)
        {
            std::string data(buffer->data(), buffer->size());
            std::size_t bodyStart = data.find("\r\n\r\n") + 4;
            std::size_t bodyLength = data.size() - bodyStart;
            out_req.post_body = data.substr(bodyStart, bodyLength);
        }
        return MakeSuccess();
    }

    virtual std::string MakeRes(std::string body) override
    {
        namespace beast = boost::beast;
        namespace http = boost::beast::http;

        http::response<http::string_body> response;
        response.version(11); // HTTP/1.1
        response.result(http::status::ok);
        response.set(http::field::server, "zxf-server");
        response.set(http::field::content_type, "text/plain");
        response.body() = std::move(body);
        response.prepare_payload(); /* 准备响应体 */

        /* 手动构建 HTTP 响应字符串 */
        std::stringstream ss;
        ss << "HTTP/1.1 " << response.result_int() << " " << response.reason() << "\r\n";
        ss << "Server: " << response[http::field::server] << "\r\n";
        ss << "Content-Length: " << response.body().size() << "\r\n";
        ss << "\r\n";
        ss << response.body() << "\r\n";

        /* 也可以使用序列化器 */
        // http::serializer<false, http::string_body> serializer(response);
        return ss.str();
    }
};
