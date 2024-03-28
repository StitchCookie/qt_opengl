#ifndef MESH_H
#define MESH_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>
#include <string>
#include <vector>
using namespace std;
struct Vertex {
    QVector3D Position;
    QVector3D Normal;
    QVector2D TexCoords;
};
struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    /*  网格数据  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    void Draw(QOpenGLShaderProgram &shader);
    Mesh(QOpenGLFunctions_3_3_Core *glFuns,
         vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
private:
    /*  渲染数据  */
    unsigned int VAO, VBO, EBO;
    void setupMesh();
private:
    QOpenGLFunctions_3_3_Core *m_glFuns;
};


#endif // MESH_H
