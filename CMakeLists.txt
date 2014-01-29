cmake_minimum_required(VERSION 2.8)
project(OscMonitor)

file(GLOB oscmonitor_SOURCES source/*.c)
file(GLOB oscmonitor_HEADERS source/*.h)

add_executable(${PROJECT_NAME} ${oscmonitor_SOURCES} ${oscmonitor_HEADERS})
