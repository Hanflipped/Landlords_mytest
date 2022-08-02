#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QWidget>

namespace Ui {
class ButtonGroup;
}

class ButtonGroup : public QWidget
{
    Q_OBJECT

public:
    enum Panel{Start, PlayCard, PassOrPlay, CallLord, Empty}; //??枚举东西是什么？是变量？为什么每一个的变量还有一个值？
    explicit ButtonGroup(QWidget *parent = nullptr);
    ~ButtonGroup();

    // 初始化按钮
    void initButtons();

    //处理page页的切换
    void selectPanel(Panel type); //??type的意义是什么？(传递参数的名字)

signals: //在类外进行定义
    //开始游戏
    void startGame();
    // 出牌
    void playHand();
    //不出牌
    void pass();
    //抢地主
    void betPoint(int bet);

private:
    Ui::ButtonGroup *ui;

};

#endif // BUTTONGROUP_H
