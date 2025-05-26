#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 8181
#define IMG_PATH "server/image.png"
#define BUF_SIZE 4096

char *img_path = IMG_PATH;

void *handle_client(void *arg) {
    int clientfd = *(int *)arg;
    free(arg);
    int imgfd = open(img_path, O_RDONLY);
    if (imgfd == -1) {
        perror("open image");
        close(clientfd);
        return NULL;
    }
    
    char buf[BUF_SIZE];
    ssize_t n;
    while ((n = read(imgfd, buf, BUF_SIZE)) > 0) {
        ssize_t sent = 0;
        while (sent < n) {
            ssize_t s = send(clientfd, buf + sent, n - sent, 0);
            if (s <= 0) break;
            sent += s;
        }
    }
    close(imgfd);
    close(clientfd);
    return NULL;
}

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : PORT;
    img_path = (argc > 2) ? argv[2] : IMG_PATH;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(sockfd);
        return 1;
    }
    if (listen(sockfd, 10) == -1) {
        perror("listen");
        close(sockfd);
        return 1;
    }

    printf("Server listening on port %d...\n", port);
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int *clientfd = malloc(sizeof(int));
        if (!clientfd) continue;
        *clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (*clientfd == -1) {
            perror("accept");
            free(clientfd);
            continue;
        }
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, clientfd);
        pthread_detach(tid);
    }
    close(sockfd);
    return 0;
}