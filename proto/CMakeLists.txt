find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# Get absolute path to proto file
get_filename_component(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/memory_manager.proto" ABSOLUTE)
get_filename_component(PROTO_DIR "${PROTO_PATH}" DIRECTORY)

# Generate C++ files from proto
set(PROTO_SRCS "${PROTO_OUTPUT_DIR}/memory_manager.pb.cc")
set(PROTO_HDRS "${PROTO_OUTPUT_DIR}/memory_manager.pb.h")
set(GRPC_SRCS "${PROTO_OUTPUT_DIR}/memory_manager.grpc.pb.cc")
set(GRPC_HDRS "${PROTO_OUTPUT_DIR}/memory_manager.grpc.pb.h")

add_custom_command(
    OUTPUT "${PROTO_SRCS}" "${PROTO_HDRS}" "${GRPC_SRCS}" "${GRPC_HDRS}"
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out="${PROTO_OUTPUT_DIR}"
         --cpp_out="${PROTO_OUTPUT_DIR}"
         -I "${PROTO_DIR}"
         --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
         "${PROTO_PATH}"
    DEPENDS "${PROTO_PATH}")

# Add library
add_library(mpointers_proto
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    ${GRPC_SRCS}
    ${GRPC_HDRS}
)

target_include_directories(mpointers_proto
    PUBLIC
        ${PROTO_OUTPUT_DIR}
)

target_link_libraries(mpointers_proto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc++
) 