cmake_minimum_required(VERSION 3.16)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 1.0.${PATCH_VERSION})

project(Allocator VERSION ${PROJECT_VESRION})

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Allocator main.cpp)

if(WIN32)
    set_target_properties(Allocator PROPERTIES OUTPUT_NAME "Allocator-${PROJECT_VERSION}-Windows")
endif()

install(TARGETS Allocator DESTINATION bin)

target_compile_options(Allocator PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
)

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "dmitryanons@gmail.com")

if(WIN32)
    set(CPACK_GENERATOR ZIP) 
    set(CPACK_SOURCE_GENERATOR ZIP)
elseif(UNIX)
    set(CPACK_GENERATOR DEB) 
    set(CPACK_SOURCE_GENERATOR TGZ)
endif()

include(CPack)

