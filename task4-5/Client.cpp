#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <string>

#define M 2 // Количество рядов
#define N 2 // Количество шкафов в ряду
#define K 3 // Количество книг в шкафе
#define BUFSIZE 2048

struct Catalog {
    int data[M][N][K];
};

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in servAddr;
    unsigned short servPort;
    char *servIP;

    if (argc != 3) {
       fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
       exit(1);
    }

    servIP = argv[1];
    servPort = atoi(argv[2]);
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket() failed");
        exit(1);
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(servIP);
    servAddr.sin_port        = htons(servPort);
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        perror("connect() failed");
        exit(1);
    }
    int row;
    int cnt;
    std::string str;
    char buffer[BUFSIZE];
    while (true) {
        cnt = recv(sock, buffer, BUFSIZE, 0);
        if (cnt == 0) {
            std::cout << "Connection closed prematurely\n";
            break;
        }
        if (cnt < 0) {
            perror("recv() failed");
            break;
        } else {
            row = atoi(buffer);
            printf("Dealing with row #%d\n", row);
            for (int n = 0; n < N; ++n) {
                for (int k = 0; k < K; ++k) {
                    str += std::to_string(row * N * K + n * K + k) + " ";
                }
            }
            if (send(sock, str.c_str(), str.length(), 0) != str.length()) {
                perror("send() failed");
                break;
            }
        }
        str.clear();
    }
    close(sock);
    exit(0);
}
