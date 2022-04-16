#include "uaheaders.h"

UaHeaders::UaHeaders()
{

    auto ff = headers["ff99"];
    ff = curl_slist_append(ff, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:99.0) Gecko/20100101 Firefox/99.0");
    ff = curl_slist_append(ff, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
    ff = curl_slist_append(ff, "Accept-Language: ru-RU,ru;q=0.5");
    ff = curl_slist_append(ff, "Accept-Encoding: gzip, deflate, br");
    ff = curl_slist_append(ff, "Connection: keep-alive");
    ff = curl_slist_append(ff, "Upgrade-Insecure-Requests: 1");
    ff = curl_slist_append(ff, "Sec-Fetch-Dest: document");
    ff = curl_slist_append(ff, "Sec-Fetch-Mode: navigate");
    ff = curl_slist_append(ff, "Sec-Fetch-Site: none");
    ff = curl_slist_append(ff, "Sec-Fetch-Dest: document");
    ff = curl_slist_append(ff, "Sec-Fetch-User: ?1");

    auto chrome = headers["chrome"];
    chrome = curl_slist_append(chrome,"accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
    chrome = curl_slist_append(chrome,"accept-encoding: gzip, deflate, br");
    chrome = curl_slist_append(chrome,"accept-language: ru-RU,en-US;q=0.9,en;q=0.8");
    chrome = curl_slist_append(chrome,"sec-ch-ua: \" Not A;Brand\";v=\"99\", \"Chromium\";v=\"100\", \"Google Chrome\";v=\"100\"");
    chrome = curl_slist_append(chrome,"sec-ch-ua-mobile: ?0");
    chrome = curl_slist_append(chrome,"sec-ch-ua-platform: \"Windows\"");
    chrome = curl_slist_append(chrome,"sec-fetch-dest: document");
    chrome = curl_slist_append(chrome,"sec-fetch-mode: navigate");
    chrome = curl_slist_append(chrome,"sec-fetch-site: none");
    chrome = curl_slist_append(chrome,"sec-fetch-user: ?1");
    chrome = curl_slist_append(chrome,"upgrade-insecure-requests: 1");
    chrome = curl_slist_append(chrome,"user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.127 Safari/537.36");

}


curl_slist * UaHeaders::getRand() {
    auto it = headers.begin();
    std::advance(it, rand() % headers.size());
    return it->second;
}
