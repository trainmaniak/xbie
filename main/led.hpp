#include <ESP8266WebServer.h>

#include "light.hpp"
#include "tools.hpp"
#include "definitions.hpp"

#pragma once

class Led : public Light
{
protected:
    bool pwm_ = true;

public:
    Led(ESP8266WebServer &server, int id)
        : Light(server, id) {}

    bool isLed() override {
        return true;
    }

    virtual int getBrightness() = 0;

    bool isCycleable() { // TODO override
        return pwm_;
    }

    virtual void cycle() = 0;

    void last()
    {
        on_ = true;
    }

    void off()
    {
        on_ = false;
    }

    virtual void full() = 0;

    void toggle()
    {
        if (on_)
            off();
        else
            full();
    }

    virtual void update() = 0;
    virtual void serialize(String &result) = 0;
    virtual void registerEndpoints() = 0;
    //virtual ~Led() = 0;
};
