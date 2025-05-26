#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 8181
#define SERVER_IP "127.0.0.1"
#define BUF_SIZE 4096
#define OUT_FILE "received_image.png"

int main(int argc, char *argv[]) {
    if(argc != 3 && argc != 1) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        fprintf(stderr, "Or: %s to use default server IP and port\n", argv[0]);
        fprintf(stderr, "Default server IP: %s, Port: %d\n", SERVER_IP, PORT);
        return 1;
    }

    const char *server_ip = (argc == 1) ? SERVER_IP : argv[1];
    int port = (argc == 1) ? PORT : atoi(argv[2]);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return 1;
    }
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(sockfd);
        return 1;
    }
    int outfd = open(OUT_FILE, O_WRONLY | O_CREAT | O_TRUNC);
    if (outfd == -1) {
        perror("open output file");
        close(sockfd);
        return 1;
    }

    char buf[BUF_SIZE];
    ssize_t n;
    while ((n = read(sockfd, buf, BUF_SIZE)) > 0) {
        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(outfd, buf + written, n - written);
            if (w <= 0) break;
            written += w;
        }
    }
    
    close(outfd);
    close(sockfd);
    printf("Image received and saved as %s\n", OUT_FILE);
    return 0;
}