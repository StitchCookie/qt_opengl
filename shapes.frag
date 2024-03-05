#version 330 core
in  vec4 vertexColor;
out vec4 FragColor;
in vec2 TexCord;
uniform sampler2D texture0;
uniform sampler2D texture1;
void main(){
     FragColor = texture(texture1,TexCord);
}
