INCLUDE_DIRECTORIES(${ROSS_SOURCE_DIR} ${ROSS_BINARY_DIR})
INCLUDE_DIRECTORIES(${RIO_SOURCE_DIR} ${RIO_BINARY_DIR})

# copy the data file to the build directory
SET(_block_data_file ${CMAKE_CURRENT_SOURCE_DIR}/data_file.lnk)
CONFIGURE_FILE(${_block_data_file} ${CMAKE_CURRENT_BINARY_DIR}/data.vbench COPYONLY)

SET(module_loader_srcs
    module_main.c
    gates_model.h
    gate_rio.c
    library.h
    library.c
    routing.h
    routing.c
)

ADD_EXECUTABLE(module_loader ${module_loader_srcs})

TARGET_LINK_LIBRARIES(module_loader ROSS RIO m)
