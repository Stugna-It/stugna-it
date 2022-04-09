#ifndef MULTIGUN_H
#define MULTIGUN_H

#include "jobs.h"
#include "proxy.h"
#include "stats.h"
#include "crow_all.h"
#include <curl/curl.h>
#include <chrono>
#include "json.hpp"

using namespace nlohmann;

struct threadStateM {
public:
    std::string state;

    std::string target;
    std::string proxy;

    int count = 0;

    int httpCode = 0;
    int curlCode = 0;


    nlohmann::json toJson() {
        json j;
        j["state"] = this->state;
        j["target"] = this->target;
        j["proxy"] = this->proxy;
        j["count"] = this->count;
        j["httpCode"] = this->httpCode;
        j["curlCode"] = this->curlCode;

        return j;
    }
};

class MultiGun
{
public:
    MultiGun(Jobs * jobs, Proxy * prx, Stats *stat);
    Proxy * prx;
    Jobs  * jobs;
    Stats * stat;
    // one thread limit ++
    uint limitTime = 5*60;
    uint limitRequests = 100;

    uint reqPerProxy = 100;  //change proxy each % requests
    uint reqPerJob = 2; // change job each % requests
    bool rotateJobs = true;

    //one thread limit --
    uint threadsCount = 8;
    uint batch_requests = 4 * 1024;

    std::vector<std::thread *> threads;

    void reload(Jobs *jobs);
    void httpGet(uint thId);

    std::map<int,threadStateM> thState;

    std::thread * watcherTh;
    void watcher();

    void fire();
    time_t startTime = 0;

};
#endif // MULTIGUN_H
