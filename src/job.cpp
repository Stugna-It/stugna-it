#include "job.h"

Job::Job(crow::json::rvalue j)
{
    this->j = j; // validate when parse - jobs.load()
//    type    = j["type"].s();
//    method  = j["args"]["request"]["method"].s();
//    path    = j["args"]["request"]["path"].s();

    auto args = j["args"];
    path = args["packet"]["payload"]["data"]["path"].s();

    if (args.has("connection")) {
        if (args["connection"].has("args")) {
            this->staticHost = args["connection"]["args"]["address"].s();
            this->hostResolve = getHostResolve();
        }
    }

}


curl_slist * Job::getHostResolve() {
    struct curl_slist * hostResolve = nullptr;
    if (this->staticHost != "") {
        std::string s;
        std::string ip   = this->staticHost.substr (0,this->staticHost.find(":"));
        std::string port = this->staticHost.substr (this->staticHost.find(":")+1);
        s = this->getHost() + ":" + port + ":" + ip;
        hostResolve = curl_slist_append(NULL,s.c_str());
    }
    return hostResolve;
}

std::string Job::getTarget() {

    return this->path;
}

std::string Job::getHost() {
    return utils::getHost(this->path);

}
