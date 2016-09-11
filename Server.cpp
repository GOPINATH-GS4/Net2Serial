#include "Net2Serial.h"

int main() {

    Net2Serial *net = new Net2Serial(12345, 10, "/dev/tty" , 100);
    net->Run();
    return 0;
}