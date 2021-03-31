#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <string>
#include <vector>
#include <memory>

// common includes
#include "xbie.hpp"
#include "definitions.hpp"
#include "tools.hpp"

// includes for LED subdevice
#include <Adafruit_NeoPixel.h>

#include "led.hpp"
#include "whiteLed.hpp"
#include "rgbLed.hpp"

// includes for IR subdevice
#include "ir.hpp"

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// WIFI login
const char *ssid = "";
const char *password = "";

MDNSResponder mdns;

// I/O pins
#define BLINK_LED 14 // On board led. Second instaled LED beside LED_BUILTIN.
#define PIN_LED_W D3

// for testing purpose only
#define PIN_LED_RGB_R D8
#define PIN_LED_RGB_G D5
#define PIN_LED_RGB_B D6
#define PIN_LED_S D7

#define PIN_IR_SEND D2
#define PIN_IR_RECV D4

#define PIN_BTN D1

// Web server
ESP8266WebServer server(80);

int lastUpdate = 0;

const int ledCount = 1;

XBie xbie(server);


void setup()
{
    // IO INIT
    pinMode(BLINK_LED, OUTPUT);

    pinMode(PIN_LED_W, OUTPUT);
    digitalWrite(PIN_LED_W, LOW);

    pinMode(PIN_LED_RGB_R, OUTPUT);
    digitalWrite(PIN_LED_RGB_R, LOW);

    pinMode(PIN_LED_RGB_G, OUTPUT);
    digitalWrite(PIN_LED_RGB_G, LOW);

    pinMode(PIN_LED_RGB_B, OUTPUT);
    digitalWrite(PIN_LED_RGB_B, LOW);

    pinMode(PIN_BTN, INPUT);

    

// NEOPIXELS
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif

    // SERIAL
    Serial.begin(115200);
    Serial.println("Booting");

    // WIFI init
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    ArduinoOTA.setHostname("xbie-wemos");

    // No authentication by default
    // ArduinoOTA.setPassword((const char *)"123");

    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        switch (error) {
            case OTA_AUTH_ERROR:
            Serial.println("Auth Failed"); break;
            case OTA_BEGIN_ERROR:
            Serial.println("Begin Failed"); break;
            case OTA_CONNECT_ERROR:
            Serial.println("Connect Failed"); break;
            case OTA_RECEIVE_ERROR:
            Serial.println("Receive Failed"); break;
            case OTA_END_ERROR:
            Serial.println("End Failed"); break;
        }
    });
    ArduinoOTA.begin();

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    xbie.addWLed(PIN_LED_W);
    int rgbPins[3] = {PIN_LED_RGB_R, PIN_LED_RGB_G, PIN_LED_RGB_B};
    xbie.addRGBLed(rgbPins);
    //xbie.addRGBStrip(PIN_LED_S, 150);
    xbie.addIR(PIN_IR_SEND, PIN_IR_RECV);
    xbie.addButton(PIN_BTN, 0);
    
    xbie.setAllEndpoints();

    lastUpdate = millis();

    // START HTTP server
    server.begin();
};

void loop()
{
    ArduinoOTA.handle();
    server.handleClient();

    xbie.updateButtons();

    if (lastUpdate - millis() > 16) {
        xbie.updateLights();
        lastUpdate = millis();
    }
    delay(1);
};
