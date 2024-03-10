#version 330 core
in  vec4 vertexColor;
out vec4 FragColor;
in vec2 TexCord;
uniform sampler2D textWrap; // 纹理采样器
uniform sampler2D textureSmile;
uniform float ratio;
// 纹理采样器// uniform sampler2D textWrap;
void main(){
   // FragColor = mix(texture(textureWall,TexCord),texture(textureSmile,TexCord),0.5);
  //FragColor = texture(textureSmile,TexCord);
  //FragColor = mix(texture(textWrap,TexCord),texture(textureSmile,TexCord),ratio);
   FragColor = mix(texture(textWrap,TexCord),texture(textureSmile,TexCord),ratio)* vec4(TexCord,1.0f,1.0f);
}
