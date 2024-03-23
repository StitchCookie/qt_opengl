#version 330 core

float near = 0.1;
float far = 100.0;
float API_linearDepth(float depth)
{
   float z = depth * 2.0  -1.0;
   return (2.0 * near * far)/(far + near - z * (far - near));
}
out vec4 FragColor;

void main(){


    // 使用深度值来输出
    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0);

    // 计算出线性深度值来输出

     float depth = API_linearDepth(gl_FragCoord.z) / (far - near);
     FragColor = vec4(vec3(depth),1.0);
}
