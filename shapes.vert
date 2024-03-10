#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // 颜色变量的属性位置值为 1
layout (location = 2) in vec2 aTexCord;
out vec4 vertexColor;
out vec2 TexCord;
uniform mat4 RototionMatrix;
void main(){
    gl_Position = RototionMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    vertexColor = vec4(aColor,1.0f);
     TexCord = aTexCord;
}
