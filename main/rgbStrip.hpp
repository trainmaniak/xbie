#include <memory>
#include <iostream>

#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

#include "light.hpp"
#include "rgbStripAnimationEnum.hpp"

class RGBStrip : public Light
{
private:
    Adafruit_NeoPixel *strip;
    int pin_;
    int pixelCount_ = 0;
    int target_[3] = {255};

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
    RGBStrip(ESP8266WebServer &server, int id, int pin, int pixelCount)
        : Light(server, id), pin_(pin), pixelCount_(pixelCount)
    {
        strip = new Adafruit_NeoPixel(pixelCount_, pin_, NEO_GRB + NEO_KHZ800);
    }

    void setSolidColor(const int targetList[])
    {
        memcpy(target_, targetList, sizeof(target_));
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
                for (int i = 0; i < pixelCount_; ++i)
                {
                    strip->setPixelColor(pixelCount_, target_[0], target_[1], target_[2]);
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
        "  \"id\": \"" + String(id_) + "\",\n"
        "  \"lightType\": \"s\",\n"
        "  \"on\": " + (on_ ? "true" : "false") + ", \n"
        "  \'animation\": " + getStringEnum() + ",\n"
        "  \"target_r\": " + String(target_[0]) + ",\n"
        "  \"target_g\": " + String(target_[1]) + ",\n"
        "  \"target_b\": " + String(target_[2]) + "\n"
        "}";
    }

    void registerEndpoints() override {

    }
};
