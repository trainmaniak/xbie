#include <ESP8266WebServer.h>
#include <memory>
#include <string>
#include <vector>

#include "whiteLed.hpp"
#include "rgbLed.hpp"
#include "rgbStrip.hpp"
#include "definitions.hpp"

class XBie
{
private:
    ESP8266WebServer &server;
    std::vector<std::unique_ptr<Light>> lightList;

public:
    XBie(ESP8266WebServer &server)
        : server(server)
    {
        
    }

    void addWLed(const int pin) {
        lightList.emplace_back(new WhiteLed(server, lightList.size(), pin));
    }

    void addRGBLed(const int *pinList) {
        lightList.emplace_back(new RGBLed(server, lightList.size(), pinList));
    }

    void addRGBStrip(const int pin, const int pixelCount) {
        lightList.emplace_back(new RGBStrip(server, lightList.size(), pin, pixelCount));
    }

    void setEndpoints() {
        this->registerEndpoints();

        for (const std::unique_ptr<Light> &led : lightList) {
            led->registerEndpoints();
        }
    }

    void update() {
        for (const std::unique_ptr<Light> &led : lightList) {
            led->update();
        }
    }

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
};
