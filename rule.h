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
    explicit Rule(QObject *parent = 0);
    Rule(int currentState, QChar currentSymbol,
         int nextState, QChar nextSymbol, QChar direction,
         QObject *parent = 0);
    QString toString() const;
    bool fromString(const QString & in);

    int getCurrentState() {return currentState;}
    int getNextState() {return nextState;}
    QChar getCurrentSymbol() {return currentSymbol;}
    QChar getNextSymbol() {return nextSymbol;}
    QChar getDirection() {return direction;}
    QSet<QChar> getDirections() {return directions;}
signals:

public slots:

private:
    int currentState, nextState;
    QChar currentSymbol, nextSymbol, direction;
    QSet<QChar> directions;
};

#endif // RULE_H
