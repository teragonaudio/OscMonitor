cmake_minimum_required(VERSION 2.8)
project(OscMonitor)

file(GLOB oscmonitor_SOURCES source/*.c)
file(GLOB oscmonitor_HEADERS source/*.h)
file(GLOB mrswatson_SDK_SOURCES source/*/*.c)
file(GLOB mrswatson_SDK_HEADERS source/*/*.h)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX=1)
elseif(APPLE)
  add_definitions(-DMACOSX=1)
elseif(MSVRC)
  add_definitions(/DWINDOWS=1)
endif()

include_directories(source)
include_directories(${CMAKE_BINARY_DIR}/include)
include_directories(vendor/liblo)
add_subdirectory(vendor)

add_executable(${PROJECT_NAME}
  ${oscmonitor_SOURCES}
  ${oscmonitor_HEADERS}
  ${mrswatson_SDK_SOURCES}
  ${mrswatson_SDK_HEADERS}
)
target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/lib/liblo.a m pthread)
