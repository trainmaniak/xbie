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
    std::vector<std::unique_ptr<Led>> ledList;

public:
    XBie(ESP8266WebServer &server)
        : server(server)
    {
        
    }

    void addWLed(int pin) {
        ledList.emplace_back(new WhiteLed(server, ledList.size(), pin));
    }

    void addRGBLed(const int *pinList) {
        ledList.emplace_back(new RGBLed(server, ledList.size(), pinList));
    }

    void setEndpoints() {
        this->registerEndpoints();

        for (const std::unique_ptr<Led> &led : ledList) {
            led->registerEndpoints();
        }
    }

    void update() {
        for (const std::unique_ptr<Led> &led : ledList) {
            led->update();
        }
    }

    void registerEndpoints()
    {
        this->server.on("/status", [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");

            String result = "{\n"
            "  \"uptime\": \"" + String(millis() / 1000) + ",\n"
            "  \"lights\": [";
            for (int i = 0; i < ledList.size(); ++i) {
                String ledSer;
                ledList[i]->serialize(ledSer);                
                result += "  " + (i + 1 == ledList.size() ? ledSer + "\n" : ledSer + ",\n");
            }
            result += "  ]\n}";

            server.send(200, "text/json", result);
        });
    }
};