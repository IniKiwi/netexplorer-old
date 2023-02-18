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

enum{
    NETWORK_TASK_TASK,
    NETWORK_TASK_MULTITHREAD,
};

struct network_task_info{
    uint8_t skip;
    uint16_t threads;
    uint32_t requests;
    uint32_t timeout;
    uint8_t log_mode;
    uint32_t port;
};

struct network_search_task_args{
    const char* addr;
    struct network_task_info info;
};

int network_search_task(const char* addr, struct network_task_info info, int task_opt);
void storage_search_print(storage_search_range_ipv4_t search, struct network_task_info info);
void network_addr_req(const char* addr, uint16_t port, struct network_task_info info);