#version 330 core
#define NR_POINT_LIGHTS 4
struct Material {
    sampler2D diffuse;  // 漫反射贴图
    sampler2D specular;
    float shininess;
};
uniform Material material;
in vec2 TexCoords;  // 纹理坐标
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

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
vec3 API_calcDirLight(DirLight light,vec3 normal,vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
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

    // 计算衰减
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

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;  // 摄像机观察位置
in vec2 TexCoords;  // 纹理坐标


void main(){
    // 环境光仍然使用漫反射贴图
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    // 计算漫反射夹角
    float diff = max(dot(norm,lightDir),0.0f);
    vec3 diffuse =  light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    // 镜面反射
    // 指向视角向量
    vec3 viewDir = normalize(viewPos - FragPos);
    // 反射方向  参数 入射角方向向量  法向量
    vec3 reflectDir = reflect(-lightDir,norm);
    // 镜面反射指数
    float spec = pow(max(dot(viewDir,reflectDir),0.0f),material.shininess);
    // 镜面光照
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;


    // spotlight
    float theta = dot(lightDir,normalize(-light.direction)); // 实际的光切角
    float epsilon  = (light.cutOff - light.outerCutOff);  // 给定的内光切和外光切的范围相减
    float intensty = clamp((theta - light.outerCutOff) / epsilon,0.0,1.0);  // 计算得出的内切外切聚光范围
    diffuse *= intensty;
    specular *= intensty;


    // 衰减光源
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // 计算衰弱光源
    // 消除环境的衰减，否则在很远的距离处，由于 else 分支中的环境项，聚光灯内部的光会比外部更暗
    ambient *= attenuation;
    diffuse *= attenuation;
    specular  *= attenuation;
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result,0);
}
