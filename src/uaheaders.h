#ifndef UAHEADERS_H
#define UAHEADERS_H

#include <vector>
#include <map>
#include <string>
#include <curl/curl.h>


class UaHeaders
{
public:
    UaHeaders();

    curl_slist * getRand();
    std::map<std::string,curl_slist *> headers;

};

#endif // UAHEADERS_H
