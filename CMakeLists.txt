cmake_minimum_required(VERSION 3.10)

project(HelloWorld)

set(CMAKE_CXX_STANDARD 17)

add_executable(HelloWorld src/main.cpp)

add_subdirectory(thirdparty/glfw)

target_link_libraries(HelloWorld PRIVATE glfw)

if(WIN32)
    target_link_libraries(HelloWorld PRIVATE 
        ${CMAKE_SOURCE_DIR}/thirdparty/pvr/Library/Windows_x86_64/libEGL.lib
        ${CMAKE_SOURCE_DIR}/thirdparty/pvr/Library/Windows_x86_64/libGLESv2.lib
    
    )
elseif(UNIX)
    target_link_libraries(HelloWorld PRIVATE
        GLESv2
    )
endif()

target_include_directories(HelloWorld PRIVATE 
    thirdparty/glfw/include
    src
)

if(WIN32)
    target_include_directories(HelloWorld PRIVATE
        thirdparty/pvr/include
    )
elseif(UNIX)
endif()

target_include_directories(HelloWorld PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/tinyGLTF
)

target_compile_definitions(HelloWorld PRIVATE 
    -DTINYGLTF_IMPLEMENTATION
    -DSTB_IMAGE_IMPLEMENTATION
    -DSTB_IMAGE_WRITE_IMPLEMENTATION
)