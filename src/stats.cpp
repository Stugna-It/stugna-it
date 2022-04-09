#include "stats.h"

using namespace nlohmann;

Stats::Stats()
{

}


void Stats::push(std::string target,std::string prx,int curlCode,int httpCode) {

    if (curlCode > 0) {
        this->targets[target].push("curl",curlCode);
        this->proxies[prx].push("curl",curlCode);

        this->targets[target].count_curl += 1;
        this->proxies[prx].count_curl += 1;
    }
    if (httpCode > 0) {

        this->targets[target].push("http",httpCode);
        this->proxies[prx].push("http",httpCode);

        this->targets[target].count_http += 1;
        this->proxies[prx].count_http += 1;
    }

    this->proxies[prx].count += 1;
    this->targets[target].count += 1;

}

void Stats::pushBytes(std::string target,std::string prx,double tx,double rx) {

    this->targets[target].tx += tx;
    this->targets[target].rx += rx;

    this->proxies[prx].tx += tx;
    this->proxies[prx].rx += rx;


}

std::string Stats::toJson() {
    json j;
    json targetsJ;
    for (auto const &tg: this->targets) {
        targetsJ[tg.first]["stat"] = tg.second.stat;
        //targetsJ[tg.first]["rxtx"] = std::pair<double,double>(tg.second.rx,tg.second.tx);
        targetsJ[tg.first]["count"] = tg.second.count;
        targetsJ[tg.first]["count_curl"] = tg.second.count_curl;
        targetsJ[tg.first]["count_http"] = tg.second.count_http;
    }
    json proxyJ;
    for (auto const &tg: this->proxies) {
        proxyJ[tg.first] = tg.second.stat;
        //proxyJ[tg.first]["rxtx"] = std::pair<double,double>(tg.second.rx,tg.second.tx);
        proxyJ[tg.first]["count"] = tg.second.count;
        proxyJ[tg.first]["count_curl"] = tg.second.count_curl;
        proxyJ[tg.first]["count_http"] = tg.second.count_http;
    }
    j["targets"] = targetsJ;
    j["proxies"] = proxyJ;

    return j.dump();
}


void codeStat::push(std::string type,int code) {
   if (stat[type].find(code) == stat[type].end()) {
       stat[type][code] = 1;
   } else {
       stat[type][code]++;
   }
}
