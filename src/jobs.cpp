#include "jobs.h"

Jobs::Jobs(string uri)
{
    this->jobsUri = uri;
}


Job * Jobs::getRand() {
    if (jobs.size() == 0) {
        //std::cout << utils::log_time() << "No jobs in list " << std::endl;
        return nullptr;
    }
    auto random = rand() % jobs.size();
    return jobs[random];

}

void Jobs::load() {
    auto jobs = utils::curl_get(this->jobsUri,30);

    std::vector<Job*> tmpJobs;

    try {
        auto j = crow::json::load(jobs);
        if (!j.has("jobs")) {
            std::cout << utils::log_time() << "Jobs file not have 'jobs' section " << std::endl;
            return;
        }
        for (auto job: j["jobs"]) {
            try {
                std::string type = job["type"].s();
                if (type != "packetgen") { // skip @TODO
                    continue;
                }
                auto args = job["args"];
                std::string method = args["packet"]["payload"]["data"]["method"].s();
                if (method != "GET") { // skip @TODO
                    continue;
                }
                //std::cout << type << " " << method << " " << path << std::endl;
                tmpJobs.push_back(new Job(job));

            } catch (...) {
                std::cout << utils::log_time() << "Skip parse job " << std::endl;  // @TODO show error
            }
        }

        if (tmpJobs.size() > 0) {
            this->jobs.clear();
            std::copy(tmpJobs.begin(), tmpJobs.end(),
                          std::back_inserter(this->jobs));
        }
        std::cout << utils::log_time() << "Jobs loaded: " << this->jobs.size() << " from "<< this->jobsUri << std::endl;

    } catch (...) {
        std::cout << utils::log_time() << "Jobs load failed " << std::endl;  // @TODO show error
    }

}
