#ifndef STATS_H
#define STATS_H

#include <map>
#include <string>

#include "crow_all.h"
#include "json.hpp"

class codeStat {


public:
    std::map<std::string,std::map<int,int>> stat;
    void push(std::string type,int code);
    size_t count = 0;
    size_t count_curl = 0; //
    size_t count_http = 0;

    double tx = 0;
    double rx = 0;
};


class Stats
{
public:
    Stats();

    std::map<std::string,codeStat> targets;
    std::map<std::string,codeStat> proxies;

    void push(std::string target,std::string prx,int curlCode,int httpCode);

    void pushBytes(std::string target,std::string prx,double tx,double rx);

    std::string toJson();

};

#endif // STATS_H
