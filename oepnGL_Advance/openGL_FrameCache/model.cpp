#include "model.h"
#include <QDebug>
#include <QOpenGLTexture>

unsigned int Model::TextureFromFile(const char *path, const string &directory)
{

    string filename = string(path);
    filename = directory + '/' + filename;

    QOpenGLTexture * texture=new QOpenGLTexture(QImage(filename.c_str()).mirrored());
    if(texture==NULL) qDebug()<<"texture is NULL";
    else qDebug()<<filename.c_str()<<"loaded";
    return texture->textureId();

}

void Model::loadModel(string path)
{
    Assimp::Importer import;
    // 将模型数据读到scen'e中
    // aiProcess_Triangulate，我们告诉Assimp，如果模型不是（全部）由三角形组成，它需要将模型所有的图元形状变换为三角形。
    // aiProcess_FlipUVs将在处理的时候翻转y轴的纹理坐标（你可能还记得我们在纹理教程中说过，在OpenGL中大部分的图像的y轴都是反的，所以这个后期处理选项将会修复这个）。其它一些比较有用的选项有：

    // aiProcess_GenNormals：如果模型不包含法向量的话，就为每个顶点创建法线。
    // aiProcess_SplitLargeMeshes：将比较大的网格分割成更小的子网格，如果你的渲染有最大顶点数限制，只能渲染较小的网格，那么它会非常有用。
    // aiProcess_OptimizeMeshes：和上个选项相反，它会将多个小网格拼接为一个大的网格，减少绘制调用从而进行优化。
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    // 检查场景和其根节点不为null，并且检查了它的一个标记(Flag)，来查看返回的数据是不是不完整的。
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        qDebug() << "ERROR::ASSIMP::" << import.GetErrorString() ;
        return;
    }else
    {
        qDebug() << "SUCCESS::ASSIMP::" << import.GetErrorString() ;
    }

    // 保存当前文件的目录
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);

}

// Assimp里的结构：每个节点包含一组网格索引，每个索引指向场景对象中的特定网格
void Model::processNode(aiNode *node, const aiScene *scene)
{
    // 处理节点所有的网格（如果有的话）
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {

        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // 接下来对它的子节点重复这一过程
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {  // 每一个网格的顶点点数
        if(m_maxX<mesh->mVertices[i].x) m_maxX=mesh->mVertices[i].x;
        if(m_maxY<mesh->mVertices[i].y) m_maxY=mesh->mVertices[i].y;
        if(m_minX>mesh->mVertices[i].x) m_minX=mesh->mVertices[i].x;
        if(m_minY>mesh->mVertices[i].y) m_minY=mesh->mVertices[i].y;

        Vertex vertex;
        // 处理顶点位置、法线和纹理坐标
        QVector3D vector;
        vector.setX(mesh->mVertices[i].x);
        vector.setY(mesh->mVertices[i].y);
        vector.setZ(mesh->mVertices[i].z);
        vertex.Position = vector;

        vector.setX(mesh->mNormals[i].x);
        vector.setY(mesh->mNormals[i].y);
        vector.setZ(mesh->mNormals[i].z);
        vertex.Normal = vector;
        //Assimp允许一个模型在一个顶点上有最多8个不同的纹理坐标，
        //     我们不会用到那么多，我们只关心第一组纹理坐标。
        //        我们同样也想检查网格是否真的包含了纹理坐标（可能并不会一直如此）
        if(mesh->mTextureCoords[0]) // 有纹理坐标？
        {
            QVector2D vec;
            vec.setX(mesh->mTextureCoords[0][i].x);
            vec.setY(mesh->mTextureCoords[0][i].y);
            vertex.TexCoords = vec;
        } else
            vertex.TexCoords = QVector2D(0.0f, 0.0f);
        vertices.push_back(vertex);
    }
    // 处理索引
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // 处理材质
    //和节点一样，一个网格只包含了一个指向材质对象的索引。如果想要获取网格真正的材质，
    //    我们还需要索引场景的mMaterials数组。网格材质索引位于它的mMaterialIndex属性中，
    //    我们同样可以用它来检测一个网格是否包含有材质：
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps =
            loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps =
            loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(m_glFuns,vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{

    vector<Texture> textures;
    // 首先通过GetTextureCount函数检查储存在材质中纹理的数量
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        // 会使用GetTexture获取每个纹理的文件位置，它会将结果储存在一个aiString中
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++) {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip) {
            // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
        }
    }
    return textures;
}
