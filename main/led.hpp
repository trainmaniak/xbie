#include <ESP8266WebServer.h>

#include "light.hpp"
#include "tools.hpp"
#include "definitions.hpp"

#pragma once

class Led : public Light
{
protected:
    bool pwm = true;

public:
    Led(ESP8266WebServer &server, int id)
        : Light(server, id) {}

    virtual void update() = 0;
    virtual void serialize(String &result) = 0;
    virtual void registerEndpoints() = 0;
    //virtual ~Led() = 0;
};
