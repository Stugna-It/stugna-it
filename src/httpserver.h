#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "crow_all.h"

#include "json.hpp"
#include "multigun.h"

struct threadState;

class httpServer
{
public:
    crow::SimpleApp app;
    httpServer(MultiGun * gun);
    MultiGun * gun;

    void start(std::string addr,int port);
    void addRoutes();


    crow::response jRes(string json);
    void ping();
    void stat();
    void threads();

    void index();



};

#endif // HTTPSERVER_H
