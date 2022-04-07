#ifndef JOB_H
#define JOB_H

#include "crow_all.h"
#include "utils.h"
class Job
{
public:
    Job(crow::json::rvalue j);

    crow::json::rvalue j;

    std::string type;
    std::string method;
    std::string path;

    std::string staticHost;


    std::string getHost();
    std::string getTarget();

};

#endif // JOB_H
