#include <ESP8266WebServer.h>

#pragma once

class SubDevice {
protected:
    ESP8266WebServer &server_;
    int id_;

public:
    SubDevice(ESP8266WebServer &server, int id)
        : server_(server), id_(id) {}

    virtual void update() = 0;
    virtual void serialize(String &result) = 0;
    virtual void registerEndpoints() = 0;
}