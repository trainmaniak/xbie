#include <ESP8266WebServer.h>

#include "tools.hpp"
#include "definitions.hpp"

#pragma once

class Led
{
protected:
    int id;
    bool pwm = true;
    ESP8266WebServer &server;

public:
    Led(ESP8266WebServer &server, int id)
        : server(server), id(id) {}

    virtual void update() = 0;
    virtual void serialize(String &result) = 0;
    virtual void registerEndpoints() = 0;
    //virtual ~Led() = 0;
};
