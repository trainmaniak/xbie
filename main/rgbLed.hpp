#include <memory>
#include <string>
#include <ESP8266WebServer.h>

#include "led.hpp"

#pragma once

class RGBLed : public Led
{
private:
    int pinList[3];
    bool on = false;
    int state[3] = {255};
    int target[3] = {255};

public:
    RGBLed(ESP8266WebServer &server, int id, const int *pinList)
        : Led(server, id)
    {
        memcpy(this->pinList, pinList, sizeof(this->pinList));
    }

    void setTarget(const int value[])
    {
        memcpy(this->target, value, sizeof(this->target));
    }

    void last()
    {
        this->on = true;
    }

    void full()
    {
        this->on = true;
        int target[3] = {255, 255, 255};
        setTarget(target);
    }

    void off()
    {
        this->on = false;
    }

    void toggle()
    {
        if (this->on)
            off();
        else
            last();
    }

    void update() override
    {
        for (int i = 0; i < 3; ++i)
        {
            if (this->target[i] != this->state[i])
            {
                int change = this->target[i] - this->state[i];
                this->state[i] += DIMMINGSPEED * ((change > 0) ? 1 : -1);

                if (this->state[i] * PWMMULTIPLIER == PWMRANGE)
                {
                    digitalWrite(this->pinList[i], HIGH);
                    return;
                }
                if (this->state == 0)
                {
                    digitalWrite(this->pinList[i], LOW);
                    return;
                }

                analogWrite(this->pinList[i], this->state[i] * PWMMULTIPLIER);
            }
        }
    }

    void serialize(String &result) override {
        result = "{\n"
        "  \"id\": \"" + String(this->id) + "\",\n"
        "  \"ledType\": \"w\",\n"
        "  \"pwm\": " + (this->pwm ? "true" : "false") + ",\n"
        "  \"on\": " + (this->on ? "true" : "false") + ", \n"
        "  \"state_r\": " + String(this->state[0]) + ",\n"
        "  \"state_g\": " + String(this->state[1]) + ",\n"
        "  \"state_b\": " + String(this->state[2]) + ",\n"
        "  \"target_r\": " + String(this->target[0]) + ",\n"
        "  \"target_g\": " + String(this->target[1]) + ",\n"
        "  \"target_b\": " + String(this->target[2]) + "\n"
        "}";
    }

    void registerEndpoints() override
    {
        server.on("/light/" + String(this->id) + "/last", [this]() {
            this->last();
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "text/plain", "ok");
        });

        server.on("/light/" + String(this->id) + "/full", [this]() {
            this->full();
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "text/plain", "ok");
        });

        server.on("/light/" + String(this->id) + "/off", [this]() {
            this->off();
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(201, "text/plain", "ok");
        });

        server.on("/light/" + String(this->id) + "/toggle", [this]() {
            this->toggle();
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "text/plain", "ok");
        });

        server.on("/light/" + String(this->id) + "/set/rgb", [this]() {
            if (not server.hasArg("r") || not server.hasArg("g") || not server.hasArg("b"))
            {
                server.send(400, "text/plain", "missing argument");
                return;
            }

            int ch[3];
            ch[0] = Tools::stringToInt(server.arg("r"));
            ch[1] = Tools::stringToInt(server.arg("g"));
            ch[2] = Tools::stringToInt(server.arg("b"));

            for (int i = 0; i < 0; ++i)
            {
                if (ch[i] > 1023)
                {
                    server.send(400, "text/plain", "invalid request");
                    return;
                }
            };
            this->setTarget(ch);
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "text/plain", "ok");
        });

        server.on("/light/" + String(this->id) + "/status", [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            String result;
            serialize(result);
            server.send(200, "text/json", result);
        });
    };
};
