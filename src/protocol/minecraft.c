#include "../protocol.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <json-c/json.h>

#define HANDSHAKE_SIZE 1024
#define PROTOCOL_VERSION 210

ssize_t read_byte(const int sfd, void *buf) {
    ssize_t nread;
    nread = recv(sfd, buf, 1, 0);
    if (nread == -1) {
        return 0;
    }
    return nread;
}

int read_varint(const int sfd) {
    int numread = 0;
    int result = 0;
    int value;
    char byte;
    do {
        if (read_byte(sfd, &byte) == 0){
            return 0;
        }
        value = byte & 0x7F;
        result |= value << (7 * numread);

        numread++;

        if (numread > 5) {
            return 0;
        }
    } while ((byte & 0x80) != 0);

    return result;
}

minecraft_server_info_t protocol_get_minecraft_server_info(int sockfd, port_t addr, struct network_task_info info){
    char request[] = {0x1, 0x0};
    char byte;

    size_t host_len = strlen(addr.addr);
    uint8_t packet_buffer[HANDSHAKE_SIZE];
    size_t packet_len = 1 /* packet id */ + 2 /* Protocol version */;
    packet_len += 1 /* str len */ + host_len;
    packet_len += 2; // port
    packet_len += 1; // state

    size_t i = 0;
    packet_buffer[i++] = packet_len;
    packet_buffer[i++] = 0; /* packet id */
    packet_buffer[i++] = PROTOCOL_VERSION;
    packet_buffer[i++] = 1; /* encoded protocol version - varint */
    packet_buffer[i++] = host_len;
    memcpy(packet_buffer + i, addr.addr, host_len);
    i += host_len;
    packet_buffer[i++] = (addr.port >> 8) & 0xFF; /* port little-endian */
    packet_buffer[i++] = addr.port & 0xFF;
    packet_buffer[i] = 1; // next state

    packet_len += 1; /* add length byte */

    if (send(sockfd, packet_buffer, packet_len, 0) != packet_len) {
        return (minecraft_server_info_t){.status = STATUS_FAIL};
    }
    if (send(sockfd, request, 2, 0) != 2) {
        return (minecraft_server_info_t){.status = STATUS_FAIL};
    }
    
    read_varint(sockfd);
    
    if (read_byte(sockfd, &byte) == 0) { /* read packet id */
        return (minecraft_server_info_t){.status = STATUS_FAIL};
    }

    if (byte != 0) {
        return (minecraft_server_info_t){.status = STATUS_FAIL};
    }
    size_t json_len = read_varint(sockfd);
    size_t json_len2 = json_len;
    ssize_t nread;
    char* result_json = malloc(json_len+1);
    size_t json_r = 0;
    char* tmp_json[4096];
    while(json_len > 0) {
        nread = recv(sockfd, tmp_json, 4096, 0);
        if (nread == -1) {
            return (minecraft_server_info_t){.status = STATUS_FAIL};
        }

        json_len -= nread;

        //fwrite(tmp_json, 1, nread, stdout);
        memcpy(result_json+json_r,tmp_json,nread);
        json_r += nread;
    }
    result_json[json_len2] = '\0';
    printf("%s\n",result_json);


    //json_object obj = json_tokener_parse(result_json);


    return (minecraft_server_info_t){.status = STATUS_FAIL};
        
}