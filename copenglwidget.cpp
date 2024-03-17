#include "copenglwidget.h"
#include <QTime>
#include <QDebug>
#include <QVector4D>
#include <QMatrix4x4>
#include <QtMath>
#include "cmath"

#define TIMEOUTMESC 100
float ratio = 0.5;

QVector3D lightLocation = QVector3D(1.2f,1.0f,2.0f);

COpenGlWidget::COpenGlWidget(QWidget *parent):QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    m_shape = Rect;
    flag = false;
    installEventFilter(this);
    timer.setInterval(500);
    connect(&timer,&QTimer::timeout,this,&COpenGlWidget::slots_timeout);
    timer.start();
    timerElapsed.start();
    m_camera.m_cameraPos = QVector3D(0.0f,0.0f,3.0f);
}
COpenGlWidget::~COpenGlWidget()
{
    if(!isValid())
        return;
    makeCurrent();
    glDeleteBuffers(1,&VBO_id);
    glDeleteVertexArrays(1,&VAO_id);
    doneCurrent();
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

void COpenGlWidget::slots_timeout()
{
    //    float x = static_cast<float>(sin(timerElapsed.elapsed())) * 2.0f;
    //    float y = static_cast<float>(cos(timerElapsed.elapsed())) / 2.0f;
    static float yaw = 0;
    yaw += 0.26;
    if(yaw > (3.1415926 * 2)) yaw = 0;
    //    static float length = sqrt(lightLocation.x() * lightLocation.x() + lightLocation.y() * lightLocation.y());
    //    lightLocation.setX(cos(yaw) * length);
    //    lightLocation.setY(sin(yaw) * length);
    //    lightLocation.setZ(sin(yaw) * length);
    update();
}

void COpenGlWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glGenVertexArrays(1,&VAO_id);
    glGenBuffers(1,&VBO_id);


    glBindBuffer(GL_ARRAY_BUFFER,VBO_id);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glBindVertexArray(VAO_id);
    // 开启VAO管理的第一个属性值
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 * sizeof (float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof (float),(void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof (float),(void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1,&VAO_id_Lighting);
    glBindVertexArray(VAO_id_Lighting);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_id);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof (float),(void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);


    shaderProgramObject.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shapes.vert");
    shaderProgramObject.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shapes.frag");
    shaderProgramObject.link();
    shaderProgramObject.bind();
    m_Texture = new QOpenGLTexture(QImage(":/container2.png").mirrored());
    shaderProgramObject.setUniformValue("material.diffuse",0); // 将纹理单元的索引传递给着色器
    m_TextureSpec = new QOpenGLTexture(QImage(":/container2_specular.png").mirrored());
    shaderProgramObject.setUniformValue("material.specular",1); // 将纹理单元的索引传递给着色器
    //m_TextureCode = new QOpenGLTexture(QImage(":/matrix.jpg").mirrored());
    //shaderProgramObject.setUniformValue("material.emission",2);
    shaderProgramObject.release();
    shaderProgramLighting.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shapesLighting.vert");
    shaderProgramLighting.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shapesLighting.frag");
    shaderProgramLighting.link();

    glEnable(GL_DEPTH_TEST);
}
void COpenGlWidget::paintGL()
{
    static float yaw = 0;
    yaw += 10;
    if(yaw >= 360) yaw = 0;
    QMatrix4x4 view; // 观察矩阵
    QMatrix4x4 model;
    QMatrix4x4 projection;

    projection.perspective(m_camera.m_zoom, this->width() / this->height(), 0.1f, 100.0f);
    view = m_camera.GetViewMatrix();
    //view = calculate_lookAt_matrix(m_camera.m_cameraPos,m_camera.m_cameraPos + m_camera.m_cameraLookAtFrontDirection,m_camera.m_cameraUpDirection);
    glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float cosCut = static_cast<float>(cos(qDegreesToRadians(12.5)));
    float outerCutOff = static_cast<float>(cos(qDegreesToRadians(13.0)));
    switch (m_shape){
    case Rect:
        shaderProgramObject.bind();

        shaderProgramObject.setUniformValue("viewPos",m_camera.m_cameraPos);
        shaderProgramObject.setUniformValue("material.shininess", 32.0f);

        // directional light
        shaderProgramObject.setUniformValue("dirLight.direction", QVector3D(-0.2f, -1.0f, -0.3f));
        shaderProgramObject.setUniformValue("dirLight.ambient", QVector3D(0.3f, 0.24f, 0.14f));
        shaderProgramObject.setUniformValue("dirLight.diffuse", QVector3D(0.7f, 0.42f, 0.26f));
        shaderProgramObject.setUniformValue("dirLight.specular", QVector3D(0.5f, 0.5f, 0.5f));

        // point light 1
        shaderProgramObject.setUniformValue("pointLights[0].position", pointLightPositions.at(0).x(),pointLightPositions.at(0).y(),pointLightPositions.at(0).z());
        shaderProgramObject.setUniformValue("pointLights[0].ambient",pointLightPositions.at(0).x() * 0.1,pointLightPositions.at(0).y() * 0.1, pointLightPositions.at(0).z() * 0.1);
        shaderProgramObject.setUniformValue("pointLights[0].diffuse", pointLightPositions.at(0).x(),pointLightPositions.at(0).y(),pointLightPositions.at(0).z());
        shaderProgramObject.setUniformValue("pointLights[0].specular", pointLightPositions.at(0).x(),pointLightPositions.at(0).y(),pointLightPositions.at(0).z());
        shaderProgramObject.setUniformValue("pointLights[0].constant", 1.0f);
        shaderProgramObject.setUniformValue("pointLights[0].linear", 0.09f);
        shaderProgramObject.setUniformValue("pointLights[0].quadratic", 0.032f);
        // point light 2
        shaderProgramObject.setUniformValue("pointLights[1].position", pointLightPositions.at(1).x(),pointLightPositions.at(1).y(),pointLightPositions.at(1).z());
        shaderProgramObject.setUniformValue("pointLights[1].ambient", pointLightPositions.at(1).x()* 0.1,pointLightPositions.at(1).y()* 0.1,pointLightPositions.at(1).z()* 0.1);
        shaderProgramObject.setUniformValue("pointLights[1].diffuse", pointLightPositions.at(1).x(),pointLightPositions.at(1).y(),pointLightPositions.at(1).z());
        shaderProgramObject.setUniformValue("pointLights[1].specular",pointLightPositions.at(1).x(),pointLightPositions.at(1).y(),pointLightPositions.at(1).z());
        shaderProgramObject.setUniformValue("pointLights[1].constant", 1.0f);
        shaderProgramObject.setUniformValue("pointLights[1].linear", 0.09f);
        shaderProgramObject.setUniformValue("pointLights[1].quadratic", 0.032f);
        // point light 3
        shaderProgramObject.setUniformValue("pointLights[2].position", pointLightPositions.at(2).x(),pointLightPositions.at(2).y(),pointLightPositions.at(2).z());
        shaderProgramObject.setUniformValue("pointLights[2].ambient",pointLightPositions.at(2).x()* 0.1,pointLightPositions.at(2).y()* 0.1,pointLightPositions.at(2).z()* 0.1);
        shaderProgramObject.setUniformValue("pointLights[2].diffuse", pointLightPositions.at(2).x(),pointLightPositions.at(2).y(),pointLightPositions.at(2).z());
        shaderProgramObject.setUniformValue("pointLights[2].specular",pointLightPositions.at(2).x(),pointLightPositions.at(2).y(),pointLightPositions.at(2).z());
        shaderProgramObject.setUniformValue("pointLights[2].constant", 1.0f);
        shaderProgramObject.setUniformValue("pointLights[2].linear", 0.09f);
        shaderProgramObject.setUniformValue("pointLights[2].quadratic", 0.032f);
        // point light 4
        shaderProgramObject.setUniformValue("pointLights[3].position", pointLightPositions.at(3).x(),pointLightPositions.at(3).y(),pointLightPositions.at(3).z());
        shaderProgramObject.setUniformValue("pointLights[3].ambient",pointLightPositions.at(3).x()* 0.1,pointLightPositions.at(3).y()* 0.1,pointLightPositions.at(3).z()* 0.1);
        shaderProgramObject.setUniformValue("pointLights[3].diffuse", pointLightPositions.at(3).x(),pointLightPositions.at(3).y(),pointLightPositions.at(3).z());
        shaderProgramObject.setUniformValue("pointLights[3].specular", pointLightPositions.at(3).x(),pointLightPositions.at(3).y(),pointLightPositions.at(3).z());
        shaderProgramObject.setUniformValue("pointLights[3].constant", 1.0f);
        shaderProgramObject.setUniformValue("pointLights[3].linear", 0.09f);
        shaderProgramObject.setUniformValue("pointLights[3].quadratic", 0.032f);
        // spotLight
        shaderProgramObject.setUniformValue("spotLight.position", m_camera.m_cameraPos);
        shaderProgramObject.setUniformValue("spotLight.direction", m_camera.m_cameraLookAtFrontDirection);
        shaderProgramObject.setUniformValue("spotLight.ambient", QVector3D(0.0f, 0.0f, 0.0f));
        shaderProgramObject.setUniformValue("spotLight.diffuse", QVector3D(0.8f, 0.8f, 0.0f));
        shaderProgramObject.setUniformValue("spotLight.specular", QVector3D(0.8f, 0.8f, 0.0f));
        shaderProgramObject.setUniformValue("spotLight.constant", 1.0f);
        shaderProgramObject.setUniformValue("spotLight.linear", 0.09f);
        shaderProgramObject.setUniformValue("spotLight.quadratic", 0.032f);
        shaderProgramObject.setUniformValue("spotLight.cutOff", cosCut);
        shaderProgramObject.setUniformValue("spotLight.outerCutOff", outerCutOff);

        shaderProgramObject.setUniformValue("projection",projection);
        shaderProgramObject.setUniformValue("view",view);
        model.setToIdentity();
        // 根据右手法则 右手握住这个向量方向,那么手指背面指向的方向就是旋转轴的正方向
        // model.rotate(25,-1.0,1.0,-1.0);

        m_Texture->bind(0);
        m_TextureSpec->bind(1);
        //m_TextureCode->bind(2);
        glBindVertexArray(VAO_id);

        for(int i = 0; i < cubePositions.size(); ++i)
        {
            model.setToIdentity();
            model.translate(cubePositions[i]);
            model.rotate(20.0f * i,1.0f,0.3f,0.5f);
            shaderProgramObject.setUniformValue("model",model);
            glDrawArrays(GL_TRIANGLES,0,36);
        }

        shaderProgramObject.release();
        shaderProgramLighting.bind();

        shaderProgramLighting.setUniformValue("projection",projection);
        shaderProgramLighting.setUniformValue("view",view);
        model.setToIdentity();


        glBindVertexArray(VAO_id_Lighting);
        for(int i = 0; i < 4; ++i)
        {
            model.translate(pointLightPositions[i]);
            model.scale(0.2f,0.2f,0.2f);
            shaderProgramLighting.setUniformValue("model",model);
            glDrawArrays(GL_TRIANGLES,0,36);
        }


        shaderProgramLighting.release();
        glBindVertexArray(0);
    default:
        break;
    }
}

bool COpenGlWidget::eventFilter(QObject *watched, QEvent *event)
{
    //float cameaSpeed = (2.5 * TIMEOUTMESC) / 1000.0;
    float deltaTime = TIMEOUTMESC / 1000.0;
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
        case Qt::Key_Up:
            ratio += 0.1;
            break;
        case Qt::Key_Down:
            ratio -= 0.1;
            break;
        case Qt::Key_W:
            m_camera.ProcessKeyboard(Camera::FORWARD,deltaTime);
            break;
        case Qt::Key_A:
            m_camera.ProcessKeyboard(Camera::LEFT,deltaTime);
            break;
        case Qt::Key_S:
            m_camera.ProcessKeyboard(Camera::BACKWARD,deltaTime);
            break;
        case Qt::Key_D:
            m_camera.ProcessKeyboard(Camera::RIGHT,deltaTime);
            break;
        default:
            break;
        }
        if(ratio > 1) ratio = 1;
        if(ratio < 0) ratio = 0;
        shaderProgramObject.bind();
        shaderProgramObject.setUniformValue("ratio",ratio);
        update();
    }
    return  QWidget::eventFilter(watched,event);
}
QPoint deltaPos;
void COpenGlWidget::mouseMoveEvent(QMouseEvent *ev)
{
    static QPoint lastPos(width()/2,height()/2);
    auto currentPos = ev->pos();
    deltaPos = currentPos - lastPos;
    lastPos = currentPos;
    m_camera.ProcessMouseMovement(deltaPos.x(),-deltaPos.y());
    update();
}

void COpenGlWidget::wheelEvent(QWheelEvent *event)
{
    m_camera.ProcessMouseScroll(event->angleDelta().y() / 120);
    update();
}

// 封装数据
void COpenGlWidget::pri_packGLSLdata()
{
    // [0] 平行光源数据

    // [0] 平行光源数据
}
void COpenGlWidget::resizeGL(int w, int h)
{
}
