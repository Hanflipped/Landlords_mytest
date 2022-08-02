#ifndef ROBOT_H
#define ROBOT_H

#include "player.h"
#include <QObject>

class Robot : public Player
{
    Q_OBJECT //??
public:
    using Player::Player; //??可以使用父类里的构造函数
    explicit Robot(QObject *parent = nullptr); //??

    void prepareCallLord() override; //override做错误检查
    void preparePlayHand() override;
};

#endif // ROBOT_H
