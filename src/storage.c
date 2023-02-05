#include <sqlite3.h>
#include "storage.h"
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

void storage_close(){
    sqlite3_close(db);
}