#include <ESP8266WebServer.h>
#include <memory>
#include <string>
#include <vector>

#include "whiteLed.hpp"
#include "rgbLed.hpp"
#include "rgbStrip.hpp"
#include "button.hpp"

#include "definitions.hpp"

#include "ir.hpp"

class XBie
{
private:
    ESP8266WebServer &server;
    std::vector<std::unique_ptr<Light>> lightList;
    std::vector<std::unique_ptr<Button>> buttonList;
    std::unique_ptr<Ir> ir_;

    void registerEndpoints()
    {
        this->server.on("/status", [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");

            String result = "{\n"
            "  \"uptime\": " + String(millis() / 1000) + ",\n"
            "  \"lights\": [";
            for (int i = 0; i < lightList.size(); ++i) {
                String ledSer;
                lightList[i]->serialize(ledSer);                
                result += "  " + (i + 1 == lightList.size() ? ledSer + "\n" : ledSer + ",\n");
            }
            result += "  ]\n}";

            server.send(200, "text/json", result);
        });
    }

public:
    XBie(ESP8266WebServer &server)
        : server(server) {}

    void addWLed(const int pin) {
        lightList.emplace_back(new WhiteLed(server, lightList.size(), pin));
    }

    void addRGBLed(const int *pinList) {
        lightList.emplace_back(new RGBLed(server, lightList.size(), pinList));
    }

    void addRGBStrip(const int pin, const int pixelCount) {
        lightList.emplace_back(new RGBStrip(server, lightList.size(), pin, pixelCount));
    }

    void addIR(const int pinSend, const int pinRecv) {
        std::unique_ptr<Ir> tmpIr(new Ir(server, pinSend, pinRecv));
        ir_ = std::move(tmpIr);
    }

    void addButton(const int pin, const int lightId) {
        buttonList.emplace_back(new Button(pin, lightId));
    }


    void setAllEndpoints() {
        this->registerEndpoints();

        for (const std::unique_ptr<Light> &led : lightList) {
            led->registerEndpoints();
        }

        ir_->registerEndpoints();
    }

    void updateLights() {
        for (const std::unique_ptr<Light> &led : lightList) {
            led->update();
        }

        ir_->update();
    }

    void updateButtons() {
        for (const std::unique_ptr<Button> &btn : buttonList) {
            int result = btn->check();

            if (result == 1) {
                Light *light = &*lightList[btn->getLightId()];

                // TODO IF is it LED and cycleable
                if (light->isLed()) {
                    Led *led = static_cast<Led *>(light);
                    led->cycle();
                }
            }
        }
    }
};
