#pragma once

#include <GANGOD-OBJ.h>
#include <GANGOD-RENDER.h>
#include <GANGOD-GEOMETRY.h>

#include "Include/imgui.h"
#include "Include/imgui_impl_glfw.h"
#include "Include/imgui_impl_opengl3.h"
#include <typeinfo>
#include "json.hpp"

namespace Editor{
    extern std::shared_ptr<enggan::Object> current;
    extern std::vector<std::shared_ptr<enggan::Object>> All;

    void DrawHierarchy();
    void DrawInspector();
    void DrawScene();

    void BUILD();
}