#ifndef COPENGLWIDGET_H
#define COPENGLWIDGET_H

// openGL相关库
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QTimer>

// QOpenGLWidget 提供了三个便捷的虚函数，可以重载，用来重新实现典型的OpenGL任务
// QOpenGLFunctions_3_3_Core 提供OpenGL X.X版本核心模式的所有功能，是对OpenGL函数的封装。
// initializeOpenGLFunctions用于初始化OpenGL函数，将Qt里的函数指针指向显卡的函数，之后调用的OpenGL函数才是可用的。
class COpenGlWidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit COpenGlWidget(QWidget *parent = nullptr);
    ~COpenGlWidget() override;
    enum Shape{None,Rect,Circle,Triangle};
    void drawShape(Shape shape);
    void setWrirFame(bool wireFrame);
//    float vertices_data[12] = {
//        // 所有的值是在[-1, 1]之间的
//        0.25f,  0.0f,  0.0f,
//        0.0f,  0.5f,  0.0f,
//        -0.25f, -0.0f,  0.0f
//    };

    float vertices_data[24] = {
        // 所有的值是在[-1, 1]之间的
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };
    unsigned int indices[3] = {
        0, 1, 2, // 第一个三角形
    };
    // 索引缓冲对象(Element Buffer Object, EBO)：和顶点缓冲对象一样，EBO也是一个缓冲，
    // 它专门存储索引，OpenGL调用这些顶点的索引来决定该绘制哪个顶点。
    // VAO和VBO变量（无符号整型）
    unsigned int VAO_id,VBO_id, EBO_id;
    // 着色器变量
    unsigned int shaderProgram1_id;
public slots:
    void slots_timeout();
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
private:
    Shape m_shape;
    QOpenGLShaderProgram shaderProgramObject; // 创建相关的对象
    QTimer timer;
    bool flag;
    int value = 1;
};

#endif // COPENGLWIDGET_H
