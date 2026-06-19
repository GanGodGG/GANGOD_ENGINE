cmake_minimum_required(VERSION 3.10)

project(Platengine VERSION 0.1 LANGUAGES CXX)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
find_package(PkgConfig REQUIRED)
# Используем pkg-config для поиска библиотек
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(GL REQUIRED gl)
add_library(CORE_PLAT_ENGINE STATIC 
    CORE/src/GANGOD-OBJ.cpp
    CORE/src/GANGOD-GEOMETRY.cpp
    CORE/src/GANGOD-RENDER.cpp
    )
# Подключаем пути заголовочных файлов
target_include_directories(CORE_PLAT_ENGINE PUBLIC ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GL_INCLUDE_DIRS} CORE/include CORE/headers)
# Линковка
add_executable(PEENGINE 
    editor/main.cpp
    editor/Include/imgui.h
    editor/Include/imgui.cpp
    editor/Include/imgui_impl_glfw.h
    editor/Include/imgui_impl_glfw.cpp
    editor/Include/imgui_impl_opengl3.h
    editor/Include/imgui_impl_opengl3.cpp
    editor/Include/imgui_impl_opengl3_loader.h
    editor/Include/imgui_draw.cpp
    editor/Include/imgui_widgets.cpp
    editor/Include/imgui_tables.cpp
    editor/Include/imgui_demo.cpp
    editor/UI.h
    editor/UI.cpp
    )
target_link_libraries(PEENGINE PRIVATE
    CORE_PLAT_ENGINE
    ${GLFW_LIBRARIES} 
    ${GLEW_LIBRARIES} 
    ${GL_LIBRARIES}
    glm
    dl 
    pthread
)

add_executable(Game game/main.cpp)
target_link_libraries(Game PRIVATE
    CORE_PLAT_ENGINE
    ${GLFW_LIBRARIES} 
    ${GLEW_LIBRARIES} 
    ${GL_LIBRARIES}
    glm
    dl 
    pthread
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
