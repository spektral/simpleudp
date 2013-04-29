#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFLEN 256

int main(int argc, char *argv[]) {
    int s;
    char buf[BUFLEN] = {};
    sockaddr_in si_remote = {};
    socklen_t socklen = sizeof(si_remote);

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("Could not create socket");
        exit(1);
    }

    si_remote.sin_family = AF_INET;
    si_remote.sin_port = htons(50000);
    int r = inet_aton("127.0.0.1", &si_remote.sin_addr);
    if (r == 0) {
        perror("inet_aton()");
        exit(1);
    }

    for (int i = 0; i < 10; i++) {
        printf("Sending packet\n");
        int len = strlen(buf);
        r = sendto(s, buf, len, 0, (sockaddr*)&si_remote, socklen);
        if (r == -1) {
            perror("sendto()");
            exit(1);
        }
    }

    return 0;
}
