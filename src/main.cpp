#include "memory_manager.hpp"
#include <iostream>
#include <string>
#include <grpcpp/grpcpp.h>

using namespace mpointers;

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " <port> <memsize_mb> <dump_folder>\n"
              << "  port         - Port number to listen on (use values above 1024)\n"
              << "  memsize_mb   - Size of memory block in megabytes\n"
              << "  dump_folder  - Folder to store memory dumps\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printUsage(argv[0]);
        return 1;
    }

    // Parse command line arguments
    uint16_t port;
    size_t memSize;
    std::string dumpFolder;

    try {
        port = static_cast<uint16_t>(std::stoi(argv[1]));
        if (port < 1024) {
            std::cerr << "Warning: Ports below 1024 require root privileges. Consider using a higher port.\n";
        }
        memSize = static_cast<size_t>(std::stoul(argv[2])) * 1024 * 1024; // Convert MB to bytes
        dumpFolder = argv[3];
    } catch (const std::exception& e) {
        std::cerr << "Error parsing arguments: " << e.what() << "\n";
        printUsage(argv[0]);
        return 1;
    }

    // Initialize MemoryManager
    auto manager = MemoryManager::getInstance();
    if (!manager->initialize(port, memSize, dumpFolder)) {
        std::cerr << "Failed to initialize memory manager\n";
        return 1;
    }

    // Start gRPC server
    std::string serverAddress = "0.0.0.0:" + std::to_string(port);
    MemoryManagerServiceImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    if (!server) {
        std::cerr << "Failed to start server on " << serverAddress << "\n";
        return 1;
    }
    
    // Store the server in MemoryManager
    manager->setServer(std::move(server));
    
    std::cout << "Server listening on " << serverAddress << "\n";

    // Start garbage collector
    manager->start();

    // Wait for server to shutdown
    manager->waitForServer();

    return 0;
} 