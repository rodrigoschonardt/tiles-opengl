cmake_minimum_required(VERSION 3.20)
project(codigo)

set(CMAKE_CXX_STANDARD 20)

# Homebrew prefix (works for both Intel and Apple Silicon)
if(APPLE)
    if(EXISTS "/opt/homebrew")
        set(HOMEBREW_PREFIX "/opt/homebrew")  # Apple Silicon
    else()
        set(HOMEBREW_PREFIX "/usr/local")     # Intel
    endif()
    list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX}")
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(codigo main.cpp stb_image.cpp)

target_include_directories(codigo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(codigo PRIVATE
        OpenGL::GL
        GLEW::GLEW
        glfw
        glm::glm
)