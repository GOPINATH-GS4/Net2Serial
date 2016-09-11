//
// Created by GOPI on 8/23/16.
//

#ifndef OPENWRT_MESSAGE_H
#define OPENWRT_MESSAGE_H

#define MESSAGE_SIZE 512
#define MESSAGE_COUNT 512
#define NEW_LINE '\n'
#include <string.h>
#include <stdio.h>
#include <cstdlib>
class Messages {
private:
    char buffer[MESSAGE_COUNT][MESSAGE_SIZE+1];
    int  messageIndex = 0;
public:
    void ParseMessage(char *messages);
    int  size();
    char *getMessage(int index);
    void addMessage(char *message);
};


#endif //OPENWRT_MESSAGE_H
