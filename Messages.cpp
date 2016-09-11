//
// Created by GOPI on 8/23/16.
//

#include "Messages.h"



int Messages::size() {
    return this->messageIndex;
}

char* Messages::getMessage(int index) {
    if (index < 0 || index >= this->messageIndex) return nullptr;
    else return this->buffer[index];
}

void Messages::addMessage(char *message) {
    if (this->messageIndex > MESSAGE_COUNT) return;
    memcpy(this->buffer[this->messageIndex], message, strlen(message));
    printf("Message %s is added to index %d\n", message, this->messageIndex);
    this->messageIndex++;
}
void Messages::ParseMessage(char *messages) {

    int charCount = 0, index = 0;
    char buf[MESSAGE_SIZE + 1];

    while (messages != nullptr && index < strlen(messages) && *(messages + index) != 0x00) {

        if (*(messages + index) != NEW_LINE) {
            if (charCount > MESSAGE_SIZE ) charCount = MESSAGE_SIZE;
            buf[charCount] = *(messages + index);
            charCount++;
        } else {
            buf[charCount] = 0x00;
            charCount = 0;
            this->addMessage(buf);
        }
        index++;
    }
}