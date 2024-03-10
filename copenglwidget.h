#ifndef COPENGLWIDGET_H
#define COPENGLWIDGET_H

// openGL相关库
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QOpenGLTexture>
#include <QKeyEvent>

class COpenGlWidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit COpenGlWidget(QWidget *parent = nullptr);
    ~COpenGlWidget() override;
    enum Shape{None,Rect,Circle,Triangle};
    void drawShape(Shape shape);
    void setWrirFame(bool wireFrame);
    float vertices_data[32] = {
        // 所有的值是在[-1, 1]之间的
        //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
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

private:
    Shape m_shape;
    QOpenGLShaderProgram shaderProgramObject; // 创建相关的对象
    bool flag;
    int value = 1;
    QOpenGLTexture *textureWall;
    QOpenGLTexture *textSmile;
    QOpenGLTexture *textWrap;
    QOpenGLTexture *container;
    QTimer  m_timer;
};

#endif // COPENGLWIDGET_H
