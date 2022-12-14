#ifndef GAMEPANEL_H
#define GAMEPANEL_H

#include <QMainWindow>
#include <QMap>
#include <qlabel.h>
#include "cardpanel.h"
#include "gamecontrol.h"
#include "player.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GamePanel; }
QT_END_NAMESPACE

class GamePanel : public QMainWindow
{
    Q_OBJECT

public:
    GamePanel(QWidget *parent = nullptr);
    ~GamePanel();

    //初始化游戏控制类信息
    void gameControlInit();
    //更新分数面板的分数
    void updatePlayerScore();
    //切割并存储图片
    void initCardMap();
    //裁剪图片
    void cropImage(QPixmap& pix, int x, int y, Card& c);
    //初始化游戏按钮组
    void initButtonsGroup();
    //初始化玩家在窗口中的上下文环境
    void initPlayerContext();
    //初始化游戏场景
    void initGameScene();
    //处理游戏的状态
    void gameStatusPrecess(GameControl::GameStatus status);
    // 发牌
    void startDispatchCard();
    //移动扑克牌
    void cardMoveStep(Player* player, int curPos);
    //处理分发得到的扑克牌
    void disposeCard(Player* player, const Cards& cards);
    // 更新扑克牌在窗口的显示
    void updatePlayerCards(Player* player);
    //定时器的处理动作
    void onDispatchCard();
    //处理玩家状态的变化
    void onPlayerStatusChanged(Player* player, GameControl::PlayerStatus status);
    //处理玩家抢地主
    void onGrabLordBet(Player* player, int bet, bool flag);

protected:
    void paintEvent(QPaintEvent* ev);

private:
    enum CardAlign{Horizontal, Vertical};
    struct PlayerContext //定义结构体，方便查询与管理
    {
        //1.玩家扑克牌显示的区域
        QRect cardRect;
        //2.出牌的区域
        QRect playHandRect;
        //3.扑克牌的对齐方式(水平 or 垂直)
        CardAlign align;
        //4.扑克牌显示正面还是背面
        bool isFrontSide;
        //5.游戏过程中的提示信息，比如：不出
        QLabel* info;
        //6.玩家的头像
        QLabel* roleImg;
        //7.玩家刚打出的牌
        Cards lastCards;
    };

    Ui::GamePanel *ui;
    QPixmap m_bkImage;
    GameControl* m_gameCtl;//设置成员变量，能够在整个类里进行访问
    QVector<Player*> m_playerList;
    QMap<Card, CardPanel*> m_cardMap;
    QSize m_cardSize;//保证能在其它类成员函数内使用
    QPixmap m_cardBackImg; //??QPixmap的用法
    QMap<Player*, PlayerContext> m_contextMap; //先遍历，再取出对象实例，放到QMap中，初始化playercontext
    CardPanel* m_baseCard;
    CardPanel* m_moveCard;
    QVector<CardPanel*> m_last3Card;
    QPoint m_baseCardPos;
    GameControl::GameStatus m_gameStatus;
    QTimer* m_timer;

};
#endif // GAMEPANEL_H
