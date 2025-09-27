# CMakeLists.txt - Versão Final "Manual"
cmake_minimum_required(VERSION 3.10)
project(cliente_cpp)

# --- ENCONTRANDO AS BIBLIOTECAS COM PKG-CONFIG ---
# Usamos o pkg-config para encontrar tanto o Protobuf quanto o gRPC
find_package(PkgConfig REQUIRED)
pkg_check_modules(PROTOBUF REQUIRED protobuf)
pkg_check_modules(GRPC REQUIRED grpc++)

# --- GERAÇÃO MANUAL DOS ARQUIVOS .proto ---
# 1. Encontra os executáveis do compilador protoc e do plugin do gRPC
find_program(PROTOC_COMPILER protoc)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

# 2. Define os nomes dos arquivos que serão gerados a partir de urls.proto
set(PROTO_GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/urls.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/urls.grpc.pb.cc
)
set(PROTO_GENERATED_HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/urls.pb.h
    ${CMAKE_CURRENT_BINARY_DIR}/urls.grpc.pb.h
)

# 3. Comando customizado para executar o protoc e gerar os arquivos
add_custom_command(
    OUTPUT ${PROTO_GENERATED_SOURCES} ${PROTO_GENERATED_HEADERS}
    COMMAND ${PROTOC_COMPILER}
            --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
            --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
            -I ${CMAKE_CURRENT_SOURCE_DIR}
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
            ${CMAKE_CURRENT_SOURCE_DIR}/urls.proto
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/urls.proto
)

# 4. Cria um "alvo" para garantir que os arquivos sejam gerados antes da compilação
add_custom_target(
    generate_proto_files ALL
    DEPENDS ${PROTO_GENERATED_SOURCES} ${PROTO_GENERATED_HEADERS}
)

# --- COMPILAÇÃO DO CLIENTE ---
# Adiciona o executável, incluindo os fontes que acabamos de gerar
add_executable(cliente
    cliente.cpp
    ${PROTO_GENERATED_SOURCES}
)

# Adiciona os diretórios de include necessários
target_include_directories(cliente PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS}
)

# Linka as bibliotecas necessárias
target_link_libraries(cliente PRIVATE
    ${PROTOBUF_LIBRARIES}
    ${GRPC_LIBRARIES}
)