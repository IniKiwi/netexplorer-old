#include "../protocol.h"

#define HTTP_RECV_BUFFER 10000

int http_get(int sockfd, port_t addr, struct network_task_info info, char* buffer, size_t buffer_size , const char* path){
    char* http_req = malloc(strlen(path)+100);
    sprintf(http_req,"GET %s HTTP/1.0\r\n\r\n",path);

    size_t http_req_s = strlen(http_req);
    char* http_ret = buffer;

    int sent = 0;
    while (sent < http_req_s){
        int bytes = send(sockfd,http_req+sent,http_req_s-sent, 0);
        if (bytes < 0)
            return 0;
        if (bytes == 0)
            break;
        sent+=bytes;
    };

    bzero(http_ret, buffer_size);

    int total = buffer_size-1;
    int received = 0;
    while (received < total){
        int bytes = recv(sockfd,http_ret+received,total-received, 0);
        if (bytes < 0)
            return 0;
        if (bytes == 0)
            break;
        received+=bytes;
    };
    return received;
}

uint8_t http_find(int sockfd, port_t addr, struct network_task_info info, const char* str){
    char* http_ret = malloc(HTTP_RECV_BUFFER);
    http_get(sockfd, addr, info, http_ret, HTTP_RECV_BUFFER, str);

    char* http_content = strstr(http_ret,"\r\n\r\n");

    if(strstr(http_content,str) != NULL){
        free(http_ret);
        return 1;
    }

    free(http_ret);
    return 0;
}

http_server_info_t protocol_get_http_server_info(int sockfd, port_t addr, struct network_task_info info, char* msg){
    //Index of
    const char* path = "/";
    char* http_ret = malloc(HTTP_RECV_BUFFER);
    http_get(sockfd, addr, info, http_ret, HTTP_RECV_BUFFER, path);

    int http_ret_l = strlen(http_ret);
    int http_ret_wl = 0;
    for(int i=0;i<HTTP_RECV_BUFFER;i++){
        if(http_ret[i] == ' ' || http_ret[i] == '\n' || http_ret[i] == '\r' || http_ret[i] == ';'){
            http_ret[i] = '\0';
            http_ret_wl++;
        }
    }

    http_server_info_t ret_info;
    ret_info.status = STATUS_OK;

    char* http_ret_i = http_ret;
    http_ret_i += strlen(http_ret_i)+1;
    ret_info.return_code = atoi(http_ret_i);
    if(ret_info.return_code == 0){
        ret_info.status = STATUS_FAIL;
        return ret_info;
    }

    char ret_type_found = 0;

    while(http_ret_i < http_ret+HTTP_RECV_BUFFER){
        //printf("%s|",http_ret_i);
        if(strcmp(http_ret_i,"Content-Type:")==0){
            http_ret_i += strlen(http_ret_i)+1;
            ret_info.return_type = malloc(strlen(http_ret_i)+1);
            strcpy(ret_info.return_type,http_ret_i);
            ret_type_found = 1;
            break;
            //printf("found: %s\n",http_ret_i);
        }
        http_ret_i += strlen(http_ret_i)+1;
    }
    if(ret_type_found == 0){
        ret_info.return_type = malloc(3);
        strcpy(ret_info.return_type,"-");
    }
    sprintf(msg,"\e[33mhttp://%s:%d%s \e[39m(%d %s)",addr.addr, addr.port, path, ret_info.return_code, ret_info.return_type);

    free(http_ret);
    return ret_info;
}