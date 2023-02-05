cmake_minimum_required(VERSION 3.0)

project(IniKiwiNetExplorer)

set(
    SRCS
    src/main.c
    src/network.c
    src/storage.c
    src/protocol/minecraft.c
)

add_executable(IniKiwiNetExplorer ${SRCS} ${HEADERS})
target_link_libraries(IniKiwiNetExplorer PRIVATE sqlite3 pthread json-c)