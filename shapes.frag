#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
  vec3 positon;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 FragColor;
uniform vec3  lightColor;
uniform vec3 lightPos; // 光源位置
in vec3 Normal;
in vec3 FragPos;
uniform Material material;
uniform Light light;

uniform vec3 viewPos;  // 摄像机观察位置
void main(){

    // 设置环境光 = 光源Xambient材质属性
    vec3 ambient = light.ambient * material.ambient;

    // 将法线单位化
    vec3 norm = normalize(Normal);
    // 将光的方向向量单位化
    vec3 lightDir = normalize(lightPos -  FragPos);
    // 只有在小于90度时才会贡献漫反射光照
    float diff = max(dot(norm,lightDir),0.0f);
    // 漫反射光照值 = 光源 X 角度 X diffuse材质向量
    vec3 diffuse= light.diffuse * (diff * material.diffuse);

    // 镜面光
    vec3 viewDir = normalize(viewPos - FragPos);   // 视角方向向量  摄像机位置向量减去顶点向量
    vec3 reflectDir = reflect(-lightDir,norm);  // 光源反射的方向向量
    // 计算镜面分量 最后，这个部分将余弦值的32次方，这通常用来增强高光部分，使其更加明显。
    // 这样做是为了增强镜面高光的效果，使得夹角越小的区域有更强的高光
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    // 散射属性 = 光源 * 镜面高光的散射/半径 * 散射属性
    vec3 specular = light.specular * (spec * material.specular);
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result,1.0f);
}
