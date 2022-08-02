#include "mybutton.h"
#include "QtGui/qpainter.h"
#include <QMouseEvent>

MyButton::MyButton(QWidget *parent)
    : QPushButton(parent)
{

}

void MyButton::setImage(QString normal, QString hover, QString pressed)
{
    m_normal = normal;
    m_hover = hover;
    m_pressed = pressed;
    m_pixmap.load(m_normal);
    update();//??重新绘制
}

void MyButton::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        m_pixmap.load(m_pressed);
        update();
    }
    QPushButton::mousePressEvent(ev);//重新调用父类，发射信号后，才能触发事件进行更新

}

void MyButton::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        m_pixmap.load(m_normal);
        update();
    }
    QPushButton::mousePressEvent(ev);
}

void MyButton::enterEvent(QEvent *ev)
{
    m_pixmap.load(m_hover); //先切换
    update();  //再绘制
}

void MyButton::leaveEvent(QEvent *ev)
{
    m_pixmap.load(m_normal);
    update();
}

void MyButton::paintEvent(QPaintEvent *ev) //？？重新绘制
{
    QPainter p(this);
    p.drawPixmap(rect(), m_pixmap);
}
