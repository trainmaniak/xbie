#include <ESP8266WebServer.h>

#pragma once

class Light {
    protected:
    ESP8266WebServer &server_;
    int id_;
    bool on_ = true;

    public:
    Light(ESP8266WebServer &server, int id)
        : server_(server), id_(id), on_(false) {}

    virtual void update() = 0;
    virtual void serialize(String &result) = 0;
    virtual void registerEndpoints() = 0;
};
