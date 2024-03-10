#version 330 core
in  vec4 vertexColor;
out vec4 FragColor;
in vec2 TexCord;
uniform sampler2D textWrap; // 纹理采样器
uniform sampler2D textureSmile;
uniform float ratio;
void main(){

   FragColor = mix(texture(textWrap,TexCord),texture(textureSmile,TexCord),ratio);
}
