#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // 颜色变量的属性位置值为 1
layout (location = 2) in vec2 aTexCord;
out vec4 vertexColor;
out vec2 TexCord;
void main(){
     gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
     vertexColor = vec4(aColor,1.0f);
     //TexCord = vec2(1-aTexCord.x,aTexCord.y);  练习6.3 第一种方法
     TexCord = aTexCord;
}
