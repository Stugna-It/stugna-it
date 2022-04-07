#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <sstream>
#include <string.h>
#include <curl/curl.h>
#include <curl/easy.h>


using namespace std;

namespace utils {
    time_t time_log();
    std::string log_time();

    std::string getHost(std::string uri);
    static size_t WriteCallback(char *data, size_t size, size_t nmemb, std::string *buffer);
    std::string curl_get(const std::string &url,uint32_t timeout=10);


}
#endif // UTILS_H
