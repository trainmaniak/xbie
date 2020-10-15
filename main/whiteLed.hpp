#include <ESP8266WebServer.h>

#include "led.hpp"

#pragma once

class WhiteLed : public Led
{
private:
    int pin;
    int state = 255;
    int target = 255;

public:
    WhiteLed(ESP8266WebServer &server, int id, int pin)
        : Led(server, id), pin(pin) {}

    void setTarget(int volume)
    {
        this->target = volume;
    }

    void last()
    {
        on = true;
    }

    void full()
    {
        on = true;
        setTarget(255);
    }

    void off()
    {
        on = false;
    }

    void toggle()
    {
        if (on)
            off();
        else
            last();
    }

    void update() override
    {
        if (!on)
        {
            int change = 0 - this->state;
            this->state += DIMMINGSPEED * ((change > 0) ? 1 : -1);

            if (this->state * PWMMULTIPLIER == PWMRANGE)
            {
                digitalWrite(this->pin, HIGH);
                return;
            }
            if (this->state == 0)
            {
                digitalWrite(this->pin, LOW);
                return;
            }

            analogWrite(this->pin, this->state * PWMMULTIPLIER);
        }
        else if (on && this->target != this->state)
        {
            int change = this->target - this->state;
            this->state += DIMMINGSPEED * ((change > 0) ? 1 : -1);

            if (this->state * PWMMULTIPLIER == PWMRANGE)
            {
                digitalWrite(this->pin, HIGH);
                return;
            }
            if (this->state == 0)
            {
                digitalWrite(this->pin, LOW);
                return;
            }

            analogWrite(this->pin, this->state * PWMMULTIPLIER);
        }
    }

    void serialize(String &result) override {
        result = "{\n"
        "  \"id\": \"" + String(this->id) + "\",\n"
        "  \"lightType\": \"w\",\n"
        "  \"pwm\": " + (this->pwm ? "true" : "false") + ",\n"
        "  \"on\": " + (on ? "true" : "false") + ", \n"
        "  \"target\": " + String(this->target) + "\n"
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

        server.on("/light/" + String(this->id) + "/set/w", [this]() {
            if (not server.hasArg("vol"))
            {
                server.send(400, "text/plain", "missing argument");
                return;
            }

            int volume = Tools::stringToInt(server.arg("vol"));
            if (volume > 255)
            {
                server.send(400, "text/plain", "invalid request");
                return;
            };

            this->setTarget(volume);
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
