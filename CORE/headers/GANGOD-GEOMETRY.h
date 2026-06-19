#pragma once
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
class Vertex{
    public:
    glm::vec3 pos;
    glm::vec2 map;
    glm::vec3 color;

    Vertex();

    Vertex(glm::vec3 posiion, glm::vec2 map, glm::vec3 color);
};