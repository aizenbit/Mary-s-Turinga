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
    void operator =(const Rule & other);
    bool operator ==(const Rule & other);
    void setInitial(bool initial) {this->initial = initial;}
    void setFinal(bool final) {this->final = final;}
    bool isInitial() const {return initial;}
    bool isFinal() const {return final;}
    bool isEmpty() const {return empty;}
    int getCurrentState() const {return currentState;}
    int getNextState() const {return nextState;}
    QChar getCurrentSymbol() const {return currentSymbol;}
    QChar getNextSymbol() const {return nextSymbol;}
    QChar getDirection() const {return direction.toUpper();}
    QSet<QChar> getDirections() const {return directions;}

    QString toString() const;
    static Rule fromString(const QString &in);
signals:

public slots:

private:
    int currentState, nextState;
    QChar currentSymbol, nextSymbol, direction;
    QSet<QChar> directions;
    bool empty, initial, final;
};

#endif // RULE_H
