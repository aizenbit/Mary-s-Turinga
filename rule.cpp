#include "rule.h"

Rule::Rule(QObject *parent) : QObject(parent)
{
    directions << 'R' << 'L' << 'N' << 'r' << 'l' << 'n';
    this->currentState = 0;
    this->currentSymbol = QChar();
    this->nextState = 0;
    this->nextSymbol = QChar();
    this->direction = 'N';
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
}

QString Rule::toString() const
{
    QString string = QString("q%1%2->q%3%4%5").arg(currentState).arg(currentSymbol)
            .arg(nextState).arg(nextSymbol).arg(direction);
    return string;
}

bool Rule::fromString(const QString & in)
{
    QRegularExpression regExp("^q[0-9]+.->q[0-9]+.[RLN]$");
    regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    if (!regExp.match(in).hasMatch())
        return false;

    for (int i = 1; i < in.size(); i++)
        if (in.at(i - 1) == '-' && in.at(i) == '>')
        {
            currentSymbol = in.at(i - 2);
            currentState = in.mid(1, i - 3).toInt();
            direction = in.at(in.size() - 1);
            nextSymbol = in.at(in.size() - 2);
            nextState = in.mid(i + 2, in.size() - 3 - (i + 1)).toInt();
            break;
        }

    return true;
}
