//
// Created by GOPI on 9/16/16.
//

#include "Net2Serial.h"

void eventhandler(char *msg) {

    printf("Value added : %s\n", msg);
}

int main() {

    Net2Serial *net = new Net2Serial(12345, 10, "/dev/ttyUSB0" , B115200);
    net->Run(eventhandler, false);
    return 0;
}