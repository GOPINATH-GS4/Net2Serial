#include "Net2Serial.h"

int main(int argc, char *argv[]) {

    for (int i = 0; i < 10; i ++) {
        float x = -1.0 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0- -1.0)));
        float y = -1.0 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0- -1.0)));
        char message[1024];
        sprintf(message, "%2.2f|%2.2f\n", x, y);
        printf(message);
        Net2Serial::sendMessage("localhost", 12345, message);
    }

    return 0;
}
