#ifndef COPENGLWIDGET_H
#define COPENGLWIDGET_H

// openGL相关库
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QElapsedTimer>
#include <QOpenGLTexture>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVector3D>

class COpenGlWidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit COpenGlWidget(QWidget *parent = nullptr);
    ~COpenGlWidget() override;
    enum Shape{None,Rect,Circle,Triangle};
    void drawShape(Shape shape);
    void setWrirFame(bool wireFrame);
//    float vertices_data[32] = {
//        // 所有的值是在[-1, 1]之间的
//        //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
//        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
//        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
//        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
//        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
//    };
    float vertices_data[180] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    QVector<QVector3D> cubePositions = {
      QVector3D (0.0f,  0.0f,  0.0f),
      QVector3D( -2.0f,  -5.0f, -15.0f),
      QVector3D(-1.5f, -2.2f, -2.5f),
      QVector3D(-3.8f, -2.0f, -12.3f),
      QVector3D( 2.4f, -0.4f, -3.5f),
      QVector3D(-1.7f,  3.0f, -7.5f),
      QVector3D( 1.3f, -2.0f, -2.5f),
      QVector3D( 1.5f,  2.0f, -2.5f),
      QVector3D( 1.5f,  0.2f, -1.5f),
      QVector3D(-1.3f,  1.0f, -1.5f)
    };
    unsigned int indices[6] = {
        0, 1, 2, // 第一个三角形
        0,2,3
    };
    // 索引缓冲对象(Element Buffer Object, EBO)：和顶点缓冲对象一样，EBO也是一个缓冲，
    // 它专门存储索引，OpenGL调用这些顶点的索引来决定该绘制哪个顶点。
    // VAO和VBO变量（无符号整型）
    unsigned int VAO_id,VBO_id, EBO_id;

public slots:
    void slots_timeout();
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
    bool eventFilter(QObject *watched, QEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event)override;
private:
    Shape m_shape;
    QOpenGLShaderProgram shaderProgramObject; // 创建相关的对象
    bool flag;
    int value = 1;
    QOpenGLTexture *textureWall;
    QOpenGLTexture *textSmile;
    QOpenGLTexture *textWrap;
    QOpenGLTexture *container;
    QElapsedTimer  timerElapsed;
    QTimer         timer;
    QVector3D camerPos;
    QVector3D camerTarget;
    QVector3D camerirection;
    QVector3D up;
    QVector3D cameraRight;
    QVector3D camerUp;

    QVector3D cameraFront;
    float fov = 45.0;
};

#endif // COPENGLWIDGET_H
