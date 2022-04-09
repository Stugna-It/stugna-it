#include "utils.h"


namespace utils {

    using namespace std;



    std::string log_time() {
        time_t ct = time(nullptr);
        std::string lt = ctime(&ct);
        if (!lt.empty()) {
            lt.pop_back(); //remove last char
        }
        return "["+lt+"] ";
    }

    std::string getHost(std::string uri) {
        CURLU *h;
        CURLUcode uc;
        char *host;
        h = curl_url(); /* get a handle to work with */
        if(!h) {
            return "";
        }
        /* parse a full URL */
        uc = curl_url_set(h, CURLUPART_URL, uri.c_str(), 0);
        /* extract host name from the parsed URL */
        uc = curl_url_get(h, CURLUPART_HOST, &host, 0);
        std::string sh = "";
        if(!uc) {
            sh = host;
        }
        curl_free(host);
        curl_url_cleanup(h);
        return sh;

    }


    static size_t WriteCallback(char *data, size_t size, size_t nmemb, std::string *buffer)
    {
        buffer->append(data,size*nmemb);
        return size*nmemb;
    }

    std::string curl_get(const std::string &url,uint32_t timeout)
    {
        CURL *curl;
        std::string readBuffer;
        curl = curl_easy_init();
        CURLcode res;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL,url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING,"gzip");
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,1);

            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT,timeout);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                 cout << log_time() << "curl failed: " <<  curl_easy_strerror(res) << endl;
            }
            //std::cout << readBuffer << std::endl;
        } else {
            cout << log_time() << "curl failed init" << endl;
        }
        curl_easy_cleanup(curl);
        return readBuffer;

    }


    int random_int(int min, int max)
    {

       return min + rand() % (max+1 - min);
    }


}
