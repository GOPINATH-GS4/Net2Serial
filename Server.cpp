#include "Net2Serial.h"

void eventhandler(char *msg) {

    printf("Value added : %s\n", msg);
}

int main() {

    Net2Serial *net = new Net2Serial(12345, 10, "/dev/ttyUSB0" , 100);
    net->Run(eventhandler);
    return 0;
}