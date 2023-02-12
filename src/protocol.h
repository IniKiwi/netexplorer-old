#pragma once
#include <inttypes.h>

#include "storage.h"
#include "network.h"

enum{
    STATUS_OK,
    STATUS_FAIL,
};

typedef struct{
    uint8_t status;
    int players;
    int maxplayers;
    uint16_t protocol;
    char* protocol_str;
} minecraft_server_info_t;

typedef struct{
    uint8_t status;
    int return_code;
    char* return_type;
    size_t data_size;
} http_server_info_t;

typedef struct{
    uint8_t status;
    size_t storage;
    size_t files;
} ftp_server_info_t;

typedef struct{
    uint8_t type;
    char* name;
} ftp_entry_t;

minecraft_server_info_t protocol_get_minecraft_server_info(int sockfd, port_t addr, struct network_task_info info, char* msg);
http_server_info_t protocol_get_http_server_info(int sockfd, port_t addr, struct network_task_info info, char* msg);
ftp_server_info_t protocol_get_ftp_server_info(int controlfd, port_t addr, struct network_task_info info, char* msg);