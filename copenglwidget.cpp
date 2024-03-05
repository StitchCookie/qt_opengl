#include "copenglwidget.h"
#include <QTime>
#include <QDebug>
#include <QVector4D>
#include "cmath"

COpenGlWidget::COpenGlWidget(QWidget *parent):QOpenGLWidget(parent)
{
    m_shape = Rect;
    timer.setInterval(100);
    flag = false;
    connect(&timer,&QTimer::timeout,this,&COpenGlWidget::slots_timeout);
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
    // timer.start();
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
    makeCurrent();
    // 拿出秒的值
    int timeValue = QTime::currentTime().second();
    float greenValue = (sin(timeValue)/2.0f) + 0.5f;
    shaderProgramObject.bind();
    QVector4D colorVec(0.0f, greenValue, 0.0f, 1.0f);
    shaderProgramObject.setUniformValue("ourColor",colorVec);
    shaderProgramObject.release();
    update();
    doneCurrent();

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
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 * sizeof (float),(void*)0);
    glEnableVertexAttribArray(0);
        // 开启VAO管理的第二个属性值
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
        // 开启VAO管理的第三个属性值
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    shaderProgramObject.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shapes.vert");
    shaderProgramObject.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shapes.frag");
    shaderProgramObject.link();

    textureWall = new QOpenGLTexture(QImage(":/wall.jpg"));
    // 一个好的习惯 不管他要不要记录属性,不要让一些准备工作还未结束时,就解绑
}
void COpenGlWidget::paintGL()
{
    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shaderProgramObject.bind();
    glBindVertexArray(VAO_id);
    switch (m_shape){
    case Rect:
        // shaderProgramObject.setUniformValue("xOffset",0.5f);
        textureWall->bind(0);
        glDrawElements(GL_TRIANGLES, 6,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);
        shaderProgramObject.release();
        // 否则，不进行绘制
    default:
        break;
    }
}
void COpenGlWidget::resizeGL(int w, int h)
{
}
