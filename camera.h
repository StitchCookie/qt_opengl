#ifndef CAMERA_H
#define CAMERA_H
#include<QMatrix4x4>

#include <vector>
#include <cmath>


// 默认值
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// 一个抽象的camera类，用于处理输入并计算相应的Euler角度、向量和矩阵，以便在OpenGL中使用
class Camera
{
public:

    // 移动方向枚举量. 是一种抽象，以避开特定于窗口系统的输入方法
    // 我们这里是WSAD
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    /*
    * 摄像机属性
    */
    // 相机位置
    QVector3D m_cameraPos;
    // 相机向前看的方向
    QVector3D m_cameraLookAtFrontDirection;
    // 相机向上看的方向
    QVector3D m_cameraUpDirection;
    // 相机向右看的方向
    QVector3D m_cameraRightDirection;

    // 定义世界下的向上的方向，用于计算m_cameraUpDirection
    QVector3D m_worldUp;
    /*
    * 欧拉角
    */
    // 绕y轴旋转的角度
    float m_yaw;
    // 绕x轴旋转的角度
    float m_pitch;
    /*
    * 相机操作
    */
    // 移动速度 用来平衡不同电脑刷新速度问题
    float m_movementSpeed;
    // 灵敏度
    float m_mouseSensitivity;
    /*
    * 缩放
    * projection矩阵中的fov角度
    *   fov(field of View) 定义了可以看到场景中多大的范围。当视野变小时，场景投影出来的空间就会减小，
    * 产生放大(Zoom In)的感觉
    */
    float m_zoom;

    Camera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f),
           QVector3D up = QVector3D(0.0f, 1.0f, 0.0f),
           float yaw = YAW, float pitch = PITCH) :
        m_cameraPos(position),
        m_worldUp(up),
        m_yaw(yaw),
        m_pitch(pitch),
        m_cameraLookAtFrontDirection(QVector3D(0.0f, 0.0f, -1.0f)),
        m_movementSpeed(SPEED),
        m_mouseSensitivity(SENSITIVITY),
        m_zoom(ZOOM)
    {
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    QMatrix4x4 GetViewMatrix()
    {
        QMatrix4x4 theMatrix;
        theMatrix.lookAt(m_cameraPos, m_cameraPos + m_cameraLookAtFrontDirection, m_cameraUpDirection);
        return theMatrix;
    }

    // 处理从任何类似键盘的输入系统接收的输入。接受摄像机定义枚举形式的输入参数（从窗口系统中提取）
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = m_movementSpeed * deltaTime;
        if (direction == FORWARD)
            m_cameraPos += m_cameraLookAtFrontDirection * velocity;
        if (direction == BACKWARD)
            m_cameraPos -= m_cameraLookAtFrontDirection * velocity;
        if (direction == LEFT)
            m_cameraPos -= m_cameraRightDirection * velocity;
        if (direction == RIGHT)
            m_cameraPos += m_cameraRightDirection * velocity;
    }

    // 处理从鼠标输入系统接收的输入。需要x和y方向上的偏移值。
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= m_mouseSensitivity;
        yoffset *= m_mouseSensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // 确保超出边界时，屏幕不会翻转
        if (constrainPitch)
        {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;
        }

        // 使用更新的Euler角度更新前、右和上矢量
        updateCameraVectors();
    }

    // 处理从鼠标滚轮事件接收的输入。仅需要在垂直车轮轴上输入
    void ProcessMouseScroll(float yoffset)
    {
        m_zoom -= (float)yoffset;
        if (m_zoom < 1.0f)
            m_zoom = 1.0f;
        if (m_zoom > 75.0f)
            m_zoom = 75.0f;
    }

private:
    // 根据相机的（更新的）Euler角度计算前矢量
    void updateCameraVectors()
    {
        // 计算新的 lookat 方向向量
        QVector3D front;
        front.setX(cos(m_yaw * M_PI / 180.0));
        front.setY(sin(m_pitch * M_PI / 180.0));
        // 不明白这里为什么是相乘的关系
        front.setZ(sin(m_yaw * M_PI / 180.0) * cos(m_pitch * M_PI / 180.0));
        front.normalize();
        m_cameraLookAtFrontDirection = front;
        // 重新计算相机的right和up方向，即重新调整相机的坐标系
        m_cameraRightDirection = QVector3D::crossProduct(m_cameraLookAtFrontDirection, m_worldUp);
        // 标准化向量，因为向上或向下看得越多，向量的长度就越接近0，这会导致移动速度变慢。
        m_cameraRightDirection.normalize();
        m_cameraUpDirection = QVector3D::crossProduct(m_cameraRightDirection, m_cameraLookAtFrontDirection);
        m_cameraUpDirection.normalize();
    }
};
#endif // CAMERA_H
