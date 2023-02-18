#include <sqlite3.h>
#include "storage.h"
#include "network.h"
#include <stdlib.h>
#include <string.h>

sqlite3 *db;

port_t port_new(const char* addr, uint16_t port, uint16_t type){
    port_t ret;
    ret.addr = malloc(strlen(addr));
    strcpy(ret.addr,addr);
    ret.port = port;
    ret.type = type;
    return ret;
}

void storage_init(){
    sqlite3_open("database.db", &db);
    sqlite3_exec(db,"CREATE TABLE IF NOT EXISTS ports(id INTEGER PRIMARY KEY, addr TEXT, port INTEGER, type INTEGER, first_seen TEXT, last_seen TEXT)",NULL,NULL,NULL);
}

void storage_save_port(port_t port){
    //check if exists
    sqlite3_stmt *stmt;
    char* querys = sqlite3_mprintf("SELECT * FROM ports WHERE addr='%q' AND port=%d;", port.addr, port.port);
    if(sqlite3_prepare_v2(db, querys, -1, &stmt, NULL) == SQLITE_OK){
        if (sqlite3_step(stmt) == SQLITE_ROW){
            char* query = sqlite3_mprintf("UPDATE ports SET last_seen=datetime('now') WHERE addr='%q' AND port=%d;", port.addr, port.port);
            sqlite3_exec(db,query,NULL,NULL,NULL);
        }
        else{
            //save
            char* query = sqlite3_mprintf("insert into ports(addr,port,type,first_seen,last_seen) values ('%q',%d,%d,datetime('now'),datetime('now'));", port.addr, port.port, port.type);
            sqlite3_exec(db,query,NULL,NULL,NULL);
        }
    }

    sqlite3_finalize(stmt);
};

storage_search_range_ipv4_t storage_decode_ipv4(const char* addr){
    storage_search_range_ipv4_t result;
    result.ip_search = IP_SEARCH_RANGE;
    result.port_search = PORT_SEARCH_POPULAR;
    result.port[0] = 0;
    result.port[1] = 65535;

    size_t iplen = strlen(addr);
    char* iplist = malloc(strlen(addr));
    int objects = 1;
    strcpy(iplist,addr);
    for(int i=0;i<iplen;i++){
        if(iplist[i] == '.' || iplist[i] == ':'){
            iplist[i] = '\0';
            objects++;
        }
    }

    char* sip_ptr = iplist;
    if(objects == 4 || objects == 5){
        for(int i=0;i<4;i++){
            if(strcmp(sip_ptr,"*")==0){
                result.ip[i][0] = 0;
                result.ip[i][1] = 255;
                sip_ptr += strlen(sip_ptr)+1;
                continue;
            }
            result.ip[i][0] = atoi(sip_ptr);
            result.ip[i][1] = atoi(sip_ptr);
            sip_ptr += strlen(sip_ptr)+1;
        }

        if(objects == 5){
            if(strcmp(sip_ptr,"*")==0){
                result.port[0] = 1;
                result.port[1] = 65535;
                result.port_search = PORT_SEARCH_RANGE;
                return result;
            }
            else if(strcmp(sip_ptr,"pop")==0 || strcmp(sip_ptr,"popular")==0){
                result.port_search = PORT_SEARCH_POPULAR;
                return result;
            }
            else if(strcmp(sip_ptr,"random")==0 || strcmp(sip_ptr,"rand")==0){
                result.port_search = PORT_SEARCH_RANDOM;
                return result;
            }
            result.port_search = PORT_SEARCH_RANGE;
            result.port[0] = atoi(sip_ptr);
            result.port[1] = atoi(sip_ptr);
        }
    }
    else if(objects == 2 || objects == 1){
        result.ip_search = IP_SEARCH_RANDOM;
        if(strcmp(sip_ptr,"random")==0 || strcmp(sip_ptr,"rand")==0){
            result.ip_search = IP_SEARCH_RANDOM;
        }
        sip_ptr += strlen(sip_ptr)+1;

        if(objects == 2){
            if(strcmp(sip_ptr,"*")==0){
                result.port[0] = 1;
                result.port[1] = 65535;
                result.port_search = PORT_SEARCH_RANGE;
                return result;
            }
            else if(strcmp(sip_ptr,"pop")==0 || strcmp(sip_ptr,"popular")==0){
                result.port_search = PORT_SEARCH_POPULAR;
                return result;
            }
            else if(strcmp(sip_ptr,"random")==0 || strcmp(sip_ptr,"rand")==0){
                result.port_search = PORT_SEARCH_RANDOM;
                return result;
            }
            result.port_search = PORT_SEARCH_RANGE;
            result.port[0] = atoi(sip_ptr);
            result.port[1] = atoi(sip_ptr);
        }
    }
    return result;
}

void storage_search_print(storage_search_range_ipv4_t search, struct network_task_info info){
    sqlite3_stmt *stmt;

    char* tmpip = malloc(50);
    for(int ip0=search.ip[0][0];ip0<=search.ip[0][1];ip0++){
        for(int ip1=search.ip[1][0];ip1<=search.ip[1][1];ip1++){
            for(int ip2=search.ip[2][0];ip2<=search.ip[2][1];ip2++){
                for(int ip3=search.ip[3][0];ip3<=search.ip[3][1];ip3++){
                    for(int port=search.port[0];port<=search.port[1];port++){
                        sprintf(tmpip,"%d.%d.%d.%d",ip0,ip1,ip2,ip3);
                        char* querys = sqlite3_mprintf("SELECT * FROM ports WHERE addr='%q' AND port=%d;", tmpip, port);
                        if(sqlite3_prepare_v2(db, querys, -1, &stmt, NULL) == SQLITE_OK){
                            while (sqlite3_step(stmt) != SQLITE_DONE) {
                                network_addr_req(sqlite3_column_text(stmt,1),sqlite3_column_int(stmt,2),info);
                            }
                            sqlite3_finalize(stmt);
                        }
                    }
                }
            }
        }
    }
    free(tmpip);
};

void storage_close(){
    sqlite3_close(db);
}