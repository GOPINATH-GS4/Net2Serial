#include "Net2Serial.h"

int main(int argc, char *argv[]) {

    Net2Serial::sendMessage("localhost", 12345, "F");
    sleep(10);
    Net2Serial::sendMessage("localhost", 12345, "S");

    return 0;
}
