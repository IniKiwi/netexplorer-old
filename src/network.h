#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "storage.h"

enum{
    LOG_ALL,
    LOG_OK
};

struct network_task_info{
    uint16_t threads;
    uint32_t requests;
    uint32_t timeout;
    uint8_t log_mode;
    uint16_t port;
};

int network_addr_search(const char* addr, struct network_task_info info);
int network_search_local(uint8_t subnet, struct network_task_info info);
int network_search_task(const char* addr, struct network_task_info info);
int network_explore(const char* addr, struct network_task_info info);
void* network_search_random(void* _info);
void network_addr_req(const char* addr, uint16_t port, struct network_task_info info);