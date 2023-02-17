#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"
#include "network.h"

const char cli_help_msg[] = "IniKiwi's netexplorer\n" \
"actions:\n" \
"explore <ip>\n" \
"scan <ip>\n";

int main(int argc, char *argv[]){
    if(argc == 1){
        printf(cli_help_msg);
        exit(0);
    }
    storage_init();
    struct network_task_info info = {
        .timeout = 500000,
        .threads = 5,
        .requests = 50,
        .log_mode = LOG_ALL,
        .port = 0,
        .skip = 0
    };
    for(int a=1;a<argc;a++){
        if(strcmp("explore",argv[a]) == 0){
            if(argc < a+1){exit(1);}
            network_search_task(argv[a+1],info);
        }
        if(strcmp("search",argv[a]) == 0){
            if(argc < a+1){exit(1);}
            storage_search_print(storage_decode_ipv4(argv[a+1]), info);
        }
        else if(strcmp("-j",argv[a]) == 0){
            if(argc < a+1){exit(1);}
            info.threads = atoi(argv[a+1]);
        }
        else if(strcmp("-p",argv[a]) == 0){
            if(argc < a+1){exit(1);}
            if(strcmp("random",argv[a+1]) == 0){info.port = -1; continue;}
            info.port = atoi(argv[a+1]);
        }
        else if(strcmp("-t",argv[a]) == 0){
            if(argc < a+1){exit(1);}
            info.timeout = atoi(argv[a+1]);
        }
        else if(strcmp("-r",argv[a]) == 0){
            if(argc < a+1){exit(1);}
            info.requests = atoi(argv[a+1]);
        }
        else if(strcmp("--hide-fail",argv[a]) == 0){
            info.log_mode = LOG_OK;
        }
        else if(strcmp("--offline",argv[a]) == 0){
            info.skip = 1;
        }
    }

    storage_close();
}