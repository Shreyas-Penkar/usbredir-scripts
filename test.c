#include <stdio.h>
#include <sys/types.h>
#include "connect.h"

#define HOST "localhost"
#define PORT "1337"

int main(void) {
    int sock = connect_socket(HOST, PORT);
    if (sock == -1) {
        fprintf(stderr, "Failed to connect to %s:%s\n", HOST, PORT);
        return 1;
    }

    unsigned char buf[REDIR_BUF_SIZE];
    ssize_t n = receive_data(sock, buf, sizeof(buf));
    if (n > 0)
        print_buffer(buf, n);

    close_socket(sock);
    return 0;
}