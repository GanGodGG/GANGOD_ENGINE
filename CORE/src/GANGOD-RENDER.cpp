#include "../headers/GANGOD-RENDER.h"
#include "../headers/GANGOD-OBJ.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"



void Image2D::LoadImage(const std::string& path){
    this->path = path;
    int nrCh;
    image = stbi_load(this->path.c_str(), &x, &y, &nrCh, 0);
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGB;
    if(nrCh == 4) format = GL_RGBA;
    else if(nrCh == 1) format = GL_RED;

    if(image){
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, format, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "ERROR: FAILED TO LOAD IMAGE '" << path << "'" << std::endl;
    }
    stbi_image_free(image);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try{
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch(std::ifstream::failure& e){
        std::cout << "ERROR: SHADER FILE NOT SUCCESSFULLY READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR: VERTEX SHADER COMPILATION FAILED\n" << infoLog << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR: FRAGMENT SHADER COMPILATION FAILED\n" << infoLog << std::endl;
    }

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR: SHADER PROGRAM LINK FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Sprite::Sprite()
: shader("../shaders/mainShader.vert", "../shaders/mainShader.frag"), image("../sprites/default.png")
{
    Init();
}

Sprite::Sprite(const Image2D& image, const Shader& shader)
: shader(shader), image(image)
{
    Init();
}

void Sprite::Init()
{
    vertices = {
        Vertex(glm::vec3(0, 0, 0), glm::vec2(0, 1), glm::vec3(1, 1, 1)),
        Vertex(glm::vec3(1, 0, 0), glm::vec2(1, 1), glm::vec3(1, 1, 1)),
        Vertex(glm::vec3(1, 1, 0), glm::vec2(1, 0), glm::vec3(1, 1, 1)),
        Vertex(glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(1, 1, 1))
    };
    indices = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);
    std::cout << "POS OFFSET: " << offsetof(Vertex, pos) << std::endl;
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, map));
    glEnableVertexAttribArray(1);
    std::cout << "MAP OFFSET: " << offsetof(Vertex, map) << std::endl;
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    std::cout << "COLOR OFFSET: " << offsetof(Vertex, color) << std::endl;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

void Sprite::Draw(std::function<void(unsigned int)> onDraw)
{
    glUseProgram(shader.id);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
    int uni_asp = glGetUniformLocation(shader.id, "aspectRatio");
    glUniform1f(uni_asp, enggan::AspectRatio);
    onDraw(shader.id);
    glBindVertexArray(0);
}