#include "../protocol.h"

#define FTP_LS "list"
#define BUFFER_SIZE 1500

// TODO
ftp_entry_t* ftp_ls(int controlfd, int datafd){
    int bytes;
    uint8_t buffer = malloc(BUFFER_SIZE);
    send(controlfd,FTP_LS,strlen(FTP_LS),0);
    while(1){
        bzero(buffer,BUFFER_SIZE);
        bytes = recv(datafd,buffer,BUFFER_SIZE-1,0);
        printf("%s\n",buffer);
    }
}

ftp_server_info_t protocol_get_ftp_server_info(int controlfd, port_t addr, struct network_task_info info, char* msg){
    int datafd;
}