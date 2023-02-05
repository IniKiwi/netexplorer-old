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

minecraft_server_info_t protocol_get_minecraft_server_info(int sockfd, port_t addr, struct network_task_info info);