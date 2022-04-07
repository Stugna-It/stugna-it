#ifndef JOBS_H
#define JOBS_H

#include "crow_all.h"
#include "utils.h"
#include "job.h"

class Jobs
{
public:
    Jobs(std::string uri);


    std::string jobsUri;
    void load();



    std::vector<Job*> jobs;
    Job * getRand();

};

#endif // JOBS_H
