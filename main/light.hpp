#include <ESP8266WebServer.h>

#pragma once

class Light {
    protected:
    ESP8266WebServer &server;
    int id;
    bool on = true;

    public:
    Light(ESP8266WebServer &server, int id)
        : server(server), id(id), on(false) {}

    virtual void update() = 0;
    virtual void serialize(String &result) = 0;
    virtual void registerEndpoints() = 0;
};
