#include "rule.h"

Rule::Rule(QObject *parent) : QObject(parent)
{
    directions << 'R' << 'L' << 'N' << 'r' << 'l' << 'n';
    this->currentState = -1;
    this->currentSymbol = QChar();
    this->nextState = -1;
    this->nextSymbol = QChar();
    this->direction = QChar();
    initial = false;
    final = false;
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
    initial = other.initial;
    final = other.final;
}

bool Rule::operator ==(const Rule & other)
{
    if (currentState == other.currentState &&
            currentSymbol == other.currentSymbol &&
            nextState == other.nextState &&
            nextSymbol == other.nextSymbol &&
            direction == other.direction &&
            empty == other.empty &&
            initial == other.initial &&
            final == other.final)
        return true;
    else
        return false;
}

Rule::Rule(QString currentState, QChar currentSymbol,
           QString nextState, QChar nextSymbol, QChar direction,
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

    if (currentState > 0 || nextState > 0 ||
        !currentSymbol.isNull() || !nextSymbol.isNull() || !direction.isNull())
        empty = false;
}

QString Rule::toString() const
{
    QString string = QString("%1;%2->%3;%4;%5")
            .arg(currentState)
            .arg(currentSymbol.isNull() ? ' ' : currentSymbol)
            .arg(nextState)
            .arg(nextSymbol.isNull() ? ' ' : nextSymbol)
            .arg(direction.isNull() ? ' ' : direction);

    if (initial)
        string += "i";
    if (final)
        string += "f";

    return string;
}

Rule Rule::fromString(const QString & in)
{
    QString textRule = in.simplified();
    QRegularExpression regExp("^[0-9]+;.->[0-9]+;.;[RLN][Ii]?[Ff]?$");
    regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    if (!regExp.match(textRule).hasMatch())
        return Rule();

    bool isFinal = textRule.at(textRule.size() - 1).toLower() == 'f';
    if (isFinal)
        textRule.remove(textRule.size() - 1, 1);

    bool isInitial = textRule.at(textRule.size() - 1).toLower() == 'i';
    if (isInitial)
        textRule.remove(textRule.size() - 1, 1);

    QStringList elements = textRule.split(QRegularExpression("(;|->)"));

    QChar currentSymbol, nextSymbol, direction;
    QString currentState = elements.at(0);
    QString nextState = elements.at(2);

    if (!elements.at(1).isEmpty())
        currentSymbol = elements.at(1).at(0);

    if (!elements.at(1).isEmpty())
        nextSymbol = elements.at(3).at(0);

    if (!elements.at(1).isEmpty())
        direction = elements.at(4).at(0);

    Rule rule(currentState, currentSymbol, nextState, nextSymbol, direction);
    rule.setInitial(isInitial);
    rule.setFinal(isFinal);
    return rule;


    return Rule();
}
