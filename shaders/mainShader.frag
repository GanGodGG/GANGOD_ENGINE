#version 330 core

in vec3 ColGlob;
in vec2 TexCoord;
uniform sampler2D ourTexture;
out vec4 FragColor;

void main()
{
    vec4 t = texture(ourTexture, TexCoord);
    if(t.a < 0.2){
        discard;
    }
    FragColor = t;
}
