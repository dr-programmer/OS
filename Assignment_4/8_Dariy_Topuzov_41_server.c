#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 12345
#define MAX_CLIENTS  FD_SETSIZE
#define BUF_SIZE 1024
#define MAX_QUEUE 10

int main(int argc, char *argv[]) {
    int listen_fd, conn_fd, port, max_fd, i;
    int client_fds[MAX_CLIENTS];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    fd_set all_fds, read_fds;
    char buf[BUF_SIZE];
    ssize_t nbytes;

    port = (argc > 1) ? atoi(argv[1]) : PORT;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(listen_fd, MAX_QUEUE) < 0) {
        perror("listen");
        exit(1);
    }

    for (i = 0; i < MAX_CLIENTS; i++) client_fds[i] = -1;

    FD_ZERO(&all_fds);
    FD_SET(listen_fd, &all_fds);
    max_fd = listen_fd;

    printf("Server started on port %d\n", port);

    while (1) {
        read_fds = all_fds;
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(1);
        }

        if (FD_ISSET(listen_fd, &read_fds)) {
            cli_len = sizeof(cli_addr);
            conn_fd = accept(listen_fd, (struct sockaddr*)&cli_addr, &cli_len);
            if (conn_fd < 0) {
                perror("accept");
                continue;
            }
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] < 0) {
                    client_fds[i] = conn_fd;
                    break;
                }
            }
            if (i == MAX_CLIENTS) {
                fprintf(stderr, "Too many clients\n");
                close(conn_fd);
            } else {
                FD_SET(conn_fd, &all_fds);
                if (conn_fd > max_fd) max_fd = conn_fd;
                printf("New connection: fd %d\n", conn_fd);
            }
        }

        for (i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];
            if (fd < 0) continue;
            if (FD_ISSET(fd, &read_fds)) {
                nbytes = read(fd, buf, BUF_SIZE);
                if (nbytes <= 0) {
                    if (nbytes < 0) perror("read");
                    printf("Client fd %d disconnected\n", fd);
                    close(fd);
                    FD_CLR(fd, &all_fds);
                    client_fds[i] = -1;
                } else {
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        int out_fd = client_fds[j];
                        if (out_fd >= 0 && out_fd != fd) {
                            if (write(out_fd, buf, nbytes) < 0) {
                                perror("write");
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}