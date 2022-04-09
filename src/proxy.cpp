#include "proxy.h"
#include "utils.h"
// https://raw.githubusercontent.com/porthole-ascend-cinnamon/proxy_scraper/main/proxies.txt
Proxy::Proxy()
{



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
    std::cout << "Proxy clean, deleted " << deleted<< " leave in list " << this->prx.size() << std::endl;
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
    if (this->url == "") {
        return false;
    }
    auto sp = utils::curl_get(this->url,30);
    std::vector<std::string> prxTmp;
    std::istringstream iss(sp);
    for (std::string line; std::getline(iss, line); ) {
        if (line.length() < 8) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        prxTmp.push_back(line);

    }
    if (prxTmp.size() < 8) {
        std::cout << "Proxy loaded from URL failed " << this->url << std::endl;
        return false;
    }

    proxyLock.lock();
    prx.clear();
    std::copy(prxTmp.begin(), prxTmp.end(),
                  std::back_inserter(this->prx));
    proxyLock.unlock();
    std::cout << "Proxy loaded from URL ("<< prx.size()<<") " << this->url << std::endl;
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
        std::cout << utils::log_time() << "warning:: cannot open saved proxy list from: " << std::filesystem::current_path() << fileName << std::endl;
        return false;
    }

}
