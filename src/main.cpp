

#include <iostream>
#include <thread>
#include <sys/resource.h>
#include <limits>

#define CROW_MAIN
#define CROW_DISABLE_STATIC_DIR
// #define CROW_ENABLE_COMPRESSION
#include "crow_all.h"

#include "httpserver.h"
#include "jobs.h"
#include "proxy.h"
#include "utils.h"
#include "stats.h"
#include "multigun.h"

using namespace std;


int limits() {
    struct rlimit limit = {0,0};
    if ( getrlimit(RLIMIT_NOFILE, &limit) )
    {
        std::cout<<utils::log_time() << "getrlimit() error = \"" << strerror(errno) << "\"" << std::endl;
        return 1;
    }
    std::cout<<utils::log_time() << "Change resource limits for process"<<std::endl;
    std::cout<<utils::log_time() << "rlim_cur = " <<  limit.rlim_cur << std::endl;
    std::cout<<utils::log_time() << "rlim_max = " <<  limit.rlim_max << std::endl;
    limit.rlim_cur = limit.rlim_cur * 20;
    limit.rlim_max = limit.rlim_cur * 20 * 2;
    if (setrlimit(RLIMIT_NOFILE, &limit)) {
        std::cout<<utils::log_time() << "setrlimit() error = \"" << strerror(errno) << "\"" << std::endl;
        return 1;
    }
    if (getrlimit(RLIMIT_NOFILE, &limit)) {
        std::cout<<utils::log_time() << "getrlimit() error = \"" << strerror(errno) << "\"" << std::endl;
        return 1;
    }
    std::cout<<utils::log_time() << "rlim_cur = " <<  limit.rlim_cur << std::endl;
    std::cout<<utils::log_time() << "rlim_max = " <<  limit.rlim_max << std::endl;
    return 0;
}

int main()
{
    json conf;
    try {
        std::ifstream ifs("config.json");
        conf = json::parse(ifs);
    } catch (...) {
        std::cout << "Config not found (config.json) or not valid json, using defaults " << std::endl;
        conf["jobs_uri"] = "https://raw.githubusercontent.com/db1000n-coordinators/LoadTestConfig/main/config.v0.7.json";
        conf["proxy_list_uri"] = "https://raw.githubusercontent.com/porthole-ascend-cinnamon/proxy_scraper/main/proxies.txt";

        conf["threads"] = 4;//std::thread::hardware_concurrency();
        conf["batch_requests"] = 128;

    }


    limits();
    curl_global_init(CURL_GLOBAL_DEFAULT);

    std::string jobsUri = conf["jobs_uri"];

    auto jobs = new Jobs(jobsUri);
    jobs->load();

    auto prxs  = new Proxy();

    if (prxs->load("proxies.txt") == false) {
        prxs->url = conf["proxy_list_uri"];
        prxs->loadUrl();
    }

    //prxs->load("proxies.txt");
    //prxs->load("proxy.txt");


    auto stats = new Stats();
//    auto gun  = new Gun(jobs,prxs,stats);

//    gun->threadsCount = 32;
    //gun->fire();

    auto mgun  = new MultiGun(jobs,prxs,stats);
    mgun->threadsCount = conf["threads"];//std::thread::hardware_concurrency();
    mgun->batch_requests =  conf["batch_requests"];
    mgun->fire();


    auto srv = new httpServer(mgun);
    std::cout << "Starting server on http://127.0.0.1:8088/" << std::endl;

    std::cout << "==============================" << std::endl;
    std::cout << "Open in browser http://127.0.0.1:8088/ " << std::endl;

    srv->start("127.0.0.1",8088);

    return 0;
}
