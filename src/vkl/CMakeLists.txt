project(vkl VERSION 1.0 DESCRIPTION "Vulkan Learning Project Engine" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRC_FILE vkl_window.cpp
        vkl_device.cpp
        vkl_swap_chain.cpp
        vkl_renderer.cpp
        vkl_buffer.cpp
        vkl_model.cpp
        vkl_camera.cpp
        vkl_descriptor.cpp
        vkl_object.cpp
        io/assimp_loader.cpp
        vkl_texture.cpp
        vkl_compute_pipeline.cpp
        vkl_scene.cpp
        bvh/vkl_bvh.cpp
        system/path_tracing_compute_system.cpp
        vkl_image.cpp
)

add_library(${PROJECT_NAME} ${SRC_FILE})

add_definitions(-DSHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shader")
add_definitions(-DDATA_DIR="${CMAKE_SOURCE_DIR}/data")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${VK_SDK_INCLUDE})

target_link_libraries(${PROJECT_NAME} glm glfw)

############## Build SHADERS #######################

# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
)

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/shader/*.frag"
        "${PROJECT_SOURCE_DIR}/shader/*.vert"
        "${PROJECT_SOURCE_DIR}/shader/*.comp"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/shader/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shader" $<TARGET_FILE_DIR:${PROJECT_NAME}>/shader
)

if (GLSL_VALIDATOR)
    add_dependencies(vkl Shaders)
endif ()