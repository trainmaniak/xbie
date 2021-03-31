#include <ESP8266WebServer.h>

#include "light.hpp"
#include "tools.hpp"
#include "definitions.hpp"

#pragma once

class Button
{
private:
    int pin_;
    int lightId_;

    bool active_ = false;

public:
    Button(int pin, int lightId)
        : pin_(pin), lightId_(lightId) {}

    const int getLightId() {
        return lightId_;
    }

    int check() {
        int result = -1;

        if (digitalRead(pin_) > 0) {
            if (!active_) {
                result = 1;
            }

            active_ = true;
        } else {
            active_ = false;
        }

        return result;
    }
};
