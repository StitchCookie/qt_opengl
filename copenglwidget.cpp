#include "copenglwidget.h"
#include <QTime>
#include <QDebug>
#include <QVector4D>
#include <QMatrix4x4>
#include "cmath"

#define TIMEOUTMESC 100
float ratio = 0.5;
float PI = 3.1415926;


COpenGlWidget::COpenGlWidget(QWidget *parent):QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    m_shape = Rect;
    flag = false;
    installEventFilter(this);
    timer.setInterval(100);
    connect(&timer,&QTimer::timeout,this,&COpenGlWidget::slots_timeout);
    // timer.start();


    camerPos = QVector3D(0.0f,0.0f,3.0f);
    camerTarget = QVector3D(0.0f,0.0f,0.0f);
    camerirection = QVector3D(camerPos - camerTarget);
    camerirection.normalize();
    up = QVector3D(0.0f,1.0f,0.0f);
    cameraRight = QVector3D::crossProduct(up,camerirection);
    cameraRight.normalize();
    camerUp = QVector3D::crossProduct(camerirection,cameraRight);
    //camerUp

    cameraFront = QVector3D(0.0f,0.0f,-1.0f);
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
void COpenGlWidget::slots_timeout()
{

    if(m_shape == None)
        return;
    update();
}
void COpenGlWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glGenVertexArrays(1,&VAO_id);
    glGenBuffers(1,&VBO_id);
    glGenBuffers(1,&EBO_id);
    glBindVertexArray(VAO_id);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_id);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices_data),vertices_data,GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    // 开启VAO管理的第一个属性值
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5 * sizeof (float),(void*)0);
    glEnableVertexAttribArray(0);
    // 开启VAO管理的第二个属性值
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    shaderProgramObject.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shapes.vert");
    shaderProgramObject.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shapes.frag");
    shaderProgramObject.link();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);  // 设置透明度
    glEnable(GL_DEPTH_TEST);
    textureWall = new QOpenGLTexture(QImage(":/wall.jpg").mirrored());
    textSmile = new QOpenGLTexture(QImage(":/awesomeface.png").mirrored());
    shaderProgramObject.bind();
    shaderProgramObject.setUniformValue("textureWall",0);
    shaderProgramObject.setUniformValue("textureSmile",1);


    // 纹理环绕
    textWrap = new QOpenGLTexture(QImage(":/wrap.png").mirrored());
    container = new QOpenGLTexture(QImage(":/container.jpg").mirrored());
    shaderProgramObject.setUniformValue("textWrap",2);

}
void COpenGlWidget::paintGL()
{
    glViewport(0,0,width(),height());
    QMatrix4x4 view; // 观察矩阵
    // const float radius = 10.0f;
    float time = timerElapsed.elapsed() / 1000.0;
    // float camX = sin(time) * radius;
    // float camZ = cos(time) * radius;
    //view.lookAt(QVector3D(camX,0.0,camZ),camerTarget,up);
    QMatrix4x4 projection;
    projection.perspective(fov, this->width() / this->height(), 0.1f, 100.0f);

    view.lookAt(camerPos,camerPos + cameraFront,up);

    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgramObject.bind();
    shaderProgramObject.setUniformValue("ratio",ratio);
    glBindVertexArray(VAO_id);
    switch (m_shape){
    case Rect:

        textSmile->bind(1);
        container->bind(2); // 设置cpu端
        shaderProgramObject.setUniformValue("view",view);
        shaderProgramObject.setUniformValue("projection",projection);

        for(unsigned int i = 0; i < 10; i++) {
            QMatrix4x4 model1;
            model1.translate(cubePositions[i]);
            //if(i%3 == 0)
            //  model1.rotate(QTime::currentTime().second() * (i  + 1) ,1.0f,0.3f,0.5f);
            shaderProgramObject.setUniformValue("model",model1);
            glDrawArrays(GL_TRIANGLES,0,36);
        }
        glBindVertexArray(0);
        shaderProgramObject.release();
        // 否则，不进行绘制
    default:
        break;
    }
}

bool COpenGlWidget::eventFilter(QObject *watched, QEvent *event)
{
    float cameaSpeed = (2.5 * TIMEOUTMESC) / 1000.0;
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
            camerPos += cameaSpeed * cameraFront;
            break;
        case Qt::Key_A:
            camerPos -= cameaSpeed * cameraRight;
            break;
        case Qt::Key_S:
            camerPos -= cameaSpeed * cameraFront;
            break;
        case Qt::Key_D:
            camerPos += cameaSpeed * cameraRight;

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

void COpenGlWidget::mouseMoveEvent(QMouseEvent *event)
{


    float xpos = event->pos().x();
    float ypos = event->pos().y();


    static float yaw = -90.0f;
    static float pitch = 0.0f;
    static QPoint lastPos(this->width()/2,this->height()/2);
    float xoffset = xpos - lastPos.x();
    float yoffset = lastPos.y() - ypos;
    lastPos.setX(xpos);
    lastPos.setY(ypos);

    float sensitivity = 0.0011f;  // 改变灵敏度

    xoffset   *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch -= yoffset;
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;
    QVector3D front;
    front.setX(cos(yaw)*cos(pitch));
    front.setY(sin(pitch));
    front.setZ(sin(yaw)*cos(pitch));
    front.normalize();
    //camerPos = front;
    cameraFront = front;
    // qDebug()<<lastPos;
    // QPoint currentPos = event->pos();

    // QPoint deltaPos(currentPos.x() - lastPos.x(),lastPos.y() - currentPos.y());
    // float sensitivity = 0.05f;  // 改变灵敏度
    // deltaPos *= sensitivity;
    // yaw   += deltaPos.x();
    // pitch += deltaPos.y();
    // if(pitch > 89.0f) pitch = 89.0f;
    // if(pitch < -89.0f) pitch = -89.0f;

    // cameraFront.setX(cos(yaw*PI/180)*cos(pitch*PI/180));
    // cameraFront.setY(sin(pitch*PI/180));
    // cameraFront.setZ(sin(yaw*PI*180)*cos(pitch*PI / 180));
    // cameraFront.normalize();
    update();
}

void COpenGlWidget::wheelEvent(QWheelEvent *event)
{
    if(fov >= 1.0f && fov <= 75.0f)
        fov -= event->angleDelta().y() / 120;
    if(fov <= 1.0f) fov = 1.0f;
    if(fov >= 75.0f) fov = 75.0f;
    update();
}
void COpenGlWidget::resizeGL(int w, int h)
{
}
