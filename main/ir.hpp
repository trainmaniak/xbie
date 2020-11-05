#include <string>
#include <sstream>

#include <ESP8266WebServer.h>

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <IRsend.h>

#include "tools.hpp"
#include "definitions.hpp"

#pragma once

class Ir
{
private:
    ESP8266WebServer &server_;

    IRsend irsend_;
    IRrecv irrecv_;

    decode_results recvData_;
    int waitingCountdown = 0;

    void send(const std::string &code) {
        Serial.println(code.c_str());
        uint32_t codeRAW = strtoul(code.c_str(), NULL, 10);
        irsend_.sendNEC(codeRAW, 32);
        Serial.println("sended");
    }

    void recv() {
        waitingCountdown = IR_WAITING_TIMEOUT;
    }

    void dump(std::string &type, std::string &code) {
        // Dumps out the decode_results structure.
        // Call this after IRrecv::decode()
        uint16_t count = recvData_.rawlen;

        switch (recvData_.decode_type) {
            case UNKNOWN:
            type = "UNKNOWN"; break;
            case NEC:
            type = "NEC"; break;
            case SONY:
            type = "SONY"; break;
            case RC5:
            type = "RC5"; break;
            case RC5X:
            type = "RC5X"; break;
            case RC6:
            type = "RC6"; break;
            case RCMM:
            type = "RCMM"; break;
            case PANASONIC:
            type = "PANASONIC"; break;
            case HEX:
            type = "HEX"; break;
            case LG:
            type = "LG"; break;
            case JVC:
            type = "JVC"; break;
            case AIWA_RC_T501:
            type = "AIWA_RC_T501"; break;
            case WHYNTER:
            type = "WHYNTER"; break;
            default:
            type = "unknown_unknown"; break;
        }

        std::ostringstream oss;
        oss << recvData_.value;
        code = oss.str();
    }
        
public:
    Ir(ESP8266WebServer &server, int pinSend, int pinRecv)
            : server_(server), irsend_(pinSend), irrecv_(pinRecv) {
        irsend_.begin();
        irrecv_.enableIRIn();
    }

    void update() {
        if (waitingCountdown == 0)
            return;

        if (irrecv_.decode(&recvData_)) {
            waitingCountdown = 0;
            
            std::string type;
            std::string code;
            dump(type, code);
            irrecv_.resume();  // reset to receive the next value

            Serial.print("recv ");
            Serial.println(code.c_str());

            std::string result = "{\n"
            "  \"subdevice\": \"ir\",\n"
            "  \"type\": \"" + type + "\",\n"
            "  \"code\": \"" + code + "\"\n"
            "}";
            server_.sendHeader("Access-Control-Allow-Origin", "*");
            server_.send(200, "text/json", result.c_str());

            return;
        }

        waitingCountdown--;
    }

    void registerEndpoints() {
        server_.on("/ir/send", [this]() {
            if (not server_.hasArg("code"))
            {
                server_.send(400, "text/plain", "missing argument");
                return;
            }

            std::string code = server_.arg("code").c_str();
            Serial.print("send ");
            Serial.println(code.c_str());
            send(code);
            server_.sendHeader("Access-Control-Allow-Origin", "*");
            server_.send(200, "text/plain", "ok");
        });

        server_.on("/ir/recv", [this]() {
            recv();
            //server_.sendHeader("Access-Control-Allow-Origin", "*");
            //server_.send(200, "text/plain", "ok");
        });
    }
};
