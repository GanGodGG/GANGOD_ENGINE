#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <vector>
#include <functional>
#include <GLFW/glfw3.h>
#include "../headers/GANGOD-GEOMETRY.h"

struct Image2D{
    int x = 64, y = 64;
    unsigned char* image;
    std::string path;
    unsigned int texid;
    void LoadImage(const std::string& path);
    void LoadImage(const char* path){
        LoadImage(std::string(path));
    }
    Image2D() : image(nullptr), texid(0) {}
    Image2D(const std::string& path){
        LoadImage(path);
    }
    Image2D(const char* path){
        LoadImage(std::string(path));
    }
};


struct Shader{
    unsigned int id;
    Shader(const char* vertexPath, const char* fragmentPath);
};

class Sprite{
private:
    unsigned int VAO, VBO, EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    void Init();
public:
    Image2D image;
    Shader shader;
    Sprite();
    Sprite(const Image2D& image, const Shader& shader);

    void Draw(std::function<void(unsigned int)> onDraw);
};