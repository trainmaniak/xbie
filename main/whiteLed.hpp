#include <ESP8266WebServer.h>

#include "led.hpp"

#pragma once

class WhiteLed : public Led
{
private:
    int pin_;
    int state_ = 255;
    int target_ = 255;

public:
    WhiteLed(ESP8266WebServer &server, int id, int pin)
        : Led(server, id), pin_(pin) {}

    void setTarget(int volume)
    {
        target_ = volume;
    }

    void last()
    {
        on_ = true;
    }

    void full()
    {
        on_ = true;
        setTarget(255);
    }

    void off()
    {
        on_ = false;
    }

    void toggle()
    {
        if (on_)
            off();
        else
            last();
    }

    void sendStatus() {
        server_.sendHeader("Access-Control-Allow-Origin", "*");
        String result;
        serialize(result);
        server_.send(200, "text/json", result);
    }

    void update() override
    {
        bool updateRequired = false;
        int change = 0;

        if (!on_ && state_ != 0) {
            change = 0 - state_;
            updateRequired = true;
        }
        else if (on_ && target_ != state_) {
            change = target_ - state_;
            updateRequired = true;
        }

        if (updateRequired) {
            state_ += DIMMINGSPEED * ((change > 0) ? 1 : -1);

            if (state_ * PWMMULTIPLIER == PWMRANGE)
            {
                digitalWrite(pin_, HIGH);
                return;
            }
            if (state_ == 0)
            {
                digitalWrite(pin_, LOW);
                return;
            }

            analogWrite(pin_, state_ * PWMMULTIPLIER);
        }
    }

    void serialize(String &result) override {
        result = "{\n"
        "  \"id\": \"" + String(id_) + "\",\n"
        "  \"lightType\": \"w\",\n"
        "  \"pwm\": " + (pwm_ ? "true" : "false") + ",\n"
        "  \"on\": " + (on_ ? "true" : "false") + ", \n"
        "  \"target\": " + String(target_) + "\n"
        "}";
    }

    void registerEndpoints() override
    {
        server_.on("/light/" + String(id_) + "/last", [this]() {
            last();
            this->sendStatus();
        });

        server_.on("/light/" + String(id_) + "/full", [this]() {
            full();
            this->sendStatus();
        });

        server_.on("/light/" + String(id_) + "/off", [this]() {
            off();
            this->sendStatus();
        });

        server_.on("/light/" + String(id_) + "/toggle", [this]() {
            toggle();
            this->sendStatus();
        });

        server_.on("/light/" + String(id_) + "/set/w", [this]() {
            if (not server_.hasArg("vol"))
            {
                server_.send(400, "text/plain", "missing argument");
                return;
            }

            int volume = Tools::stringToInt(server_.arg("vol"));
            if (volume > 255)
            {
                server_.send(400, "text/plain", "invalid request");
                return;
            };

            setTarget(volume);
            this->sendStatus();
        });

        server_.on("/light/" + String(id_) + "/status", [this]() {
            this->sendStatus();
        });
    };
};
