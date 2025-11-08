#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* ---------- Public: connect_socket ---------- */
int connect_socket(const char *host, const char *port_str) {
    int sock;
    struct addrinfo hints, *res;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    ret = getaddrinfo(host, port_str, &hints, &res);
    if (ret != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return -1;
    }

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        perror("socket");
        freeaddrinfo(res);
        return -1;
    }

    // Set timeout
    struct timeval timeout = {0, 500000};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // Reuse address
    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // Connect
    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        close(sock);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    return sock;
}


/* ---------- Public: receive_data ---------- */
ssize_t receive_data(int sock, unsigned char *buf, size_t bufsize) {
    ssize_t n = recv(sock, buf, bufsize, 0);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            fprintf(stderr, "Timeout: no data within 1 second.\n");
        else
            perror("recv");
        return -1;
    } else if (n == 0) {
        fprintf(stderr, "Peer closed connection.\n");
        return 0;
    }
    return n;
}

/* ---------- Public: print_buffer ---------- */
void print_buffer(const unsigned char *buf, size_t len) {
    printf("Received %zu bytes:\n", len);
    for (size_t i = 0; i < len; i++) {
        unsigned char c = buf[i];
        if (c >= 32 && c <= 126)
            putchar(c);
        else
            printf("\\x%02x", c);
    }
    putchar('\n');
}

/* ---------- Public: close_socket ---------- */
void close_socket(int sock) {
    if (sock >= 0)
        close(sock);
}
