#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // 颜色变量的属性位置值为 1
out vec4 vertexColor;
// uniform float xOffset;
// out vec3 ourPosition;

void main(){
     gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    //gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0f);
    vertexColor = vec4(aColor,1.0f);
   // ourPosition = aPos;
}
