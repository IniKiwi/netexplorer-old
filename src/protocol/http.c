#include "../protocol.h"

#define HTTP_RECV_BUFFER 10000

http_server_info_t protocol_get_http_server_info(int sockfd, port_t addr, struct network_task_info info, char* msg){
    const char* http_req = "GET /path?query_string HTTP/1.0\r\n\r\n";
    size_t http_req_s = strlen(http_req);
    char* http_ret = malloc(HTTP_RECV_BUFFER);

    int sent = 0;
    while (sent < http_req_s){
        int bytes = send(sockfd,http_req+sent,http_req_s-sent, 0);
        if (bytes < 0)
            return (http_server_info_t){.status = STATUS_FAIL};
        if (bytes == 0)
            break;
        sent+=bytes;
    };

    bzero(http_ret, HTTP_RECV_BUFFER);

    int total = HTTP_RECV_BUFFER-1;
    int received = 0;
    while (received < total){
        int bytes = recv(sockfd,http_ret+received,total-received, 0);
        if (bytes < 0)
            return (http_server_info_t){.status = STATUS_FAIL};
        if (bytes == 0)
            break;
        //fwrite(http_ret+received, 1, bytes, stdout);
        //printf("\n======================\n");
        received+=bytes;
    };
    http_server_info_t ret_info;
    ret_info.status = STATUS_OK;

    //printf(http_ret);

    int http_ret_l = strlen(http_req);
    int http_ret_wl = 0;
    for(int i=0;i<HTTP_RECV_BUFFER;i++){
        if(http_ret[i] == ' ' || http_ret[i] == '\n' || http_ret[i] == '\r' || http_ret[i] == ';'){
            http_ret[i] = '\0';
            http_ret_wl++;
        }
    }

    char* http_ret_i = http_ret;
    http_ret_i += strlen(http_ret_i)+1;
    ret_info.return_code = atoi(http_ret_i);

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
    sprintf(msg,"(%d %s)",ret_info.return_code,ret_info.return_type);

    free(http_ret);
    return ret_info;
}