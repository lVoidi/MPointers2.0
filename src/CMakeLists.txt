add_library(mpointers_lib
    memory_manager.cpp
)

target_include_directories(mpointers_lib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${PROTO_OUTPUT_DIR}
)

target_link_libraries(mpointers_lib
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc++
        mpointers_proto
)

add_executable(memory_manager_server
    main.cpp
)

target_include_directories(memory_manager_server
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${PROTO_OUTPUT_DIR}
)

target_link_libraries(memory_manager_server
    PRIVATE
        mpointers_lib
        mpointers_proto
) 