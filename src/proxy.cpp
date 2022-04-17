#include "proxy.h"
#include "utils.h"
// https://raw.githubusercontent.com/porthole-ascend-cinnamon/proxy_scraper/main/proxies.txt
Proxy::Proxy()
{



}

size_t Proxy::count() {
    return this->prx.size();
}

uint Proxy::cleanBad(Stats * stat) {
    uint deleted = 0;
    for (auto prxStat: stat->proxies) {
        auto prxS = prxStat.first;
        auto http = prxStat.second.count_http;
        auto curl = prxStat.second.count_curl;
        //std::cout << curl << " " << http << "   " <<prxS << std::endl;
        if (http == 0 and curl > 10) { // no http response only curl errors proxy not work or blacklisted
            auto prxI = std::find(this->prx.begin(), this->prx.end(), prxS);
            if (prxI != this->prx.end()) {
                proxyLock.lock();
                this->prx.erase(prxI);
                proxyLock.unlock();
                deleted++;
            }
        }
    }
    std::cout<<utils::log_time() << "Proxy clean, deleted: " << deleted<< " leave in list: " << this->count() << std::endl;
    return deleted;
}


std::string Proxy::getRand() {
    proxyLock.lock();
    if (prx.size() == 0) {
        //std::cout << utils::log_time() << "No proxies in list " << std::endl;
        proxyLock.unlock();
        return "";
    }
    auto random = rand() % prx.size();
    auto rprx = prx[random];
    proxyLock.unlock();
    return rprx;

}

bool Proxy::loadUrl() {
    std::vector<std::string> prxTmp;
    uint i = 0;
    for (auto url: this->urls) {
        auto sp = utils::curl_get(url,30);
        std::istringstream iss(sp);
        for (std::string line; std::getline(iss, line); ) {
            if (line.length() < 8) {
                continue;
            }
            if (line.at(0) == '#') {
                continue;
            }
            prxTmp.push_back(line);
            i++;
        }
        std::cout<<utils::log_time() << "Proxy loaded ("<< i <<") from "<< url << std::endl;
        i = 0;
    }

    proxyLock.lock();
    prx.clear();
    std::copy(prxTmp.begin(), prxTmp.end(),
                  std::back_inserter(this->prx));
    proxyLock.unlock();

    return true;

}

bool Proxy::load(std::string fileName) {
    if (fileName == "") {
        fileName = this->fileName;
    }
    std::vector<std::string> prxTmp;
    std::ifstream file(fileName);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.length() < 8) {
                continue;
            }
            if (line.at(0) == '#') {
                continue;
            }
            prxTmp.push_back(line);

        }
        file.close();
        proxyLock.lock();
        prx.clear();
        std::copy(prxTmp.begin(), prxTmp.end(),
                      std::back_inserter(this->prx));

        proxyLock.unlock();
        this->fileName = fileName;
        std::cout << utils::log_time() <<"loaded saved proxies (" << prx.size() << ") from" << std::filesystem::current_path() << fileName << std::endl;
        return true;
    } else {
        std::cout << utils::log_time() << "no proxies in local file: " << fileName << std::endl;
        return false;
    }

}
