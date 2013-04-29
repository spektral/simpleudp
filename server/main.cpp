#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#define BUFLEN 256

int main(int args, char *argv[]) {
    const short port = 50000;
    const timespec ts = { 0, 200000000 };
    const char symbols[4] = { '|', '/', '-', '\\' };

    int s;
    socklen_t socklen;
    char buf[BUFLEN];
    sockaddr_in si_local = {};
    sockaddr_in si_remote = {};

    // Create UDP socket
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("Could not create socket");
        exit(1);
    }

    // Set socket to non-blocking
    int sflags = fcntl(s, F_GETFL);
    sflags |= O_NONBLOCK;
    fcntl(s, F_SETFL, sflags);

    si_local.sin_family = AF_INET;
    si_local.sin_addr.s_addr = INADDR_ANY;
    si_local.sin_port = htons(port);

    // Assign address to socket
    int r = bind(s, (const sockaddr*)&si_local, sizeof(si_local));
    if (r == -1) {
        perror("Error in bind");
        exit(1);
    }

    int ticker = 0;
    while (true) {
        int rcvd;
        do {
            rcvd = recvfrom(s, buf, BUFLEN, 0, (sockaddr*)&si_remote, &socklen);
            if (rcvd == -1 && errno != EAGAIN) {
                perror("recvfrom()");
                exit(1);
            }
            if (r >= 0) {
                buf[r] = '\0';
                printf("Recv from: %s:%d\nData: %s\nLength: %d\n",
                        inet_ntoa(si_remote.sin_addr), ntohs(si_remote.sin_port), buf, rcvd);
            }
        } while (rcvd >= 0);

        printf("\r%c", symbols[ticker % 4]);
        fflush(stdout);
        ticker++;

        nanosleep(&ts, NULL);
    }

    return 0;
}
