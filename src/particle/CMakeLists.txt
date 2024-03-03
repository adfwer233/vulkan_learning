project(particle VERSION 1.0 DESCRIPTION "Partical Moving in Compute Shader" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRC_FILE main.cpp
        application.cpp
        system/particle_render_system.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILE})

add_definitions(-DDATA_DIR="${CMAKE_SOURCE_DIR}/data")
add_definitions(-DPARTICLE_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shader")

add_subdirectory(shader)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${VK_SDK_INCLUDE})

target_link_libraries(${PROJECT_NAME} glm glfw vkl ray_tracer)

add_dependencies(particle particle_shader)