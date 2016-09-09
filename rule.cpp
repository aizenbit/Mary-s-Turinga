#include "rule.h"

Rule::Rule(QObject *parent) : QObject(parent)
{
    directions << 'R' << 'L' << 'N' << 'r' << 'l' << 'n';
    this->currentState = -1;
    this->currentSymbol = QChar();
    this->nextState = -1;
    this->nextSymbol = QChar();
    this->direction = 'N';
    empty = true;
}

Rule::Rule(const Rule & other) : QObject(other.parent())
{
    *this = other;
}

void Rule::operator =(const Rule & other)
{
    currentState = other.currentState;
    currentSymbol = other.currentSymbol;
    nextState = other.nextState;
    nextSymbol = other.nextSymbol;
    direction = other.direction;
    empty = other.empty;
}

Rule::Rule(int currentState, QChar currentSymbol,
           int nextState, QChar nextSymbol, QChar direction,
           QObject *parent) : Rule(parent)
{
    if (currentState > 0)
        this->currentState = currentState;

    if (nextState > 0)
        this->nextState = nextState;

    this->currentSymbol = currentSymbol;
    this->nextSymbol = nextSymbol;

    if (directions.contains(direction))
        this->direction = direction;
    else
        this->direction = 'N';

    empty = false;
}

QString Rule::toString() const
{
    QString string = QString("q%1%2->q%3%4%5").arg(currentState).arg(currentSymbol)
            .arg(nextState).arg(nextSymbol).arg(direction);
    return string;
}

Rule Rule::fromString(const QString & in)
{
    QString textRule = in.simplified();
    QRegularExpression regExp("^q[0-9]+.->q[0-9]+.[RLN]$");
    regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    if (!regExp.match(textRule).hasMatch())
        return Rule();

    for (int i = 1; i < textRule.size(); i++)
        if (textRule.at(i - 1) == '-' && textRule.at(i) == '>')
        {
            QChar currentSymbol = textRule.at(i - 2);
            int currentState = textRule.mid(1, i - 3).toInt();
            QChar direction = textRule.at(textRule.size() - 1);
            QChar nextSymbol = textRule.at(textRule.size() - 2);
            int nextState = textRule.mid(i + 2, textRule.size() - 3 - (i + 1)).toInt();
            Rule rule(currentState, currentSymbol, nextState, nextSymbol, direction);
            return rule;
        }

    return Rule();
}
