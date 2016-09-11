#include "Net2Serial.h"
#include "Messages.h"

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    Messages *msg = new Messages();

    msg->addMessage("Hello World");
    msg->addMessage("Merck & Co");
    msg->addMessage("1234567890");

    for (int i = 0; i < msg->size(); i++) {
        /* Send message to the server */
        if (write(sockfd, msg->getMessage(i), strlen(msg->getMessage(i))) < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
    }

    return 0;
}
