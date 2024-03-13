#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
uniform vec3  lightColor;
uniform vec3 lightPos; // 光源位置
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;  // 摄像机观察位置
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
    float speculatStrength = 0.5;  //镜面强度变量
    vec3 viewDir = normalize(viewPos - FragPos);   // 视角方向向量  摄像机位置向量减去顶点向量
    vec3 reflectDir = reflect(-lightDir,norm);  // 光源反射的方向向量
    // 计算镜面分量 最后，这个部分将余弦值的32次方，这通常用来增强高光部分，使其更加明显。
    // 这样做是为了增强镜面高光的效果，使得夹角越小的区域有更强的高光
    float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
    vec3 specular = speculatStrength * spec * lightColor;
    vec3 result = (ambient + diffuse + specular)* objectColor;
    FragColor = vec4(result,1.0f);
}
