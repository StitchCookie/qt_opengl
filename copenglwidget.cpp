#include "copenglwidget.h"
#include <QTime>
#include <QDebug>
#include <QVector4D>
#include <QMatrix4x4>
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
    timer.setInterval(1000);
    connect(&timer,&QTimer::timeout,this,&COpenGlWidget::slots_timeout);
   // timer.start();
   // timerElapsed.start();
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
    static float length = sqrt(lightLocation.x() * lightLocation.x() + lightLocation.y() * lightLocation.y());
    lightLocation.setX(cos(yaw) * length);
    lightLocation.setY(sin(yaw) * length);
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
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof (float),(void*)0);
    glEnableVertexAttribArray(0);
#ifdef ManFanShe
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof (float),(void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
#endif

    glGenVertexArrays(1,&VAO_id_Lighting);
    glBindVertexArray(VAO_id_Lighting);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_id);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof (float),(void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    shaderProgramObject.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shapes.vert");
    shaderProgramObject.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shapes.frag");
    shaderProgramObject.link();


    shaderProgramLighting.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shapes.vert");
    shaderProgramLighting.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shapesLighting.frag");
    shaderProgramLighting.link();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);  // 设置透明度
    glEnable(GL_DEPTH_TEST);
}
void COpenGlWidget::paintGL()
{
    QMatrix4x4 view; // 观察矩阵
    QMatrix4x4 model;

    QMatrix4x4 projection;

    projection.perspective(m_camera.m_zoom, this->width() / this->height(), 0.1f, 100.0f);

    view = m_camera.GetViewMatrix();
    // view = calculate_lookAt_matrix(m_camera.m_cameraPos,m_camera.m_cameraPos + m_camera.m_cameraLookAtFrontDirection,m_camera.m_cameraUpDirection);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (m_shape){
    case Rect:
        shaderProgramObject.bind();
        shaderProgramObject.setUniformValue("objectColor",QVector3D(1.0f,0.5f,0.31f));
        shaderProgramObject.setUniformValue("lightColor",QVector3D(1.0f,1.0f,1.0f));
        shaderProgramObject.setUniformValue("lightPos",lightLocation);
        shaderProgramObject.setUniformValue("projection",projection);
        shaderProgramObject.setUniformValue("viewPos",m_camera.m_cameraPos);
        shaderProgramObject.setUniformValue("view",view);
        model.setToIdentity();
        //model.rotate(30,0,0);
        shaderProgramObject.setUniformValue("model",model);
        glBindVertexArray(VAO_id);
        glDrawArrays(GL_TRIANGLES,0,36);

        shaderProgramLighting.bind();
        model.setToIdentity();
        model.translate(lightLocation);
        model.scale(0.2f);
        shaderProgramLighting.setUniformValue("projection",projection);
        shaderProgramLighting.setUniformValue("view",view);
        shaderProgramLighting.setUniformValue("model",model);
        glBindVertexArray(VAO_id_Lighting);

        glDrawArrays(GL_TRIANGLES,0,36);
        glBindVertexArray(0);
        shaderProgramLighting.release();
        // 否则，不进行绘制
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
void COpenGlWidget::resizeGL(int w, int h)
{
}
