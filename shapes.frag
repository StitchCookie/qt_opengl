#version 330 core
struct Material {
    sampler2D diffuse;  // 漫反射贴图
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 direction;  // 聚光灯方向
    vec3 position;
    float  cutOff;     // 聚光灯切角
    float  outerCutOff; // 外光切切角

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform Light light;
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
