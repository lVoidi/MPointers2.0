#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <map>
#include <grpcpp/grpcpp.h>
#include "memory_manager.grpc.pb.h"

namespace mpointers {

struct MemoryBlock {
    uint32_t id;
    size_t size;
    size_t offset;
    std::string type;
    uint32_t refCount;
    bool isUsed;
};

class GarbageCollector {
public:
    explicit GarbageCollector(class MemoryManager* manager);
    void start();
    void stop();

private:
    void run();
    class MemoryManager* manager;
    bool running;
    std::thread thread;
    std::chrono::milliseconds interval;
};

class MemoryManager {
public:
    static MemoryManager* getInstance();
    
    bool initialize(uint16_t port, size_t memSize, const std::string& dumpFolder);
    void start();
    void stop();
    
    // Server management
    void setServer(std::unique_ptr<grpc::Server> srv);
    void waitForServer(); // Wait for server shutdown
    
    // Memory operations
    uint32_t createBlock(size_t size, const std::string& type);
    bool setValue(uint32_t id, const void* value, size_t size);
    bool getValue(uint32_t id, void* value, size_t size);
    bool increaseRefCount(uint32_t id);
    bool decreaseRefCount(uint32_t id);
    
    // Memory management
    void defragment();
    void dumpMemoryState();
    
    // Friend declarations
    friend class GarbageCollector;
    friend class MemoryManagerServiceImpl;

private:
    MemoryManager() = default;
    ~MemoryManager();
    
    // Singleton instance
    static MemoryManager* instance;
    
    // Memory management
    void* memoryBlock;
    size_t totalSize;
    std::string dumpFolderPath;
    
    // Block tracking
    std::vector<MemoryBlock> blocks;
    uint32_t nextBlockId;
    
    // Synchronization
    std::mutex mutex;
    
    // GC
    std::unique_ptr<GarbageCollector> gc;
    
    // gRPC server
    std::unique_ptr<grpc::Server> server;
};

class MemoryManagerServiceImpl final : public MemoryManagerService::Service {
public:
    // Constructor and destructor
    MemoryManagerServiceImpl();
    ~MemoryManagerServiceImpl() override;

    grpc::Status Create(grpc::ServerContext* context, 
                       const CreateRequest* request,
                       CreateResponse* response) override;
                       
    grpc::Status Set(grpc::ServerContext* context,
                     const SetRequest* request,
                     SetResponse* response) override;
                     
    grpc::Status Get(grpc::ServerContext* context,
                     const GetRequest* request,
                     GetResponse* response) override;
                     
    grpc::Status IncreaseRef(grpc::ServerContext* context,
                            const IncreaseRefRequest* request,
                            IncreaseRefResponse* response) override;
                            
    grpc::Status DecreaseRef(grpc::ServerContext* context,
                            const DecreaseRefRequest* request,
                            DecreaseRefResponse* response) override;
};

} // namespace mpointers 