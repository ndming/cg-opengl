cmake_minimum_required(VERSION 3.25)
project(CG2023)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Print build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("-- Build type is Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("-- Build type is Release")
else()
    message("-- Build type is Unknown")
endif()

# Dependencies
# GLAD
add_subdirectory(external/glad)
include_directories(external/glad/include)
# GLFW
add_subdirectory(external/glfw)
include_directories(external/glfw/include)
# GLM
add_subdirectory(external/glm)
include_directories(external/glm/glm)
# stb
include_directories(external/stb/include)

# Add include directory
include_directories(include)

# Add source files
set(SOURCES
        src/Camera.cpp
        src/Context.cpp
        src/Engine.cpp
        src/EntityManager.cpp
        src/IndexBuffer.cpp
        src/LightManager.cpp
        src/RenderableManager.cpp
        src/Renderer.cpp
        src/Scene.cpp
        src/Shader.cpp
        src/Texture.cpp
        src/TransformManager.cpp
        src/VertexBuffer.cpp
        src/View.cpp
        src/drawable/Color.cpp
        src/drawable/Cone.cpp
        src/drawable/Cube.cpp
        src/drawable/Cylinder.cpp
        src/drawable/Drawable.cpp
        src/drawable/Frustum.cpp
        src/drawable/Mesh.cpp
        src/drawable/Orbit.cpp
        src/drawable/Pyramid.cpp
        src/drawable/Ring.cpp
        src/drawable/Sphere.cpp
        src/drawable/Aura.cpp
        src/drawable/Tetrahedron.cpp
        src/drawable/Trace.cpp
        src/utils/DescentTracer.cpp
        src/utils/MediaExporter.cpp
        src/utils/DescentIterator.cpp
        src/utils/SolarSystem.cpp
        src/utils/TextureLoader.cpp
        external/stb/stb_image.cpp
        external/stb/stb_image_write.cpp
)

# Copy resources in res/ to the binary directory
if(${CMAKE_GENERATOR} MATCHES "Visual Studio")
    # Visual Studio builds the executable into a separate directory
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR}/Debug)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR}/Release)
    endif()
else()
    file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})
endif()

# Add target
add_executable(CG2023 main.cpp ${SOURCES})

# Linking
target_link_libraries(CG2023 PRIVATE glfw glad glm)