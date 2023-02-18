cmake_minimum_required(VERSION 3.0)

project(inikiwi-netexplorer)

set(
    SRCS
    src/main.c
    src/network.c
    src/storage.c
    src/protocol/minecraft.c
    src/protocol/http.c
    src/cJSON.c
)

add_executable(inikiwi-netexplorer ${SRCS} ${HEADERS})
target_link_libraries(inikiwi-netexplorer PRIVATE sqlite3 pthread )