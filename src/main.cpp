

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
    limit.rlim_cur = limit.rlim_max;

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

int main(int argc, char* argv[])
{
    /* daemonize */
    if (argc>1 and std::string(argv[1])==std::string("-d")) {
        std::cout<<utils::log_time()<<"run in background "<<std::endl;
        auto mypid = fork();
        if (mypid>0)
            exit(0);
        setsid();
        std::freopen("stugna-it.out.log", "aw", stdout);
        std::freopen("stugna-it.err.log", "aw", stderr);
    }

    json conf;
    try {
        std::ifstream ifs("config.json");
        conf = json::parse(ifs);
    } catch (...) {
        std::cout<<utils::log_time() << "Config not found (config.json) or not valid json, using defaults " << std::endl;
        conf["jobs_uri"] = "https://raw.githubusercontent.com/db1000n-coordinators/LoadTestConfig/main/config.v0.7.json";
        conf["proxies"] = {"https://raw.githubusercontent.com/porthole-ascend-cinnamon/proxy_scraper/main/proxies.txt"};

        conf["threads"] = std::thread::hardware_concurrency();
        conf["batch_requests"] = 128;

        conf["listen_on"] = "127.0.0.1";
        conf["listen_port"] = 8088;

    }


    limits();
    curl_global_init(CURL_GLOBAL_DEFAULT);

    std::string jobsUri = conf["jobs_uri"];

    auto jobs = new Jobs(jobsUri);
    jobs->load();

    auto prxs  = new Proxy();

    if (prxs->load("proxies.txt") == false) {
        for (std::string url: conf["proxies"]) {
            prxs->urls.push_back(url);
        }
        prxs->loadUrl();
    }
    auto stats = new Stats();
    auto mgun  = new MultiGun(jobs,prxs,stats);

    mgun->threadsCount = conf["threads"];//std::thread::hardware_concurrency();
    mgun->batch_requests =  conf["batch_requests"];
    mgun->fire();

    auto srv = new httpServer(mgun);

    auto statLink = "http://" + conf["listen_on"].get<std::string>() + ":" +std::to_string(conf["listen_port"].get<int>());
    std::cout<<utils::log_time() << "Starting server on " + statLink << std::endl;
    std::cout<<utils::log_time() << "==============================" << std::endl;
    std::cout<<utils::log_time() << "Open in browser " + statLink << std::endl;


#ifdef __linux
    std::system(("xdg-open  " + statLink).c_str());
#endif
#ifdef _WIN_64
    std::system(("explorer " +  statLink).c_str());
#endif
    if (std::filesystem::exists("/.dockerenv")) {
        conf["listen_on"] = "0.0.0.0";
    }

    srv->start(conf["listen_on"],conf["listen_port"]);

    return 0;
}
