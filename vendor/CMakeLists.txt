include(ExternalProject)

set(liblo_CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/vendor/liblo/configure --prefix=${CMAKE_SOURCE_DIR}/build --enable-static --disable-shared)
externalproject_add(liblo
  SOURCE_DIR liblo
  CONFIGURE_COMMAND ${liblo_CONFIGURE_COMMAND}
)
