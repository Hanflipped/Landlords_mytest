#include "cards.h"
#include <QRandomGenerator>
#include <QDebug>

Cards::Cards()
{

}

Cards::Cards(const Card &card)
{
    add(card);
}

void Cards::add(const Card &card)
{
    m_cards.insert(card);
}

void Cards::add(const Cards &cards)
{
    m_cards.unite(cards.m_cards); //??
}

void Cards::add(const QVector<Cards> &cards) //??跟上一个有什么区别
{
    for(int i=0; i < cards.count(); ++i)
    {
        add(cards.at(i)); //??
    }
}

Cards &Cards::operator<<(const Card &card)
{
    add(card);
    return *this;
}

Cards &Cards::operator<<(const Cards &cards)
{
    add(cards);
    return *this;
}

void Cards::remove(const Card &card)
{
    m_cards.remove(card); //？？.remove()什么用法
}

void Cards::remove(const Cards &cards)
{
    m_cards.subtract(cards.m_cards);
}

void Cards::remove(const QVector<Cards> &cards) //??为什么要用容器
{
    for(int i=0; i<cards.size(); ++i)
    {
        remove(cards.at(i));//？？at的用法
    }
}

int Cards::cardCount()
{
    return m_cards.size();
}

bool Cards::isEmpty()
{
    return m_cards.isEmpty();
}

void Cards::clear()
{
    m_cards.clear();
}
//得到最小点数
//??
Card::CardPoint Cards::maxPoint()
{
    Card::CardPoint max = Card::Card_Begin;//??
    if(!m_cards.isEmpty())
    {
        for(auto it = m_cards.begin(); it!= m_cards.end(); ++it)
        {
            if(it->point() > max)
            {
                max = it->point();
            }
        }
    }
    return max;
}

Card::CardPoint Cards::minPoint()
{
    Card::CardPoint min = Card::Card_End; //??枚举值能够判断大小吗
    if(!m_cards.isEmpty())
    {
        for(auto it = m_cards.begin(); it != m_cards.end(); ++it)
        {
            if(it->point() < min)
            {
                min = it->point();
            }
        }
    }
    return min;
}

int Cards::pointCount(Card::CardPoint point)
{
    int count = 0;
    for(auto it = m_cards.begin(); it != m_cards.end(); ++it)
    {
        if(it->point() == point)
        {
            count++;
        }
    }
    return count;
}

bool Cards::contains(const Card &card)
{
    return m_cards.contains(card);//??
}

bool Cards::contains(const Cards &cards)
{
    return m_cards.contains(cards.m_cards);//??
}

Card Cards::takeRandomCard()
{
    //生成一个随机数
    int num = QRandomGenerator::global()->bounded(m_cards.size());//?
    QSet<Card>::const_iterator it = m_cards.constBegin();//?? m_cards的类型到底是什么
    for(int i=0; i<num; ++i, ++it);
    Card card = *it;
    m_cards.erase(it); //??
    return card;
}

//类型转换
CardList Cards::toCardList(SortType type)
{
    CardList list;
    for(auto it = m_cards.begin(); it != m_cards.end(); ++it)
    {
        list << *it;
    }
    if(type == Asc)
    {
        std::sort(list.begin(), list.end(), lessSort);//??lessSort在哪里
    }
    else if(type == Desc)
    {
        std::sort(list.begin(), list.end(), greaterSort);
    }
    return list;
}

void Cards::printAllCardInfo()
{
    QString text;
    char pts[] = "JQKA2";
    for(auto it = m_cards.begin(); it != m_cards.end(); ++it)
    {
        QString msg;
        Card::CardPoint pt = it->point();
        Card::CardSuit suit = it->suit();
        if(suit == Card::CardSuit::Club) //??Cardsuit后面为什么能接作用域
        {
            msg = "梅花";
        }
        else if(suit == Card::CardSuit::Diamond)
        {
            msg = "方片";
        }
        else if(suit == Card::CardSuit::Heart)
        {
            msg = "红桃";
        }
        else
        {
            msg = "黑桃";
        }
        if(pt>=Card::Card_3 && pt <= Card::Card_10)
        {
            msg = QString("%1%2").arg(msg).arg(pt+2);//??为什么要+2
        }
        else if(pt >= Card::Card_J && pt <= Card::Card_2)
        {
            msg = QString("%1%2").arg(msg).arg(pts[pt-Card::Card_J]);//??pts.pt-Card用法？
        }
        if(pt == Card::Card_BJ)
        {
            msg = "Big Joker";
        }
        if(pt == Card::Card_SJ)
        {
            msg = "Small Joker";
        }
        msg += " ";//??这句命令有什么意义？
        text += msg;
    }

    qDebug() << text;
}



