#version 330 core
struct Material {
    sampler2D diffuse;  // 漫反射贴图
    sampler2D specular;
    float shininess;
};

struct DirLight{
    vec3 direction;  // 定向光方向 它不需要强度因为是定向光

    vec3 ambient;   // 定向光的环境光照分量
    vec3 diffuse;   // 定向光的漫反射光照分量
    vec3 specular;  // 定向光的镜面光光照分量
};
struct PointLight {
    vec3 position;    // 点光源的位置向量

    float constant;  // 常量 一般为1
    float linear;    // 线性衰减常量
    float quadratic; // 二次方衰减常量

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;       // 内切角的cos值
    float outerCutOff;  // 外切角的cos值

    float constant;  // 常量 一般为1
    float linear;    // 线性衰减常量
    float quadratic; // 二次方衰减常量
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Material material;
uniform PointLight pointLights[4];
uniform DirLight dirLight;
uniform SpotLight spotLight;

vec3 API_calcSpotLight(SpotLight light,vec3 normal,vec3 fragPos,vec3 viewDir);
vec3 API_calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 API_calcDirLight(DirLight light,vec3 normal,vec3 viewDir);


out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;  // 纹理坐标

uniform vec3 viewPos;  // 摄像机观察位置


void main(){
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = API_calcDirLight(dirLight,norm,viewDir);
    for(int i = 0; i < 4; i++)
        result += API_calcPointLight(pointLights[i],norm,FragPos,viewDir);
    result += API_calcSpotLight(spotLight,norm,FragPos,viewDir);
    FragColor = vec4(result,1.0);
}
vec3 API_calcDirLight(DirLight light,vec3 normal,vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);  // 平行光不关心向量强度
    float diff = max(dot(normal,lightDir),0.0);
    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0f),material.shininess);
    // 上面是获取对应光照分量的值 下面则要开始计算了
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}
vec3 API_calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal,lightDir),0.0);
    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // 计算光照衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                               light.quadratic * (distance * distance));

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    // 衰减
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
vec3 API_calcSpotLight(SpotLight light,vec3 normal,vec3 fragPos,vec3 viewDir)
{
    // 光照方向
    vec3 lightDir = normalize(light.position - fragPos);
    //
    // 漫反射
    float diff = max(dot(normal,lightDir),0.0f);
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0f),material.shininess);
    // 距离衰弱
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                               light.quadratic * (distance * distance));
    // 聚光强度
    float theta = dot(lightDir,normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.cutOff) / (epsilon),0.0f,1.0f);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    // 合并计算

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
