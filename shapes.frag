#version 330 core
// [1] 环境光照
// out vec4 FragColor;
// uniform vec3 objectColor;
// uniform vec3  lightColor;
// void main(){

//     // [1] 环境光照
//     // 设置一个常量环境因子
//     float ambientStrength = 0.1;
//     vec3 ambient = ambientStrength * lightColor;
//     vec3 result = ambient * objectColor;
//     FragColor = vec4(result,1.0f);
// }
// [1] 环境光照

// [2] 漫反射光照
out vec4 FragColor;
uniform vec3 objectColor;
uniform vec3  lightColor;
uniform vec3 lightPos; // 光源位置
in vec3 Normal;
in vec3 FragPos;
void main(){

    // 设置一个常量环境因子
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // 将法线单位化
    vec3 norm = normalize(Normal);
    // 将光的方向向量单位化
    vec3 lightDir = normalize(lightPos -  FragPos);
    // 只有在小于90度时才会贡献漫反射光照
    float diff = max(dot(norm,lightDir),0.0f);
    // 漫反射光照值
    vec3 diffuse= diff * lightColor;

    vec3 result = (ambient + diffuse)* objectColor;
    FragColor = vec4(result,1.0f);
}
