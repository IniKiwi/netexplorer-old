#include "network.h"
#include "storage.h"
#include "protocol.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdatomic.h>

//bolt - green - OK! - normal color - normal text
#define MSG_OK "\e[1m\e[32mOK\e[39m\e[0m"
#define MSG_FAIL "\e[1m\e[31mFAIL\e[39m\e[0m"
#define MSG_SKIPPED "\e[1m\e[33mSKIPPED\e[39m\e[0m"

uint16_t search_ports[] = { 28,
    20,     //FTP
    21,     //FTP
    22,     //SSH
    23,     //telnet
    25,     //smtp
    53,     //dns
    80,     //HTTP
    110,    //POP3 (email)
    119,    //NNTP
    123,    //net time
    143,    //IMAP (email)
    161,    //SNMP
    162,    //SNMP
    194,    //IRC
    179,    //bgp
    443,    //HTTPS
    445,    //PME (microsoft)
    500,    //ISAKMP
    554,    //RTSP (audio/video)
    1080,   //SOCKS (proxy)
    3306,   //mySQL
    3389,   //RDP (windows)
    6443,   //Kubernetes api
    5000,   //flask
    5432,   //PostgreSQL
    8080,   //other http
    25565,  //minecraft java
};

void network_addr_req(const char* addr, uint16_t port, struct network_task_info info){
    if(info.skip==1){
        printf("try connect to %s:%d -> %s\n",addr,port,MSG_SKIPPED);
        return;
    }
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct timeval timeout;      
    timeout.tv_sec = 0;
    timeout.tv_usec = info.timeout;
    char* msg = malloc(1500);
    strcpy(msg,"");
    
    setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in server;
    memset(&server,0,sizeof(struct sockaddr_in));
    server.sin_addr.s_addr = inet_addr(addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    port_t port_result = port_new(addr,port,0);

    if(connect(sockfd, (struct sockaddr*)&server, sizeof(server)) != -1){
        port_result.type = PORT_TYPE_UNKNOW;
        if(port == 25565){
            port_result.type = PORT_TYPE_MINECRAFT;
            minecraft_server_info_t res = protocol_get_minecraft_server_info(sockfd, port_result, info, msg);
        }
        if(port == 80  || port == 8080 || port == 8000){
            port_result.type = PORT_TYPE_HTTP;
            http_server_info_t res = protocol_get_http_server_info(sockfd, port_result, info, msg);
            free(res.return_type);
        }

        storage_save_port(port_result);
    }
    else{
        
    }
    close(sockfd);
    if(port_result.type == 0){
        if(info.log_mode == LOG_ALL){printf("try connect to %s:%d -> %s\n",addr,port,MSG_FAIL);}
        return;
    }

    printf("try connect to %s:%d -> %s %s\n",addr,port,MSG_OK, msg);

    free(msg);
    return;
    
}
atomic_int network_search_task_threads = 1;
void* network_search_task_thread(void* args){
    network_search_task_threads++;
    //printf("%d\n",network_search_task_threads);
    struct network_search_task_args* a = args;
    network_search_task(a->addr,a->info, NETWORK_TASK_TASK);
    network_search_task_threads--;
    //printf("%d\n",network_search_task_threads);
}

int network_search_task(const char* addr, struct network_task_info info, int task_opt){
    srand(time(NULL));
    storage_search_range_ipv4_t search = storage_decode_ipv4(addr);

    char* tmpip = malloc(50);
    if(search.ip_search == IP_SEARCH_RANGE){
        for(int ip0=search.ip[0][0];ip0<=search.ip[0][1];ip0++){
            for(int ip1=search.ip[1][0];ip1<=search.ip[1][1];ip1++){
                for(int ip2=search.ip[2][0];ip2<=search.ip[2][1];ip2++){
                    for(int ip3=search.ip[3][0];ip3<=search.ip[3][1];ip3++){
                        sprintf(tmpip,"%d.%d.%d.%d",ip0,ip1,ip2,ip3);
                        if(search.port_search == PORT_SEARCH_RANGE){
                            for(int port=search.port[0];port<=search.port[1];port++){
                                network_addr_req(tmpip, port, info);
                            }
                        }
                        else if(search.port_search == PORT_SEARCH_POPULAR){
                            for(int i=1;i<search_ports[0];i++){
                                network_addr_req(tmpip, search_ports[i],info);
                            }
                        }
                        else if(search.port_search == PORT_SEARCH_RANDOM){
                            network_addr_req(tmpip, rand()%65535,info);
                        }
                    }
                }
            }
        }
    }
    else if(search.ip_search == IP_SEARCH_RANDOM){
        if(network_search_task_threads < info.threads && task_opt == NETWORK_TASK_MULTITHREAD){
            int s = network_search_task_threads;
            while (network_search_task_threads < info.threads){
                pthread_t thread;
                struct network_search_task_args arg = {
                    .addr = addr,
                    .info = info,
                };
                pthread_create(&thread, NULL, &network_search_task_thread, (void*)&arg);
            }
            while(network_search_task_threads != s){
                
            }
        }
        else{
            for(size_t i=0;i<info.requests;i++){
                sprintf(tmpip,"%d.%d.%d.%d",rand()%255,rand()%255,rand()%255,rand()%255);
                if(search.port_search == PORT_SEARCH_RANGE){
                    for(int port=search.port[0];port<=search.port[1];port++){
                        network_addr_req(tmpip, port, info);
                    }
                }
                else if(search.port_search == PORT_SEARCH_POPULAR){
                    for(int i=1;i<search_ports[0];i++){
                        network_addr_req(tmpip, search_ports[i],info);
                    }
                }
                else if(search.port_search == PORT_SEARCH_RANDOM){
                    network_addr_req(tmpip, rand()%65535,info);
                }
            }
        }
    }
    free(tmpip);
}
