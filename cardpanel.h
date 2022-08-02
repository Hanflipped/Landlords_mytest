#ifndef CARDPANEL_H
#define CARDPANEL_H

#include <QWidget>
#include "card.h"
#include "player.h"

class CardPanel : public QWidget
{
    Q_OBJECT
public:
    explicit CardPanel(QWidget *parent = nullptr);

    //设置获取图片函数
    void setImage(const QPixmap &front, const QPixmap &back);
    QPixmap getImage();

    //扑克牌显示那一面
    void setFrontSide(bool flag);
    bool isFrontSide();

    //记录窗口是否被选中了
    void setSeclected(bool flag);
    bool isSelected();

    //扑克牌的花色以及点数
    void setCard(const Card& card);
    Card getCard();

    //扑克牌的所有者
    void setOwner(Player* player);
    Player* getOwner();

    //模拟扑克牌的点击事件
    void clicked();

protected:
    //窗口重绘事件
    void paintEvent(QPaintEvent *event);
    //窗口鼠标事件
    void mousePressEvent(QMouseEvent* event);

signals:
    void cardSelected(Qt::MouseButton button);

private:
    QPixmap m_front;//保存正面图片
    QPixmap m_back; //保存背面图片
    bool m_isfront = true; //当前是正面还是反面
    bool m_isSelect = false; //记录当前是否被选中
    Card m_card;
    Player* m_owner = nullptr; //当前这张牌的所有者



};

#endif // CARDPANEL_H
