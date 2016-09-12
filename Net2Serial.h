//
// Created by GOPI on 8/20/16.
//

#ifndef OPENWRT_Net2Serial_H
#define OPENWRT_Net2Serial_H


#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <iostream>
#include <stdio.h>
#include <unordered_map>
#include <array>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include "Messages.h"

class Net2Serial {
private:

    int  sockfd;
    int  port = 5001;
    int  backlog = 10;
    int  serialFd = -1;
    Messages *msgs;
    void ProcessRequest(int);
    bool SetNonBlocking(int);
    bool setBaudRate(int fd, int speed);
    static void setSigChild();
    static  void handle_sigchld(int sig);

public:

    Net2Serial(int port, int backlog, char *serialPort, int baudRate);
    void Run();
    Messages *getMessages();

};


#endif //OPENWRT_Net2Serial_H
