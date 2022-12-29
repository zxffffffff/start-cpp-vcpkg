#include "SampleNet.h"
#include "uv.h"
#include "curl/curl.h"

SampleNet::SampleNet()
{
    std::cout << "Hello SampleNet." << std::endl;

    std::cout << "libuv version: " << uv_version_string() << std::endl;
    {
        //static uv_loop_t loop;
        //uv_tcp_t server;
        //uv_tcp_init(&loop, &server);
        //struct sockaddr_in addr;
        //uv_ip4_addr("127.0.0.1", 12345, &addr);
        //uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
        //int ret = uv_listen((uv_stream_t*)&server, 128, [](uv_stream_t* server, int status) {
        //    uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
        //    uv_tcp_init(&loop, client);
        //    if (uv_accept(server, (uv_stream_t*)client) == 0) {
        //        //do_some_thind();
        //    }
        //});
        //assert(ret == 0);
    }

    std::cout << "curl version: " << curl_version() << std::endl;
    {
        CURL* curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        assert(curl);
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com");
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
          fprintf(stderr, " curl_easy_perform () failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
}

int SampleNet::Test(int ret)
{
    return ret;
}
