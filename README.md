# MPointers 2.0

Sistema de administración de memoria remota con capacidades de garbage collection y defragmentación.

## Requisitos

- CMake 3.10 o superior
- Compilador C++ con soporte para C++17
- gRPC y Protocol Buffers
- Boost (para las pruebas)

## Compilación

1. Crear directorio de build:
```bash
mkdir build
cd build
```

2. Configurar CMake:
```bash
cmake ..
```

3. Compilar:
```bash
make
```

## Uso

### Memory Manager Server

El servidor se ejecuta con los siguientes parámetros:

```bash
./bin/memory_manager_server <port> <memsize_mb> <dump_folder>
```

Donde:
- `port`: Puerto en el que escuchará el servidor
- `memsize_mb`: Tamaño del bloque de memoria en megabytes
- `dump_folder`: Carpeta donde se guardarán los dumps de memoria

Ejemplo:
```bash
./bin/memory_manager_server 50051 1024 /tmp/memory_dumps
```

### MPointer Library

Para usar la biblioteca de MPointer en tu proyecto:

```cpp
#include <mpointer.hpp>

// Inicializar conexión con el servidor
mpointers::MPointer<int>::Init("localhost:50051");

// Crear un nuevo MPointer
auto ptr = mpointers::MPointer<int>::New();

// Asignar un valor
*ptr = 42;

// Leer un valor
int value = *ptr;
```

## Características

- Administración de memoria remota
- Garbage collection automático
- Defragmentación de memoria
- Conteo de referencias
- Dumps de estado de memoria
- Comunicación vía gRPC

## Estructura del Proyecto

```
.
├── CMakeLists.txt
├── include/
│   ├── memory_manager.hpp
│   └── mpointer.hpp
├── proto/
│   ├── CMakeLists.txt
│   └── memory_manager.proto
└── src/
    ├── CMakeLists.txt
    ├── main.cpp
    └── memory_manager.cpp
```

## Licencia

Este proyecto está licenciado bajo la Licencia MIT.
