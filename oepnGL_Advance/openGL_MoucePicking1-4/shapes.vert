#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal; // 法向量
layout(location = 2) in vec2 aTexCoords;
out vec2 TexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 Normal;
out vec3 FragPos; // 顶点的世界坐标位置

void main(){
     FragPos = vec3(model * vec4(aPos,1.0f));
     Normal = mat3(transpose(inverse(model))) * aNormal;
     TexCoords = aTexCoords;
     gl_Position =  projection * view * model * vec4(aPos, 1.0);
}
