include(ExternalProject)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(CONFIGURE_DEBUG "--enable-debug")
else()
  set(CONFIGURE_DEBUG "")
endif()

set(liblo_CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/vendor/liblo/configure --prefix=${CMAKE_SOURCE_DIR}/build ${CONFIGURE_DEBUG} --enable-static --disable-shared)
externalproject_add(liblo
  SOURCE_DIR liblo
  CONFIGURE_COMMAND ${liblo_CONFIGURE_COMMAND}
)
