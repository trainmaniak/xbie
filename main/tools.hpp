#include <string>
#include <math.h>

#pragma once

class Tools {
    public:
        static int stringToInt(String input) {
            int len = input.length();
            int value = 0;
            for (int i = 0; i < len; i++) {
                int pos = (len - 1) - i;
                char chr = input[pos];
                if ((chr >= '0') and (chr <= '9')) {
                    value += pow(10.0, i) * (chr - '0');
                }
            }
            return value;
        }
};
