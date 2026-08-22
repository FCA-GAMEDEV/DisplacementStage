cmake_minimum_required(VERSION 3.20)
project(DisplacementStage VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Dependências ──────────────────────────────────────────────
find_package(OpenGL REQUIRED)
find_package(glfw3  CONFIG REQUIRED)
find_package(GLEW   CONFIG REQUIRED)
find_package(glm    CONFIG REQUIRED)
find_package(OpenCL REQUIRED)   # fornecido pelo CUDA Toolkit

# ── Fontes ────────────────────────────────────────────────────
set(SOURCES
    src/ContactScene.cpp
    src/CustomScene.cpp
    src/DisplacementMap.cpp
    src/DisplacementStage.cpp
    src/ForceScene.cpp
    src/Game.cpp
    src/Interface.cpp
    src/Interoperability.cpp
    src/main.cpp
    src/MenuScene.cpp
    src/MorphingScene.cpp
    src/Scene.cpp
    src/SceneManager.cpp
    src/ShaderManager.cpp
    src/Terrain.cpp
    src/TestScene.cpp
    src/Texture.cpp
    src/TimeManager.cpp
)

add_executable(DisplacementStage ${SOURCES})

# ── Includes ──────────────────────────────────────────────────
target_include_directories(DisplacementStage PRIVATE
    src/
    vendor/stb/     # stb_image.h
    vendor/         # contains CL/opencl.hpp
)

# ── Link ──────────────────────────────────────────────────────
target_link_libraries(DisplacementStage PRIVATE
    OpenGL::GL
    OpenGL::GLU
    glfw
    GLEW::GLEW
    glm::glm
    OpenCL::OpenCL
)

# ── Windows: subsistema console para ver mensagens de erro ────
if(WIN32)
    set_target_properties(DisplacementStage PROPERTIES
        WIN32_EXECUTABLE FALSE
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:DisplacementStage>"
    )
endif()

# ── Copiar pasta data/ para o diretório de saída ──────────────
add_custom_command(TARGET DisplacementStage POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/bin/data"
        "$<TARGET_FILE_DIR:DisplacementStage>/data"
    COMMENT "Copiando bin/data para o diretório de build"
)

# ── Copiar DLLs de runtime (GLFW, GLEW) ──────────────────────
add_custom_command(TARGET DisplacementStage POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:DisplacementStage>
        $<TARGET_FILE_DIR:DisplacementStage>
    COMMAND_EXPAND_LISTS
    COMMENT "Copiando DLLs de runtime"
)
