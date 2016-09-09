#ifndef RULE_H
#define RULE_H

#include <QObject>
#include <QTextStream>
#include <QRegularExpression>
#include <QSet>
#include <QDebug>

class Rule : public QObject
{
    Q_OBJECT

public:  
    Rule(QObject *parent = 0);
    Rule(const Rule &other);
    Rule(int currentState, QChar currentSymbol,
         int nextState, QChar nextSymbol, QChar direction,
         QObject *parent = 0);

    QString toString() const;
    int getCurrentState() const {return currentState;}
    int getNextState() const {return nextState;}
    QChar getCurrentSymbol() const {return currentSymbol;}
    QChar getNextSymbol() const {return nextSymbol;}
    QChar getDirection() const {return direction;}
    QSet<QChar> getDirections() const {return directions;}
    bool isEmpty() {return empty;}
    void operator =(const Rule & other);
    static Rule fromString(const QString &in);
signals:

public slots:

private:
    int currentState, nextState;
    QChar currentSymbol, nextSymbol, direction;
    QSet<QChar> directions;
    bool empty;
};

#endif // RULE_H
