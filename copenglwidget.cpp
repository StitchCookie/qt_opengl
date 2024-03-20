#include "copenglwidget.h"
#include <QTime>
#include <QDebug>
#include <QVector4D>
#include <QMatrix4x4>
#include "cmath"

#define TIMEOUTMESC 100
//float ratio = 0.5;

QVector3D lightLocation = QVector3D(1.2f, 1.0f, 2.0f);
QVector3D viewInitPos(0.0,0.0,5.0);

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
    m_camera.m_cameraPos = viewInitPos;
}
COpenGlWidget::~COpenGlWidget()
{
    if(!isValid())
        return;
    makeCurrent();
    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&VAO);
    glDeleteVertexArrays(1,&lightVAO);
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

void COpenGlWidget::loadModel(string path)
{
    if(m_model != nullptr)
        delete m_model;
    m_model = nullptr;
    makeCurrent();
    m_model = new Model(QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>()
                        ,path.c_str());

    m_camera.m_cameraPos = cameraPosInit(m_model->m_maxY,m_model->m_minY);
    doneCurrent();
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
}
void COpenGlWidget::paintGL()
{
    if(m_model == nullptr) return;
    QMatrix4x4 view; // 观察矩阵
    QMatrix4x4 model;
    QMatrix4x4 projection;
    float time=timerElapsed.elapsed()/50.0;
    projection.perspective(m_camera.m_zoom, (float)this->width() / this->height(), 0.1f, 100.0f);
    view = m_camera.GetViewMatrix();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    switch (m_shape){
    case Rect:
        shaderProgramObject.bind();
        shaderProgramObject.setUniformValue("projection",projection);
        shaderProgramObject.setUniformValue("view",view);
        // 根据右手法则 右手握住这个向量方向,那么手指背面指向的方向就是旋转轴的正方向
        model.rotate(time,1.0,1.0,0.5);
        shaderProgramObject.setUniformValue("viewPos",m_camera.m_cameraPos);


        // 设置光源属性

        shaderProgramObject.setUniformValue("light.ambient",   QVector3D(0.4f, 0.4f, 0.4f));
        shaderProgramObject.setUniformValue("light.diffuse",   QVector3D(0.9f, 0.9f, 0.9f)); // 将光照调暗了一些以搭配场景
        shaderProgramObject.setUniformValue("light.specular",  QVector3D(1.0f, 1.0f, 1.0f));

        shaderProgramObject.setUniformValue("material.shininess", 32.0f);


        shaderProgramObject.setUniformValue("light.direction",QVector3D(-0.2f, -1.0f, -0.3f));
        shaderProgramObject.setUniformValue("model",model);
        m_model->Draw(shaderProgramObject);
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
        case Qt::Key_W: m_camera.ProcessKeyboard(Camera::FORWARD,deltaTime);break;
        case Qt::Key_S: m_camera.ProcessKeyboard(Camera::BACKWARD,deltaTime);break;
        case Qt::Key_D: m_camera.ProcessKeyboard(Camera::RIGHT,deltaTime);break;
        case Qt::Key_A: m_camera.ProcessKeyboard(Camera::LEFT,deltaTime);break;
            //case Qt::Key_Q: m_camera.ProcessKeyboard(Camera::DOWN,deltaTime);break;
            // case Qt::Key_E: m_camera.ProcessKeyboard(Camera::UP,deltaTime);break;
        case Qt::Key_Space: m_camera.m_cameraPos=viewInitPos;break;
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

void COpenGlWidget::resizeGL(int w, int h)
{
}
