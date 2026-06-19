#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define PEENGINE
#include <GANGOD-OBJ.h>
#include <GANGOD-RENDER.h>
#include <GANGOD-GEOMETRY.h>

#include "Include/imgui.h"
#include "Include/imgui_impl_glfw.h"
#include "Include/imgui_impl_opengl3.h"

#include "UI.h"
//that'll be so fucking long!!!!!!!!!!!!!!!!!!!
int main(){
    // ------------------------------ WINDOW ------------------------------
    if (!glfwInit()) return -1;
    // GL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "GANGOD IS ENGINE!!!! IM PICKLE!!", NULL, NULL);
    if (!window) {
        // oh fuck
        glfwTerminate();
        return -1;
    }
    // we created window!!
    glfwMakeContextCurrent(window);
    glewInit();
    //creatin yo
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& yo = ImGui::GetIO(); (void)yo;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        if(glfwGetKey(window, GLFW_KEY_S)){
            Editor::BUILD();
        }

        // ---------IMGUI---------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ---------IMGUI---------

        // ---------UI------------
        Editor::DrawHierarchy();
        Editor::DrawInspector();
        Editor::DrawScene();
        // ---------UI------------
        ImGui::Render();
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}