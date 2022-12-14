#include "gamepanel.h"
#include "ui_gamepanel.h"

#include <QPainter>
#include <QRandomGenerator> //生成随机数
#include <QRect>
#include <QTimer>

GamePanel::GamePanel(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GamePanel)
{
    ui->setupUi(this);


    //1.背景图
    int num = QRandomGenerator::global()->bounded(10);//随机生成0-9的数字
    QString path = QString(":/images/background-%1.png").arg(num+1);//进行路径拼接
    m_bkImage.load(path);

    //2.窗口标题的大小
    this->setWindowTitle("欢乐斗地主");
    this->setFixedSize(1000,650);

    //3.实例化游戏控制对象
    gameControlInit();
    //4.玩家得分(更新）
    updatePlayerScore();//注意函数的调用顺序

    //5.切割游戏图片
    initCardMap();

    //6.初始化游戏中的按钮组
    initButtonsGroup();

    //7.初始化玩家在窗口中的上下文环境
    initPlayerContext();

    //8. 扑克牌场景初始化
    initGameScene();

    //定时器实例化
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GamePanel::onDispatchCard);

}

GamePanel::~GamePanel()
{
    delete ui;
}

void GamePanel::gameControlInit()
{
    m_gameCtl = new GameControl(this);//先实例化指定一个父对象，然后可以自动析构 ???传递的this
    m_gameCtl->playerInit();
    //得到三个玩家的实例对象
    Robot* leftRobot = m_gameCtl->getLeftRobot();
    Robot* rightRobot = m_gameCtl->getRightRobot();
    UserPlayer* user = m_gameCtl->getUserPlayer();
    //存储的顺序:左侧机器人，右侧机器人，当前玩家
    m_playerList << leftRobot << rightRobot << user; //<<被重载了，可以执行插入操作

    connect(m_gameCtl, &GameControl::playerStatusChanged, this, &GamePanel::onPlayerStatusChanged);
    connect(m_gameCtl, &GameControl::notifyGrabLordBet, this, &GamePanel::onGrabLordBet);
    connect(m_gameCtl, &GameControl::gameStatusChanged, this, &GamePanel::gameStatusPrecess);
}

void GamePanel::updatePlayerScore()
{
    ui->scorePanel->setScores(
                m_playerList[0]->getScore(),
                m_playerList[1]->getScore(),
            m_playerList[2]->getScore());
}

void GamePanel::initCardMap()
{
    //1.加载大图
    QPixmap pixmap(":/images/card.png"); //QPixmap的用法？？如何查看头文件的source源文件
    //2.计算每张图片大小
    m_cardSize.setWidth(pixmap.width() / 13);
    m_cardSize.setHeight(pixmap.height() / 5);

    //3.设置背景图
    m_cardBackImg = pixmap.copy(2*m_cardSize.width(), 4*m_cardSize.height(), m_cardSize.width(), m_cardSize.height());

    //正常花色
    for(int i=0, suit=Card::Suit_Begin+1; suit<Card::Suit_End; ++suit, ++i)
    {
        for(int j=0, pt=Card::Card_Begin+1; pt<Card::Card_SJ; ++pt, ++j)
        {
            Card card((Card::CardPoint)pt, (Card::CardSuit)suit);
            //裁剪图片
            cropImage(pixmap, j*m_cardSize.width(), i*m_cardSize.height(), card);
        }
    }
    //大小王
    Card c;
    c.setPoint(Card::Card_SJ);
    c.setSuit(Card::Suit_Begin);
    cropImage(pixmap, 0, 4*m_cardSize.height(), c);

    c.setPoint(Card::Card_BJ);
    cropImage(pixmap, m_cardSize.width(), 4*m_cardSize.height(), c);
}

void GamePanel::cropImage(QPixmap &pix, int x, int y, Card& c)
{
    QPixmap sub = pix.copy(x, y, m_cardSize.width(), m_cardSize.height());
    CardPanel* panel = new CardPanel(this); //传递到窗口中,传递的this是什么？？Qt的运行机制是？
    panel->setImage(sub, m_cardBackImg);
    panel->setCard(c);
    panel->hide(); //隐藏窗口
    m_cardMap.insert(c, panel); //保存卡片和窗口

}

void GamePanel::initButtonsGroup()
{
    ui->btnGroup->initButtons();
    ui->btnGroup->selectPanel(ButtonGroup::Start);

    connect(ui->btnGroup, &ButtonGroup::startGame, this, [=](){
        //界面的初始化
        ui->btnGroup->selectPanel(ButtonGroup::Empty);
        m_gameCtl->clearPlayerScore(); //清空分数,可做可不做
        updatePlayerScore();//可做可不做
        //修改游戏->发牌状态
        gameStatusPrecess(GameControl::DispatchCard);
    });
    connect(ui->btnGroup, &ButtonGroup::playHand, this, [=](){});
    connect(ui->btnGroup, &ButtonGroup::pass, this, [=](){});
    connect(ui->btnGroup, &ButtonGroup::betPoint, this, [=](int bet){
        m_gameCtl->getUserPlayer()->grabLordBet(bet);
    });
}

void GamePanel::initPlayerContext()
{
    //1.放置玩家扑克牌的区域
    const QRect cardsRect[] =  //??QRect的使用，如何确定位置？是通过ui文件吗
    {
        //x ,y(左上角是原点), width, height
        QRect(90, 130, 100, height() - 200), //左侧机器人
        QRect(rect().right() - 190, 130, 100, height() - 200),  //右侧机器人
        QRect(250, rect().bottom() - 120, width() - 500, 100) //当前玩家
    };

    //2.玩家出牌的区域

    const QRect playHandRect[] =
    {
        QRect(260, 150, 100, 100), //左侧机器人
        QRect(rect().right() - 360, 150, 100, 100),  //右侧机器人
        QRect(150, rect().bottom() - 290, width() - 300, 105) //当前玩家
    };

    //3.玩家头像显示的位置
    const QPoint roleImagPos[] =
    {
        QPoint(cardsRect[0].left()-80, cardsRect[0].height() / 2 + 20), //左侧机器人
        QPoint(cardsRect[1].right()+10, cardsRect[1].height() / 2 + 20),  //右侧机器人
        QPoint(cardsRect[2].right()-10, cardsRect[2].top() - 10)//当前玩家
    };

    //循环
    int index = m_playerList.indexOf(m_gameCtl->getUserPlayer());
    for(int i=0; i < m_playerList.size(); ++i)
    {
        PlayerContext context;
        context.align = i==index ? Horizontal : Vertical;
        context.isFrontSide = i==index ? true : false;
        context.cardRect = cardsRect[i];
        context.playHandRect = playHandRect[i];
        //提示信息
        context.info = new QLabel(this); //？？传递this的意义？
        context.info->resize(160, 98);
        context.info->hide();
        //显示到出牌区域的中心位置
        QRect rect = playHandRect[i];
        QPoint pt(rect.left() + (rect.width() - context.info->width()) / 2, rect.top() + (rect.height() - context.info->height()) / 2);
        context.info->move(pt);
        //玩家的头像
        context.roleImg = new QLabel(this);
        context.roleImg->resize(84, 120);
        context.roleImg->hide();
        context.roleImg->move(roleImagPos[i]);
        m_contextMap.insert(m_playerList.at(i), context);
    }

}

void GamePanel::initGameScene() //若需要在其它成员函数中使用，则将其指定为成员变量
{
    // 1. 发牌区的扑克牌
        m_baseCard = new CardPanel(this);
        m_baseCard->setImage(m_cardBackImg, m_cardBackImg);
        // 2. 发牌过程中移动的扑克牌
        m_moveCard = new CardPanel(this);
        m_moveCard->setImage(m_cardBackImg, m_cardBackImg);
        // 3. 最后的三张底牌(用于窗口的显示)
        for(int i=0; i<3; ++i)
        {
            CardPanel* panel = new CardPanel(this);
            panel->setImage(m_cardBackImg, m_cardBackImg);
            m_last3Card.push_back(panel);
            panel->hide();
        }
        // 扑克牌的位置
        m_baseCardPos = QPoint((width() - m_cardSize.width()) / 2,
                               height() / 2 - 100);
        m_baseCard->move(m_baseCardPos);
        m_moveCard->move(m_baseCardPos);

        int base = (width() - 3 * m_cardSize.width() - 2 * 10) / 2;
        for(int i=0; i<3; ++i)
        {
            m_last3Card[i]->move(base + (m_cardSize.width() + 10) * i, 20);
        }
}

void GamePanel::gameStatusPrecess(GameControl::GameStatus status)
{
    // 记录游戏状态
    m_gameStatus = status;
    // 处理游戏状态
    switch(status) //当判断条件较多时，使用switch的效率较高
    {
        case GameControl::DispatchCard:
            startDispatchCard();
            break;
        case GameControl::CallingLord:
    {
            //取出底牌数据
            CardList last3Card = m_gameCtl->getSurplusCards().toCardList();
            //给底牌窗口设置图片
            for (int i = 0; i < last3Card.size(); ++i)
            {
                QPixmap front = m_cardMap[last3Card.at(i)]->getImage();
                m_last3Card[i]->setImage(front, m_cardBackImg);
                m_last3Card[i]->hide();
            }
            //开始叫地主
            m_gameCtl->startLordCard();
            break;
    }
        case GameControl::PlayingHand:
            break;
        default:
            break;
    }
}

void GamePanel::startDispatchCard()
{
    //重置每张卡牌的属性
    for (auto it = m_cardMap.begin(); it != m_cardMap.end(); ++it)
    {
        it.value()->setSeclected(false); //设置选中状态(it有value值，其类型可以调用相关函数
        it.value()->setFrontSide(true);
        it.value()->hide();
    }
    //隐藏三张底牌
    for (int i = 0; i < m_last3Card.size(); ++i)
    {
        m_last3Card.at(i)->hide();
    }

    //重置玩家的窗口上下文信息
    int index = m_playerList.indexOf(m_gameCtl->getUserPlayer()); //? .+函数是允许的吗？，.不是只能取属性吗
    for(int i = 0; i < m_playerList.size(); ++i)
    {
        m_contextMap[m_playerList.at(i)].lastCards.clear();
        m_contextMap[m_playerList.at(i)].info->hide();
        m_contextMap[m_playerList.at(i)].roleImg->hide();
        m_contextMap[m_playerList.at(i)].isFrontSide = i==index ? true : false;
    }
    //重置所有玩家的卡牌数据
    m_gameCtl->resetCardData();
    //显示底牌
    m_baseCard->show();
    //隐藏按钮面板
    ui->btnGroup->selectPanel(ButtonGroup::Empty);
    //启动定时器
    m_timer->start(10); //触发动态发牌效果
    //播放背景音乐
}

void GamePanel::cardMoveStep(Player *player, int curPos)
{
    //得到每个玩家的扑克牌展示区域
    QRect cardRect = m_contextMap[player].cardRect;
    //每个玩家的单元步长
    const int unit[] = {
        (m_baseCardPos.x() - cardRect.right()) / 100,
        (cardRect.left() - m_baseCardPos.x()) / 100,
        (cardRect.top() - m_baseCardPos.y()) / 100
    };
    //每次窗口移动时每个玩家对应的牌的坐标位置
    const QPoint pos[] =
    {
        QPoint(m_baseCardPos.x() - curPos * unit[0], m_baseCardPos.y()), //左侧机器人
        QPoint(m_baseCardPos.x() + curPos * unit[1], m_baseCardPos.y()), //右侧机器人
        QPoint(m_baseCardPos.x(),curPos * unit[2] + m_baseCardPos.y()),  //当前玩家
    };

    //移动扑克牌窗口
    int index = m_playerList.indexOf(player);
    m_moveCard->move(pos[index]);

    //临界状态处理
    if (curPos == 0)
    {
        m_moveCard->show();
    }
    if (curPos == 100)
    {
        m_moveCard->hide();
    }
}

void GamePanel::disposeCard(Player *player, const Cards &cards)
{
    Cards& myCard = const_cast<Cards&>(cards);
    CardList list = myCard.toCardList();
    for (int i=0; i < list.size(); ++i)
    {
        CardPanel* panel = m_cardMap[list.at(i)];
        panel->setOwner(player);
    }
    //更新扑克牌在窗口中的显示
    updatePlayerCards(player);
}

void GamePanel::updatePlayerCards(Player *player)
{
    Cards cards = player->getCards();
    CardList list = cards.toCardList(); //按照一定顺序进行排列
    //取出展示扑克牌的区域
    int cardSpace = 20;
    QRect cardsRect = m_contextMap[player].cardRect;
    for(int i=0; i < list.size(); ++i)
    {
        CardPanel* panel = m_cardMap[list.at(i)];
        panel->show();
        panel->raise();
        panel->setFrontSide(m_contextMap[player].isFrontSide);

        //水平或垂直显示
        if(m_contextMap[player].align == Horizontal)
        {
            int leftX = cardsRect.left() + (cardsRect.width() - (list.size() - 1) * cardSpace - panel->width()) / 2;
            int topY = cardsRect.top() + (cardsRect.height() - m_cardSize.height()) / 2;
            if(panel->isSelected())//如果扑克牌能被选中
            {
                topY -= 10;
            }
            panel->move(leftX + cardSpace * i, topY);
        }
        else
        {
            int leftX = cardsRect.left() + (cardsRect.width() - m_cardSize.width()) / 2;
            int topY = cardsRect.top() + (cardsRect.height() - (list.size() - 1) * cardSpace - panel->height()) / 2;
            panel->move(leftX, topY + i * cardSpace);
        }
    }

}

void GamePanel::onDispatchCard()
{
    //记录扑克牌的位置
    static int curMovePos = 0;
    //当前玩家
    Player* curPlayer = m_gameCtl->getCurrentPlayer();
    if(curMovePos >= 100)
    {
        //给玩家发一张牌
        Card card = m_gameCtl->takeOneCard();
        curPlayer->storeDispatchCard(card);
        Cards cs(card);
        disposeCard(curPlayer, cs);
        //切换玩家
        m_gameCtl->setCurrentPlayer(curPlayer->getNextPlayer());
        curMovePos = 0;
        //发牌动画
        cardMoveStep(curPlayer, curMovePos);
        //判断牌是否发完了
        if(m_gameCtl->getSurplusCards().cardCount() == 3)
        {
            //终止定时器
            m_timer->stop();
            //  切换游戏状态
            gameStatusPrecess(GameControl::CallingLord);
            return;
        }
    }
    // 移动扑克牌
    cardMoveStep(curPlayer, curMovePos);
    curMovePos += 15;
}

void GamePanel::onPlayerStatusChanged(Player *player, GameControl::PlayerStatus status)
{
    switch (status)
    {
    case GameControl::ThinkingForCallLord:
        if(player == m_gameCtl->getUserPlayer())
        {
            ui->btnGroup->selectPanel(ButtonGroup::CallLord, m_gameCtl->getPlayerMaxBet());
        }
        break;
    case GameControl::ThinkingForPlayHand:
        break;
    case GameControl::Winning:
        break;
    default:
        break;
    }
}

void GamePanel::onGrabLordBet(Player *player, int bet, bool flag)
{
    // 显示抢地主的信息提示
    PlayerContext context = m_contextMap[player];
    if(bet == 0)
    {
        context.info->setPixmap(QPixmap(":/images/buqinag.png"));
    }
    else
    {
        if (flag)
        {
            context.info->setPixmap(QPixmap(":/images/jiaodizhu.png"));
        }
        else
        {
            context.info->setPixmap(QPixmap(":/images/qiangdizhu.png"));
        }
    }
    context.info->show();

    //显示叫地主的分数
    //播放分数背景音乐
}


void GamePanel::paintEvent(QPaintEvent *ev) //窗口初始化后会自动执行(因为它是回调函数)
{
    Q_UNUSED(ev)
    QPainter p(this); //自动添加头文件alt+enter
    p.drawPixmap(rect(),m_bkImage);
}
