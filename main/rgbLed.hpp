#include <memory>
#include <string>
#include <ESP8266WebServer.h>

#include "led.hpp"

#pragma once

class RGBLed : public Led
{
private:
    int pinList_[3];
    int state_[3] = {255};
    int target_[3] = {255};

public:
    RGBLed(ESP8266WebServer &server, int id, const int *pinList)
        : Led(server, id)
    {
        memcpy(pinList_, pinList, sizeof(pinList_));
    }

    void setTarget(const int value[])
    {
        memcpy(target_, value, sizeof(target_));
    }

    void last()
    {
        on_ = true;
    }

    void full()
    {
        on_ = true;
        int target_[3] = {255, 255, 255};
        setTarget(target_);
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

    void update() override
    {
        for (int i = 0; i < 3; ++i)
        {
            bool updateRequired = false;
            int change = 0;

            if (!on_ && state_[i] != 0) {
                change = 0 - state_[i];
                updateRequired = true;
            }
            else if (on_ && target_[i] != state_[i]) {
                change = target_[i] - state_[i];
                updateRequired = true;
            }

            if (updateRequired) {
                state_[i] += DIMMINGSPEED * ((change > 0) ? 1 : -1);

                if (state_[i] * PWMMULTIPLIER == PWMRANGE)
                {
                    digitalWrite(pinList_[i], HIGH);
                    return;
                }
                if (state_[i] == 0)
                {
                    digitalWrite(pinList_[i], LOW);
                    return;
                }

                analogWrite(pinList_[i], state_[i] * PWMMULTIPLIER);
            }
        }
    }

    void serialize(String &result) override {
        result = "{\n"
        "  \"id\": \"" + String(id_) + "\",\n"
        "  \"lightType\": \"rgb\",\n"
        "  \"pwm\": " + (pwm_ ? "true" : "false") + ",\n"
        "  \"on\": " + (on_ ? "true" : "false") + ", \n"
        "  \"target_r\": " + String(target_[0]) + ",\n"
        "  \"target_g\": " + String(target_[1]) + ",\n"
        "  \"target_b\": " + String(target_[2]) + "\n"
        "}";
    }

    void registerEndpoints() override
    {
        server_.on("/light/" + String(id_) + "/last", [this]() {
            last();
            server_.sendHeader("Access-Control-Allow-Origin", "*");
            server_.send(200, "text/plain", "ok");
        });

        server_.on("/light/" + String(id_) + "/full", [this]() {
            full();
            server_.sendHeader("Access-Control-Allow-Origin", "*");
            server_.send(200, "text/plain", "ok");
        });

        server_.on("/light/" + String(id_) + "/off", [this]() {
            off();
            server_.sendHeader("Access-Control-Allow-Origin", "*");
            server_.send(201, "text/plain", "ok");
        });

        server_.on("/light/" + String(id_) + "/toggle", [this]() {
            toggle();
            server_.sendHeader("Access-Control-Allow-Origin", "*");
            server_.send(200, "text/plain", "ok");
        });

        server_.on("/light/" + String(id_) + "/set/rgb", [this]() {
            if (not server_.hasArg("r") || not server_.hasArg("g") || not server_.hasArg("b"))
            {
                server_.send(400, "text/plain", "missing argument");
                return;
            }

            int ch[3];
            ch[0] = Tools::stringToInt(server_.arg("r"));
            ch[1] = Tools::stringToInt(server_.arg("g"));
            ch[2] = Tools::stringToInt(server_.arg("b"));

            for (int i = 0; i < 0; ++i)
            {
                if (ch[i] > 1023)
                {
                    server_.send(400, "text/plain", "invalid request");
                    return;
                }
            };
            setTarget(ch);
            server_.sendHeader("Access-Control-Allow-Origin", "*");
            server_.send(200, "text/plain", "ok");
        });

        server_.on("/light/" + String(id_) + "/status", [this]() {
            server_.sendHeader("Access-Control-Allow-Origin", "*");
            String result;
            serialize(result);
            server_.send(200, "text/json", result);
        });
    };
};
