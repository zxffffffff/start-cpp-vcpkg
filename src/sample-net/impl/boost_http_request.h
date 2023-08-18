/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../http_interface.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/url/url_view.hpp>
#include <boost/algorithm/string.hpp>

class HttpParserImpl : public IHttpParser
{
public:
    virtual Error ParseReq(Buffer buffer, HttpRequest& out_req) override
    {
        boost::beast::error_code ec;
        boost::beast::http::request_parser<boost::beast::http::string_body> parser;
        parser.put(boost::asio::buffer(buffer->data(), buffer->size()), ec);
        if (!parser.is_done())
        {
            // error
            return MakeError(-1, ec.message());
        }
        // success
        boost::beast::http::request<boost::beast::http::string_body> request = parser.get();
        boost::urls::url_view url(request.body());

        out_req.method = request.method_string();
        out_req.path = url.path();
        out_req.parameters.clear();
        std::vector<std::string> keyValues;
        boost::algorithm::split(keyValues, url.query(), boost::is_any_of("&"));
        for (const auto& keyValue : keyValues) {
            std::vector<std::string> keyValueSplit;
            boost::algorithm::split(keyValueSplit, keyValue, boost::is_any_of("="));
            if (keyValueSplit.size() == 2) {
                out_req.parameters[keyValueSplit[0]] = keyValueSplit[1];
            }
        }
        return MakeSuccess();
    }

    virtual Buffer MakeRes(const HttpRequest& req, const HttpResponse& res) override
    {
        return MakeBuffer(std::string(__func__));
    }
};
