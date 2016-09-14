//
// Created by GOPI on 8/20/16.
//

#include "Net2Serial.h"

bool Net2Serial::setBaudRate(int fd, int speed) {
    struct termios tty;

    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        printf("tcgetattr %s\n", sys_errlist[errno]);
        return false;
    }


    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    /* Needed for ROOMBA Serial communication from create open interface documentation
     Data bits: 8
     Parity: None
     Stop bits: 1
     Flow control: None
     */


    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN);
    tty.c_oflag &= ~(OPOST);
    tty.c_iflag &= ~(INLCR | IGNCR | ICRNL | IGNBRK);

    tty.c_cflag |= (CLOCAL | CREAD | CS8);
    tty.c_cflag &= ~(PARENB | CSTOPB);
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);           // disable XON XOFF (for transmit and receive)
    tty.c_cflag &= ~CSIZE;
    tty.c_cc[VMIN] = 1;     //min carachters to be read
    tty.c_cc[VTIME] = 0;    //Time to wait for data (tenths of seconds)

    //Set the new options for the port...

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("tcgetattr %s\n", sys_errlist[errno]);
        return false;

    }
    return true;
}

void Net2Serial::sendMessage(char *host, int port, char *message) {

    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(host);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    int flags = fcntl(sockfd, F_GETFL, 0);

    flags = flags | O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);

    if (write(sockfd, message, strlen(message)) < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
    close(sockfd);
    return;
}
Net2Serial::Net2Serial(int port, int backlog, char *serialPort, int baudRate) {
    this->port = port;
    this->backlog = backlog;

    this->serialFd = open(serialPort, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (this->serialFd < 0) {
        printf("Error opening file ... %s\n", sys_errlist[errno]);
        exit(1);
    }
    if (!this->setBaudRate(this->serialFd, baudRate)) {
        printf("Unable to set baudrate to %d\n", baudRate);
        exit(1);
    }

    setSigChild();
}

void Net2Serial::setSigChild() {
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror(0);
        exit(1);
    }
}


void Net2Serial::Run(void (*f)(char *)) {

    int client_fd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    /* First call to socket() function */
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(this->port);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        client_fd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (client_fd < 0) {
            perror("ERROR on accept");
            exit(1);
        }
        int pid = fork();

        if (pid < 0) {
            perror("ERROR on fork");
            exit(1);
        }

        if (pid == 0) {
            close(sockfd);
            SetNonBlocking(client_fd);
            char *buffer = ProcessRequest(client_fd);
            if(write(this->serialFd, buffer, strlen(buffer)) < strlen(buffer)) {
                perror("write");
                f((char *) sys_errlist[errno]);
                exit(1);
            }
            printf("Buffer = %s\n", buffer);
            close(client_fd);
            f(buffer);
            exit(0);
        }

    }
}

bool Net2Serial::SetNonBlocking(int fd) {
    if (fd < 0) return false;

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return false;
    flags = flags | O_NONBLOCK;
    return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;

}

void Net2Serial::handle_sigchld(int sig) {
    printf("SigChild ....\n");
    int saved_errno = errno;
    setSigChild();
    while (waitpid((pid_t) (-1), 0, WNOHANG) > 0) {}
    errno = saved_errno;
}

char *Net2Serial::ProcessRequest(int sockfd) {

    fd_set readfds;
    struct timeval tv;
    char buffer[2048];
    int index, ret;
    char c;

    FD_ZERO(&readfds);

    FD_SET(sockfd, &readfds);


    int n = sockfd + 1;

    tv.tv_sec = 5;
    tv.tv_usec = .5;

    index = 0;
    ret = select(n, &readfds, NULL, NULL, &tv);

    if (ret == -1) {
        perror("select"); // error occurred in select()
    } else if (ret == 0) {
        printf("Timeout occurred!  No data after .5 seconds.\n");
        close(sockfd);
    } else {

        if (FD_ISSET(sockfd, &readfds)) {
            int n = 0;

            printf("Reading .... \n");

            while ((n = read(sockfd, &c, 1)) > 0) {
                printf("%d %d\n", c, n);
                buffer[index] = c;
                index++;
            }
            buffer[index] = 0x00;
        }
    }
    //printf("Buffer %s\n", buffer);
    close(sockfd);
    return buffer;
}