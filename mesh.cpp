#include "mesh.h"

void Mesh::Draw(QOpenGLShaderProgram &shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++) {
        m_glFuns->glActiveTexture(GL_TEXTURE0 + i);  // 在绑定之前激活相应的纹理单元
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);

        // 绑定纹理单元到指定的纹理对象
        m_glFuns->glBindTexture(GL_TEXTURE_2D, textures[i].id);
        // 设置采样器统一变量的值，告知着色器从哪个激活的纹理单元进行采样。
        shader.setUniformValue(("material." + name + number).c_str(), i);
    }
    m_glFuns->glBindVertexArray(VAO);
    m_glFuns->glDrawArrays(GL_TRIANGLES, 0, 36);
    m_glFuns->glBindVertexArray(0);
}

Mesh::Mesh(QOpenGLFunctions_3_3_Core *glFuns,
           vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    :m_glFuns(glFuns)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
}

void Mesh::setupMesh()
{
    //创建VBO和VAO对象，并赋予ID
    m_glFuns->glGenVertexArrays(1, &VAO);
    m_glFuns->glGenBuffers(1, &VBO);
    m_glFuns->glGenBuffers(1,&EBO);
    //绑定VBO和VAO对象
    m_glFuns->glBindVertexArray(VAO);
    m_glFuns->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //为当前绑定到target的缓冲区对象创建一个新的数据存储。
    //如果data不是NULL，则使用来自此指针的数据初始化数据存储
    m_glFuns->glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex),
                           &vertices[0], GL_STATIC_DRAW);
    m_glFuns->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    m_glFuns->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                           indices.size() * sizeof(unsigned int),&indices[0], GL_STATIC_DRAW);
    // 顶点位置
    m_glFuns->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    m_glFuns->glEnableVertexAttribArray(0);
    // 顶点法线
    // offset宏会返回那个变量距结构体头部的字节偏移量(Byte Offset)
    m_glFuns->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                    (void*)offsetof(Vertex, Normal));

    // 顶点纹理坐标
    m_glFuns->glEnableVertexAttribArray(1);
    m_glFuns->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                    (void*)offsetof(Vertex, TexCoords));
    m_glFuns->glEnableVertexAttribArray(2);
}

