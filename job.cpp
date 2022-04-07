#include "job.h"

Job::Job(crow::json::rvalue j)
{
    this->j = j; // validate when parse - jobs.load()
    type    = j["type"].s();
    method  = j["args"]["request"]["method"].s();
    path    = j["args"]["request"]["path"].s();

    if (j["args"].has("client")) {
        if (j["args"]["client"].has("static_host")) {
            staticHost = j["args"]["client"]["static_host"]["addr"].s();
            //std::cout << "static host  " << staticHost << std::endl;
        }
    }

}


std::string Job::getTarget() {

    return this->path;
}

std::string Job::getHost() {
    return utils::getHost(this->path);

}
