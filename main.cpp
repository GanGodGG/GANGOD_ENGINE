#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../headers/GANGOD-RENDER.h"
#include "../headers/GANGOD-GEOMETRY.h"
#include "../headers/GANGOD-OBJ.h"

float lastTime = 0;
float dt = 0;
int main() {
    
    enggan::Object obj;
    enggan::Object obj2;
    enggan::Object floor;
    enggan::Object cam;
    obj.AddComponent<enggan::Render>();
    obj.AddComponent<enggan::BoxCollider>();
    obj.AddComponent<enggan::Body>();


    obj2.AddComponent<enggan::Render>();
    obj2.AddComponent<enggan::BoxCollider>();
    obj2.AddComponent<enggan::Body>();

    floor.AddComponent<enggan::BoxCollider>();
    floor.AddComponent<enggan::Render>();
    cam.AddComponent<enggan::Camera>();
    obj.pos = glm::vec3(4.0f, -2.0f, 0.0f);
    obj2.pos = glm::vec3(0.0f, -2.0f, 0.0f);
    obj.GetComponent<enggan::Body>()->velocity = { -9, 0 };
    obj.GetComponent<enggan::Body>()->mass = 50;
    obj2.GetComponent<enggan::Body>()->mass = 90;
    floor.pos = glm::vec3(0.0f, -4.0f, 0.0f);
    floor.GetComponent<enggan::BoxCollider>()->size = { 1000, .5f };
    glfwSetFramebufferSizeCallback(window, enggan::ScrMgr::UpdateWindowCallback);

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        dt = currentTime - lastTime;
        glEnable(GL_DEPTH_TEST | GL_BLEND);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        enggan::ObjMgr::Update();
        enggan::ObjMgr::FixUpd(dt);
        glfwSwapBuffers(window);
        glfwPollEvents();
        lastTime = currentTime;
    }

    glfwTerminate();
    return 0;
}