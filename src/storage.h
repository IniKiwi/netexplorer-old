#include <inttypes.h>
#include <stdio.h>
#pragma once

#define MAX_RESULTS 25000

enum{
    IP_TYPE_UNKNOW,
    IP_TYPE_PRINTER,
};

enum{
    PORT_TYPE_NONE,
    PORT_TYPE_UNKNOW,
    PORT_TYPE_HTTP,
    PORT_TYPE_HTTPS,
    PORT_TYPE_FTP,
    PORT_TYPE_SSH,
    PORT_TYPE_MINECRAFT,
};

typedef struct {
    uint16_t port;
    char* addr;
    uint16_t type;
} port_t;


typedef struct{
    size_t results_size;
    port_t* results;
} storage_result_t;

typedef struct{
    uint8_t ip[4][2];
    uint16_t port[2];
} storage_search_range_ipv4_t;

port_t port_new(const char* addr, uint16_t port, uint16_t type);

void storage_init();
void storage_close();
void storage_save_port(port_t port);
storage_result_t storage_search_port(uint16_t port);
storage_search_range_ipv4_t storage_decode_ipv4(const char* addr);