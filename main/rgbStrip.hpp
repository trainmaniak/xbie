#include <memory>
#include <iostream>

#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

#include "light.hpp"
#include "rgbStripAnimationEnum.hpp"

class RGBStrip : Light
{
private:
    Adafruit_NeoPixel *strip;
    int pin;
    int pixelCount = 0;
    int target[3] = {255};

    RGBStripAnimationEnum animation_ = RGBStripAnimationEnum::solid;
    bool solidAnimationUpdated_ = false;

    String getStringEnum() {
        switch (animation_) {
            case RGBStripAnimationEnum::solid:
            return "solid"; break;
            default:
            return "unknown"; break;
        }
    }

public:
    RGBStrip(ESP8266WebServer &server, int id)
        : Light(server, id)
    {
        strip = new Adafruit_NeoPixel(pixelCount, pin, NEO_GRB + NEO_KHZ800);
    }

    void setSolidColor(const int targetList[])
    {
        memcpy(this->target, targetList, sizeof(this->target));
        animation_ = RGBStripAnimationEnum::solid;
        solidAnimationUpdated_ = false;
    }

    void update() override
    {

        if (animation_ == RGBStripAnimationEnum::solid)
        {
            if (solidAnimationUpdated_)
                return;
            else
            {
                for (int i = 0; i < pixelCount; ++i)
                {
                    strip->setPixelColor(pixelCount, target[0], target[1], target[2]);
                }

                solidAnimationUpdated_ = true;
            }
        }
        else
        {
            return;

        }
    }

    void serialize(String &result) override {
        result = "{\n"
        "  \"id\": \"" + String(this->id) + "\",\n"
        "  \"lightType\": \"s\",\n"
        "  \"on\": " + (this->on ? "true" : "false") + ", \n"
        "  \'animation\": " + getStringEnum() + ",\n"
        "  \"target_r\": " + String(this->target[0]) + ",\n"
        "  \"target_g\": " + String(this->target[1]) + ",\n"
        "  \"target_b\": " + String(this->target[2]) + "\n"
        "}";
    }

    void registerEndpoints() override {

    }
};
