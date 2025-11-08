// usbredir.c
#include "usbredir.h"
#include "connect.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

const char *usbredir_type_enum_get(uint32_t type) {
    switch (type) {
        case 0: return "hello";
        case 1: return "device_connect";
        case 2: return "device_disconnect";
        case 3: return "reset";
        case 4: return "interface_info";
        case 5: return "ep_info";
        case 6: return "set_configuration";
        case 7: return "get_configuration";
        case 8: return "configuration_status";
        case 9: return "set_alt_setting";
        case 10: return "get_alt_setting";
        case 11: return "alt_setting_status";
        case 12: return "start_iso_stream";
        case 13: return "stop_iso_stream";
        case 14: return "iso_stream_status";
        case 15: return "start_interrupt_receiving";
        case 16: return "stop_interrupt_receiving";
        case 17: return "interrupt_receiving_status";
        case 18: return "alloc_bulk_streams";
        case 19: return "free_bulk_streams";
        case 20: return "bulk_streams_status";
        case 21: return "cancel_data_packet";
        case 22: return "filter_reject";
        case 23: return "filter_filter";
        case 24: return "device_disconnect_ack";
        case 100: return "data_control_packet";
        case 101: return "data_bulk_packet";
        case 102: return "data_iso_packet";
        case 103: return "data_interrupt_packet";
        default: return "UNKNOWN";
    }
}

size_t prepare_hello_pkt(uint8_t *out) {
    usbredir_header hdr = {0, sizeof(USB_REDIR_HELLO_PACKET), 0};
    memcpy(out, &hdr, sizeof(hdr));
    memcpy(out + sizeof(hdr), USB_REDIR_HELLO_PACKET, sizeof(USB_REDIR_HELLO_PACKET));
    return sizeof(hdr) + sizeof(USB_REDIR_HELLO_PACKET);
}

size_t prepare_interface_info_pkt(uint8_t *out) {
    usbredir_interface_info_header info = {0};
    info.interface_count = 1;
    info.interface_class[0] = 3;

    usbredir_header hdr = {4, sizeof(info), 0};
    memcpy(out, &hdr, sizeof(hdr));
    memcpy(out + sizeof(hdr), &info, sizeof(info));
    return sizeof(hdr) + sizeof(info);
}

size_t prepare_ep_info_pkt(uint8_t *out) {
    usbredir_ep_info_header ep = {0};
    for (int i = 0; i < 32; i++) {
        ep.type[i] = 0;
        ep.interval[i] = 0;
        ep.interface[i] = 0;
        ep.max_packet_size[i] = 0;
        ep.max_streams[i] = 0;
    }
    ep.max_packet_size[0] = 0x40;

    usbredir_header hdr = {5, sizeof(ep), 0};
    memcpy(out, &hdr, sizeof(hdr));
    memcpy(out + sizeof(hdr), &ep, sizeof(ep));
    return sizeof(hdr) + sizeof(ep);
}

size_t prepare_connect_pkt(uint8_t *out) {
    usbredir_device_connect_header conn = {
        USB_REDIR_SPEED_HIGH, 0, 0, 0,
        0x1130, 0x3101, 0x0002
    };

    usbredir_header hdr = {1, sizeof(conn), 0};
    memcpy(out, &hdr, sizeof(hdr));
    memcpy(out + sizeof(hdr), &conn, sizeof(conn));
    return sizeof(hdr) + sizeof(conn);
}

int recv_all(int sock, uint8_t *buf, size_t len) {
    size_t total = 0;
    while (total < len) {
        uint8_t temp[512];
        size_t remaining = len - total;
        size_t chunk_size = buf ? remaining : (remaining > sizeof(temp) ? sizeof(temp) : remaining);
        uint8_t *target = buf ? buf + total : temp;

        ssize_t n = recv(sock, target, chunk_size, 0);
        if (n == 0) {
            fprintf(stderr, "Connection closed by peer.\n");
            return -1;
        } else if (n < 0) {
            perror("recv");
            return -1;
        }

        total += (size_t)n;
    }
    return (int)total;
}

Resp check_resp(int sock, int verbose) {
    Resp resp = {0};
    uint8_t hdr[12];

    // Receive 12-byte header
    if (recv_all(sock, hdr, 12) <= 0) {
        printf("Socket Timeout: no data to receive\n");
        resp.type = -1;
        return resp;
    }

    // Parse little-endian fields manually
    uint32_t key = hdr[0] | (hdr[1] << 8) | (hdr[2] << 16) | (hdr[3] << 24);
    uint32_t len = hdr[4] | (hdr[5] << 8) | (hdr[6] << 16) | (hdr[7] << 24);
    uint32_t id  = hdr[8] | (hdr[9] << 8) | (hdr[10] << 16) | (hdr[11] << 24);

    resp.type = key;
    resp.id = id;

    memcpy(resp.raw, hdr, 12);
    resp.raw_len = 12;

    // Print type name
    const char *type_name = usbredir_type_enum_get(key);
    printf("Guest Log [KEY]: %s\n", type_name ? type_name : "UNKNOWN");

    if (verbose) {
        printf("\nkey: %u, length: %u, id: 0x%x\n", key, len, id);
        printf("Raw header bytes: ");
        for (int i = 0; i < 12; i++) printf("%02x ", hdr[i]);
        printf("\n");
    }

    // Safety check for payload size
    if (len > USB_REDIR_BUF_SIZE) {
        fprintf(stderr, "Payload too large: %u bytes\n", len);
        resp.type = -1;
        return resp;
    }

    if (key == RT_HELLO) {
        // Hello packet — discard payload safely
        if (recv_all(sock, NULL, len) < 0) {
            fprintf(stderr, "Failed to discard hello payload\n");
            resp.type = -1;
        }
    } 
    return resp;
}
