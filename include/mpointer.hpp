#pragma once

#include <memory>
#include <grpcpp/grpcpp.h>
#include "memory_manager.grpc.pb.h"

namespace mpointers {

template<typename T>
class MPointer {
private:
    uint32_t id;
    static std::shared_ptr<grpc::Channel> channel;
    static std::unique_ptr<MemoryManagerService::Stub> stub;

public:
    // Constructor
    MPointer() : id(-1) {}
    
    // Destructor
    ~MPointer() {
        if (id != -1) {
            DecreaseRefRequest request;
            request.set_id(id);
            DecreaseRefResponse response;
            stub->DecreaseRef(nullptr, &request, &response);
        }
    }

    // Static initialization
    static void Init(const std::string& address) {
        channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
        stub = MemoryManagerService::NewStub(channel);
    }

    // Create new instance
    static MPointer<T> New() {
        MPointer<T> ptr;
        CreateRequest request;
        request.set_size(sizeof(T));
        request.set_type(typeid(T).name());
        
        CreateResponse response;
        grpc::ClientContext context;
        
        auto status = stub->Create(&context, request, &response);
        if (status.ok() && response.success()) {
            ptr.id = response.id();
        }
        return ptr;
    }

    // Operators
    T operator*() const {
        GetRequest request;
        request.set_id(id);
        
        GetResponse response;
        grpc::ClientContext context;
        
        auto status = stub->Get(&context, request, &response);
        if (status.ok() && response.success()) {
            T value;
            std::memcpy(&value, response.value().data(), sizeof(T));
            return value;
        }
        throw std::runtime_error("Failed to get value");
    }

    MPointer<T>& operator=(const MPointer<T>& other) {
        if (this != &other) {
            // Decrease ref count of current
            if (id != -1) {
                DecreaseRefRequest request;
                request.set_id(id);
                DecreaseRefResponse response;
                stub->DecreaseRef(nullptr, &request, &response);
            }

            // Copy and increase ref count
            id = other.id;
            if (id != -1) {
                IncreaseRefRequest request;
                request.set_id(id);
                IncreaseRefResponse response;
                stub->IncreaseRef(nullptr, &request, &response);
            }
        }
        return *this;
    }

    uint32_t operator&() const {
        return id;
    }

    void operator=(const T& value) {
        if (id != -1) {
            SetRequest request;
            request.set_id(id);
            request.set_value(&value, sizeof(T));
            
            SetResponse response;
            grpc::ClientContext context;
            
            auto status = stub->Set(&context, request, &response);
            if (!status.ok() || !response.success()) {
                throw std::runtime_error("Failed to set value");
            }
        }
    }

    bool operator==(const MPointer<T>& other) const {
        return id == other.id;
    }

    bool operator!=(const MPointer<T>& other) const {
        return !(*this == other);
    }
};

// Static member initialization
template<typename T>
std::shared_ptr<grpc::Channel> MPointer<T>::channel = nullptr;

template<typename T>
std::unique_ptr<MemoryManagerService::Stub> MPointer<T>::stub = nullptr;

} // namespace mpointers 