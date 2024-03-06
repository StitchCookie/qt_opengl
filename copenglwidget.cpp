#include "copenglwidget.h"
#include <QTime>
#include <QDebug>
#include <QVector4D>
#include "cmath"
float ratio = 0.5;

COpenGlWidget::COpenGlWidget(QWidget *parent):QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    m_shape = Rect;
    flag = false;
    installEventFilter(this);
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);  // 设置透明度

    textureWall = new QOpenGLTexture(QImage(":/wall.jpg").mirrored());
    textSmile = new QOpenGLTexture(QImage(":/awesomeface.png").mirrored());
    shaderProgramObject.bind();
    shaderProgramObject.setUniformValue("textureWall",0);
    shaderProgramObject.setUniformValue("textureSmile",1);


    // 纹理环绕
    textWrap = new QOpenGLTexture(QImage(":/wrap.png").mirrored());
    // shaderProgramObject.setUniformValue("textWrap",2);  // 设置GPU端数据

    // 纹理环绕实现
    //  textSmile->bind(1);
    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 多级渐远纹理
    // textWrap->generateMipMaps();
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    container = new QOpenGLTexture(QImage(":/container.jpg").mirrored());
    shaderProgramObject.setUniformValue("textWrap",2);

}
void COpenGlWidget::paintGL()
{
    float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f }; // OpenGl 是C语言的 这个不能放到下面的switch里面

    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shaderProgramObject.bind();
    shaderProgramObject.setUniformValue("ratio",ratio);
    glBindVertexArray(VAO_id);
    switch (m_shape){
    case Rect:
        //glActiveTexture(GL_TEXTURE0);
        // textureWall->bind(0);
        // glActiveTexture(GL_TEXTURE1);
        textSmile->bind(1);

        container->bind(2); // 设置cpu端

        //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);  // 垂直方向镜像重复

        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);  // GL_CLAMP_TO_BORDER 超出的坐标为用户指定的边缘颜色。
        // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glDrawElements(GL_TRIANGLES, 6,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);
        shaderProgramObject.release();
        // 否则，不进行绘制
    default:
        break;
    }
}

bool COpenGlWidget::eventFilter(QObject *watched, QEvent *event)
{
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
void COpenGlWidget::resizeGL(int w, int h)
{
}
