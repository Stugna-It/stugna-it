#include "httpserver.h"

using namespace nlohmann;


httpServer::httpServer(MultiGun *gun) : gun(gun)
{
    this->gun = gun;
    app.loglevel(crow::LogLevel::Critical);
    //app.loglevel(crow::LogLevel::Info);
    //app.loglevel(crow::LogLevel::Debug);

    addRoutes();

}

void httpServer::start(std::string addr,int port) {
    app.bindaddr(addr);
    app.port(port).multithreaded().run();


}

void httpServer::addRoutes() {
    ping();
    threads(); ///status
    stat();
    index();

}

void httpServer::index() {

    CROW_ROUTE(app, "/").methods("GET"_method)
    ([&](const crow::request& req) {
        auto res = crow::response();

        res.set_static_file_info("index.html");

        return res;
    });
}



crow::response httpServer::jRes(std::string json) {
    auto res = crow::response();
    res.set_header("Content-Type", "application/json");
    res.set_header("Access-Control-Allow-Origin", "*");
    res.body = json;
    return res;
}


void httpServer::ping() {
    auto gun = this->gun;
    CROW_ROUTE(app, "/ping").methods("GET"_method)
    ([&](const crow::request& req) {
        crow::json::wvalue j;
        j["response"] = "pong";
        return jRes(j.dump());
    });
}


void httpServer::stat() {
    auto &stat = this->gun->stat;
    CROW_ROUTE(app, "/stat").methods("GET"_method)
    ([&](const crow::request& req) {

        return jRes(stat->toJson());
    });
}

void httpServer::threads() {
    auto &thState = this->gun->thState;
    auto &gun     = this->gun;
    CROW_ROUTE(app, "/status").methods("GET"_method)
    ([&](const crow::request& req) {
        json j;
        json jr;
        size_t requests = 0;
        for (auto &th: thState ) {
            jr[th.first] = th.second.toJson();
            requests += th.second.count;
        }
        auto elapsed = time(nullptr) - gun->startTime;
        j["status"] = jr;
        j["summ"]["elapsed"] = elapsed;
        j["summ"]["requests"] = requests;
        j["summ"]["proxylistsize"] = gun->prx->count();

        return jRes(j.dump());
    });


}



