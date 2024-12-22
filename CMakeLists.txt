cmake_minimum_required(VERSION 3.19 FATAL_ERROR)
project("simulator")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "c++ standart: ${CMAKE_CXX_STANDARD}")

SET(SRCS 
    src/source/main.cpp 
    src/source/cpu.cpp 
    src/source/cpu_defs.cpp 
    src/source/decode.cpp
    src/source/elf_loader.cpp
    src/source/memory.cpp
    src/source/program_loader.cpp
    src/source/sim.cpp
)

add_executable(simulator ${SRCS})

target_include_directories(simulator 
    PUBLIC
        src/include/
)

set(ASAN_FLAGS "-fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fstack-protector-strong -fcheck-new -fstrict-overflow")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g3 -DDEBUG -ggdb -D_FORTIFY_SOURCE=3 ${ASAN_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -march=native -flto -g3 -DNDEBUG")

set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Og -g3 -DDEBUG -ggdb -D_FORTIFY_SOURCE=3 ${ASAN_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -O2 -march=native -flto -g3 -DNDEBUG")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

# CPMAddPackage(
#   NAME asmjit
#   GITHUB_REPOSITORY asmjit/asmjit
#   GIT_TAG 0b3aec39d18a98a87449f031a469b60aedae1a9b
#   OPTIONS 
#     "ASMJIT_STATIC TRUE"
# )
# if(asmjit_ADDED)
#     target_link_libraries(simulator PRIVATE asmjit)
#     # message(STATUS "asmjit src dir: ${asmjit_SOURCE_DIR}")
#     target_include_directories(simulator PUBLIC "${asmjit_SOURCE_DIR}/src")
# endif()

message(STATUS "try to add ELFIO:")

CPMAddPackage(
    NAME ELFIO
    GITHUB_REPOSITORY serge1/ELFIO
    # GIT_TAG Release_3.9
    GIT_TAG main
)

if(ELFIO_ADDED)
    target_include_directories(simulator PUBLIC "${ELFIO_SOURCE_DIR}/")
endif()

message(STATUS "try to add spdlog:")

CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    GIT_TAG v1.15.0
)

if (spdlog_ADDED)
    # message(STATUS "spdlog src dir: ${spdlog_SOURCE_DIR}")
    target_link_libraries(simulator PRIVATE spdlog)
    target_include_directories(simulator PUBLIC "${spdlog_SOURCE_DIR}/include/")
endif()

