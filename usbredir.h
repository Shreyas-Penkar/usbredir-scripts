// usbredir.h
#ifndef USBREDIR_H
#define USBREDIR_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>


#define USB_REDIR_BUF_SIZE 1024

// HID Descriptor for Interface 0 (9 bytes)
static const uint8_t USB_REDIR_HELLO_PACKET[68] = {
    // "usbredirserver 0.8.0" padded to 64 bytes
    'u','s','b','r','e','d','i','r','s','e','r','v','e','r',' ','0','.','8','.','0',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,
    // capabilities: 0xdf000000
    0xdf, 0x00, 0x00, 0x00
};

#define HIGH_BYTE(x) (((x) >> 8) & 0xff)

// ---- Protocol constants ----
enum {
    RT_HELLO = 0,
    RT_CTRL_PKT = 100,
    RT_RESET = 3,
    RT_SET_CONFIG = 6,
};

// ---- Speed constants ----
#define USB_REDIR_SPEED_LOW     0
#define USB_REDIR_SPEED_FULL    1
#define USB_REDIR_SPEED_HIGH    2
#define USB_REDIR_SPEED_SUPER   3
#define USB_REDIR_SPEED_UNKNOWN 255

// ---- Structs ----
#pragma pack(push, 1)
typedef struct {
    uint32_t type;
    uint32_t length;
    int32_t  id;
} usbredir_header;

typedef struct {
    uint32_t interface_count;
    uint8_t  interface[32];
    uint8_t  interface_class[32];
    uint8_t  interface_subclass[32];
    uint8_t  interface_protocol[32];
} usbredir_interface_info_header;

typedef struct {
    uint8_t  type[32];
    uint8_t  interval[32];
    uint8_t  interface[32];
    uint16_t max_packet_size[32];
    uint32_t max_streams[32];
} usbredir_ep_info_header;

typedef struct {
    uint8_t  speed;
    uint8_t  device_class;
    uint8_t  device_subclass;
    uint8_t  device_protocol;
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t device_version_bcd;
} usbredir_device_connect_header;

typedef struct {
    uint8_t  endpoint;
    uint8_t  request;
    uint8_t  requesttype;
    uint8_t  status;
    uint16_t value;
    uint16_t index;
    uint16_t length;
} usbredir_control_packet_header;

typedef struct {
    uint8_t configuration;
} usb_redir_set_configuration_header;

#pragma pack(pop)

typedef struct {
    int type;
    int id;
    usbredir_control_packet_header cphdr;
    uint8_t raw[USB_REDIR_BUF_SIZE + 12];
    int raw_len;
    int has_data;
} Resp;

// ---- Functions ----
size_t prepare_hello_pkt(uint8_t *out);
size_t prepare_interface_info_pkt(uint8_t *out);
size_t prepare_ep_info_pkt(uint8_t *out);
size_t prepare_connect_pkt(uint8_t *out);
Resp check_resp(int sock, int verbose);
int recv_all(int sock, uint8_t *buf, size_t len);
#endif
