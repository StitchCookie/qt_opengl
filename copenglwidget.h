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
#include "camera.h"
#include <QOpenGLTexture>

#include "model.h"

class COpenGlWidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit COpenGlWidget(QWidget *parent = nullptr);
    ~COpenGlWidget() override;
    enum Shape{None,Rect,Circle,Triangle};
    void drawShape(Shape shape);
    void setWrirFame(bool wireFrame);
    QMatrix4x4 calculate_lookAt_matrix(QVector3D position,QVector3D target,QVector3D worldUp);
    void loadModel(string path);

protected:
    /**
     * @brief slots_timeout 让光照在场景中移动
     */
    void slots_timeout();

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
    bool eventFilter(QObject *watched, QEvent *event)override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void wheelEvent(QWheelEvent *event)override;
private:
    Shape m_shape;
    QOpenGLShaderProgram shaderProgramObject; // 创建相关的对象
    bool flag;
    unsigned int VAO,VBO,lightVAO;
    QElapsedTimer  timerElapsed;
    QTimer         timer;
    Camera          m_camera;
    Model            *m_model = nullptr;

    QVector3D     cameraPosInit(float maxY,float minY);
};

#endif // COPENGLWIDGET_H
