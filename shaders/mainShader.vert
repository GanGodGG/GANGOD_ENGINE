#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 color;

out vec3 ColGlob;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float aspectRatio;
void main(){
    vec3 collapsedPos = position / aspectRatio;
    gl_Position = proj * view * model * vec4(collapsedPos, 1.0f);
    ColGlob = color;
    TexCoord = texCoord;
}