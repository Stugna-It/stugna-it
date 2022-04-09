#ifndef PROXY_H
#define PROXY_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <shared_mutex>
#include "utils.h"
#include "stats.h"

class Proxy
{
public:
    Proxy();

    std::string url;
    std::string fileName;

    std::shared_mutex proxyLock;
    bool loadUrl();
    bool load(std::string fileName);

    std::string getRand();

    uint cleanBad(Stats * stat);
    size_t count();

private:
    std::vector<std::string> prx;

};

#endif // PROXY_H
