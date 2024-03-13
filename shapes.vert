#version 330 core
// [1] 环境光照
// layout(location = 0) in vec3 aPos;
// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;
// void main(){
//      gl_Position =  projection * view * model * vec4(aPos, 1.0);
// }
// [1] 环境光照

// [2] 漫反射光照
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal; // 法向量
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 Normal;
out vec3 FragPos; // 顶点的世界坐标位置
void main(){
     gl_Position =  projection * view * model * vec4(aPos, 1.0);
     FragPos = vec3(model * vec4(aPos,1.0f));
     // inverse 求逆  transpose求转置
     //在漫反射光照部分，光照表现并没有问题，这是因为我们没有对物体进行任何缩放操作，所以我们并不真的需要使用一个法线矩阵，而是仅以模型矩阵乘以法线就可以。
     // 但是如果你会进行不等比缩放，使用法线矩阵去乘以法向量就是必须的了。 Normal = mat3(model) * aNormal;
      Normal = mat3(transpose(inverse(model))) * aNormal;
}
// [2] 漫反射光照
