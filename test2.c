#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "connect.h"
#include "usbredir.h"
#include "utils.h"

#define HOST "localhost"
#define PORT "1337"

int main(void) {
    int sock = connect_socket(HOST, PORT);
    if (sock < 0) {
        fprintf(stderr, "Connecting to redir socket failed\n");
        return -1;
    }

    check_resp(sock, 0);

    uint8_t payload[1024];

    size_t len = prepare_hello_pkt(payload);
    send(sock, payload, len, 0);

    len = prepare_interface_info_pkt(payload);
    printf("--> user: Sending Interface Info Pkt...\n");
    send(sock, payload, len, 0);

    len = prepare_ep_info_pkt(payload);
    printf("--> user: Sending EP INFO Pkt...\n");
    send(sock, payload, len, 0);

    len = prepare_connect_pkt(payload);
    printf("--> user: Sending Connect Pkt...\n");
    send(sock, payload, len, 0);

    check_resp(sock, 0);

    return 0;
}
