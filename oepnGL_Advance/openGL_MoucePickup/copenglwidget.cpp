#include "copenglwidget.h"
#include <QTime>
#include <QDebug>
#include <QVector4D>
#include <QMatrix4x4>
#include "cmath"
#include "vertices.h"

#define TIMEOUTMESC 100
//float ratio = 0.5;
float _near = 0.1f;
float _far = 100.0f;
QMatrix4x4 view; // 观察矩阵
QMatrix4x4 model;
QMatrix4x4 projection;
QVector3D lightLocation = QVector3D(1.2f, 1.0f, 2.0f);
QVector3D viewInitPos(0.0,5.0,20.0);

COpenGlWidget::COpenGlWidget(QWidget *parent):QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    m_shape = Rect;
    flag = false;
    installEventFilter(this);
    connect(&timer,&QTimer::timeout,this,&COpenGlWidget::slots_timeout);
    timer.start(50);
    timerElapsed.start();
    m_camera.Position = viewInitPos;
}
COpenGlWidget::~COpenGlWidget()
{
    for(auto item : m_Models)
    {
        delete item.mode;
        item.mode = nullptr;
    }
}
void COpenGlWidget::drawShape(COpenGlWidget::Shape shape)
{
    m_shape = shape;
    update();
}
void COpenGlWidget::setWrirFame(bool wireFrame)
{
    flag = !flag;
    makeCurrent();
    if(flag)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    update();
    doneCurrent();
}

QMatrix4x4 COpenGlWidget::calculate_lookAt_matrix(QVector3D position, QVector3D target, QVector3D worldUp)
{
    // 首先我们应该计算摄像机的方向向量 也就是摄像机的z轴
    QVector3D zaxis = QVector3D(position - target);
    zaxis.normalize();  // 方向向量
    // 通过方向向量和上向量计算得出 右向量 也就是x向量
    QVector3D xaxis = QVector3D::crossProduct(worldUp,zaxis);
    xaxis.normalize();
    // 计算出上向量
    QVector3D yaxis = QVector3D::crossProduct(zaxis,xaxis);
    yaxis.normalize();
    //
    QMatrix4x4 translation;
    translation.setToIdentity();
    translation.translate(-position.x(),-position.y(),-position.z());
    QMatrix4x4 rotation;
    rotation.setToIdentity();
    rotation(0, 0) = xaxis.x();
    rotation(1, 0) = xaxis.y();
    rotation(2, 0) = xaxis.z();
    rotation(0, 1) = yaxis.x();
    rotation(1, 1) = yaxis.y();
    rotation(2, 1) = yaxis.z();
    rotation(0, 2) = zaxis.x();
    rotation(1, 2) = zaxis.y();
    rotation(2, 2) = zaxis.z();

    // 为了构成完整的4x4矩阵，第四行（最后一行）需要设置为(0, 0, 0, 1)以保证齐次坐标系下的正确性
    rotation(0, 3) = 0.0f;
    rotation(1, 3) = 0.0f;
    rotation(2, 3) = 0.0f;
    rotation(3, 3) = 1.0f;
    return  rotation * translation ;
}

void COpenGlWidget::loadModel(string path)
{
    static int i = 1;
    makeCurrent();
    Model  *m_model = new Model(QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>()
                               ,path.c_str());
    m_Models["模型"+QString::number(i++)] = ModelInfo{m_model,QVector3D(0,0,0),0.0,0.0,0.0,false,"模型"};
    m_camera.Position = cameraPosInit(m_model->m_maxY,m_model->m_minY);
    doneCurrent();
}

QVector4D COpenGlWidget::API_worldPosToViewPos(int xPos, int yPos)
{
    float winZ;
    glReadPixels(xPos,
                 this->height() - yPos
                 ,1,1,
                 GL_DEPTH_COMPONENT,GL_FLOAT,&winZ);

    float x = (2.0 *  xPos) / this->width() - 1.0f;
    float y = 1.0f - (2.0f * yPos) / this->height();
    float z = winZ * 2.0 - 1.0f;
    float w = (2.0 * _near * _far) / (_far + _near - z * (_far - _near));
    //float w= _near*_far/(_near*winZ-_far*winZ+_far);
    QVector4D wolrdPostion(x,y,z,1);
    wolrdPostion = w * wolrdPostion;
    QVector4D world =  view.inverted()*projection.inverted()*wolrdPostion;
    return world;
}


void COpenGlWidget::slots_timeout()
{
    update();
}

void COpenGlWidget::initializeGL()
{
    initializeOpenGLFunctions();
    shaderProgramObject.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shapes.vert");
    shaderProgramObject.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shapes.frag");
    shaderProgramObject.link();
    if(!shaderProgramObject.isLinked()) qDebug()<<"ERR:"<<shaderProgramObject.log();
    m_planeTex = new QOpenGLTexture(QImage(":/matrix.jpg").mirrored());
    m_planeMesh = processMesh(planeVertices,6,m_planeTex->textureId());


}


void COpenGlWidget::paintGL()
{
    view.setToIdentity();
    model.setToIdentity();
    projection.setToIdentity();
    float time=timerElapsed.elapsed()/50.0;
    projection.perspective(m_camera.Zoom, (float)this->width() / this->height(), 0.1f, 100.0f);
    view = m_camera.GetViewMatrix();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_ALWAYS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    switch (m_shape){
    case Rect:
        shaderProgramObject.bind();
        shaderProgramObject.setUniformValue("projection",projection);
        shaderProgramObject.setUniformValue("view",view);
        // 根据右手法则 右手握住这个向量方向,那么手指背面指向的方向就是旋转轴的正方向
        model.setToIdentity();


        // 设置光源属性

        shaderProgramObject.setUniformValue("light.ambient",   QVector3D(0.8f, 0.8f, 0.8f));
        shaderProgramObject.setUniformValue("light.diffuse",   QVector3D(0.9f, 0.9f, 0.9f));
        shaderProgramObject.setUniformValue("light.specular",  QVector3D(1.0f, 1.0f, 1.0f));

        shaderProgramObject.setUniformValue("material.shininess", 32.0f);


        shaderProgramObject.setUniformValue("light.direction",QVector3D(-0.2f, -1.0f, -0.3f));
        shaderProgramObject.setUniformValue("model",model);
        m_planeMesh->Draw(shaderProgramObject);

        if(m_Models.size() <= 0) return;
        foreach (auto item,m_Models) {
            model.setToIdentity();
            model.translate(item.worldPos);

            model.rotate(item.pitch,QVector3D(1.0,0.0,0.0));
            model.rotate(item.yaw,QVector3D(0.0,1.0,0.0));
            model.rotate(item.roll,QVector3D(0.0,0.0,1.0));
            shaderProgramObject.setUniformValue("model",model);
            item.mode->Draw(shaderProgramObject);
        }
    default:
        break;
    }
}

bool COpenGlWidget::eventFilter(QObject *watched, QEvent *event)
{
    float deltaTime = TIMEOUTMESC / 1000.0;
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
        case Qt::Key_W: m_camera.ProcessKeyboard(FORWARD,deltaTime);break;
        case Qt::Key_S: m_camera.ProcessKeyboard(BACKWARD,deltaTime);break;
        case Qt::Key_D: m_camera.ProcessKeyboard(RIGHT,deltaTime);break;
        case Qt::Key_A: m_camera.ProcessKeyboard(LEFT,deltaTime);break;
        case Qt::Key_Q: m_camera.ProcessKeyboard(DOWN,deltaTime);break;
        case Qt::Key_E: m_camera.ProcessKeyboard(UP,deltaTime);break;
        case Qt::Key_Space: m_camera.Position=viewInitPos;break;
        default:
            break;
        }
        //  if(ratio > 1) ratio = 1;
        //  if(ratio < 0) ratio = 0;
        shaderProgramObject.bind();
        //  shaderProgramObject.setUniformValue("ratio",ratio);
        update();
    }
    return  QWidget::eventFilter(watched,event);
}
void COpenGlWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton){
        static QPoint lastPos(width()/2,height()/2);
        auto currentPos=event->pos();
        QPoint deltaPos=currentPos-lastPos;
        lastPos=currentPos;

        m_camera.ProcessMouseMovement(deltaPos.x(),-deltaPos.y());
        update();
    }

}

void COpenGlWidget::wheelEvent(QWheelEvent *event)
{
    m_camera.ProcessMouseScroll(event->angleDelta().y() / 120);
    update();
}


void COpenGlWidget::mousePressEvent(QMouseEvent *event)
{
    makeCurrent();
    if(event->buttons() & Qt::LeftButton)
    {
        QVector4D worldPos = API_worldPosToViewPos(event->pos().x(),event->pos().y());
        emit signal_mousePickingPos(worldPos);
    }
    doneCurrent();
}


QVector3D COpenGlWidget::cameraPosInit(float maxY,float minY)
{
    QVector3D temp = {0,0,0};
    float height = maxY - minY;
    temp.setZ(float(1.5*height));
    if(minY >= 0)
        temp.setY(float(height/ 2.0));
    viewInitPos = temp;
    return temp;
}

Mesh* COpenGlWidget::processMesh(float *vertices, int size, unsigned int texture)
{
    vector<Vertex> _vertices;
    vector<unsigned int> _indices;
    vector<Texture> _textures;
    for(int i=0;i<size;i++){
        Vertex vert;
        vert.Position[0] = vertices[i*5+0];
        vert.Position[1] = vertices[i*5+1];
        vert.Position[2] = vertices[i*5+2];
        vert.TexCoords[0]=vertices[i*5+3];
        vert.TexCoords[1]=vertices[i*5+4];
        _vertices.push_back(vert);
        _indices.push_back(i); // 索引
    }

    Texture tex;
    tex.id=texture;
    tex.type="texture_diffuse";
    _textures.push_back(tex);
    return new Mesh(QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>()
                    ,_vertices,_indices,_textures);

}

void COpenGlWidget::resizeGL(int w, int h)
{
}
