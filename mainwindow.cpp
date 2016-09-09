#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    run = false;
    green = QColor("#ccffcc");
    red = QColor("#ffcccc");
    contextMenu = new QMenu();
    contextMenu->addActions(QList<QAction *>() << ui->actionInitial << ui->actionFinal);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionHelp_triggered()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle(tr("About") + " Mary's Turinga");
    aboutBox.setInformativeText("<p>" +
                                tr("Mary's Turinga (Russian: Машина Тьюринга) is just another emulator of Turing machine.") + "<br>" +
                                tr("Distributed under The MIT License. See License and Credist page.") +
                                "<br><br>" + tr("Repository:") + "<br>"
                                "<a href=https://github.com/aizenbit/Mary-s-Turinga>https://github.com/aizenbit/Mary-s-Turinga<a></p>");
    aboutBox.exec();
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(0, tr("Save"), "",                                                          
                                                       tr("txt") + "(*.txt);;" +
                                                       tr("tm") +  "(*.tm);;" +
                                                       tr("All Files") + "(*.*)",
                                                    0);
    if (fileName.isEmpty())
        return;

    restoreRulesFromTable();

    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    for (const Rule & rule : rules)
         out << rule.toString() << "\n";

    file.close();
}

void MainWindow::on_actionLoad_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(0, tr("Save"), "",
                                                          tr("tm or txt") +
                                                          "(*.tm, *.txt);;" +
                                                          tr("All Files") +
                                                          "(*.*)", 0);
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    rules.clear();

    while (!file.atEnd())
    {
        QString line = file.readLine();
        Rule rule = Rule::fromString(line);

        if (!rule.isEmpty())
            rules.push_back(rule);
    }

    loadRulesToTable();
}

void MainWindow::loadRulesToTable()
{
    ui->rulesTable->clearContents();
    ui->rulesTable->setRowCount(1);

    for (const Rule & rule : rules)
    {
        int row = ui->rulesTable->rowCount() - 1;
        int column = 0;

        ui->rulesTable->setItem(row, column++,
                                new QTableWidgetItem(QString("%1").arg(rule.getCurrentState())));
        ui->rulesTable->setItem(row, column++,
                                new QTableWidgetItem(QString("%1").arg(rule.getCurrentSymbol())));
        ui->rulesTable->setItem(row, column++,
                                new QTableWidgetItem(QString("%1").arg(rule.getNextState())));
        ui->rulesTable->setItem(row, column++,
                                new QTableWidgetItem(QString("%1").arg(rule.getNextSymbol())));
        ui->rulesTable->setItem(row, column++,
                                new QTableWidgetItem(QString("%1").arg(rule.getDirection())));
        ui->rulesTable->setRowCount(ui->rulesTable->rowCount() + 1);
    }

    ui->rulesTable->update();
}

void MainWindow::restoreRulesFromTable()
{
    rules.clear();

    for (int row = 0; row < ui->rulesTable->rowCount(); row++)
    {
        int column = 0;
        int currentState = ui->rulesTable->item(row, column++)->text().toInt();
        QChar currentSymbol = ui->rulesTable->item(row, column++)->text().at(0);
        int nextState = ui->rulesTable->item(row, column++)->text().toInt();
        QChar nextSymbol= ui->rulesTable->item(row, column++)->text().at(0);
        QChar direction = ui->rulesTable->item(row, column++)->text().at(0);
        Rule rule(currentState, currentSymbol, nextState, nextSymbol, direction);

        if (ui->rulesTable->item(row, 0) != nullptr &&
            ui->rulesTable->item(row, 0)->backgroundColor() == green)
            rule.setInitial(true);

        if (ui->rulesTable->item(row, 0) != nullptr &&
            ui->rulesTable->item(row, 0)->backgroundColor() == red)
            rule.setFinal(true);

        if (!rule.isEmpty())
            rules.push_back(rule);
    }
}

void MainWindow::on_rulesTable_itemChanged(QTableWidgetItem *item)
{
    bool isInteger = false;
    QString text = item->text();
    Rule rule;

    if (text.isEmpty())
        return;

    switch (item->column())
    {
    case 0: case 2:
        text.toInt(&isInteger);

        if (!isInteger)
            text.remove(QRegularExpression("[^0-9]"));

        item->setText(text);
        break;

    case 1: case 3:
        if (text.size() > 1)
              item->setText(text.at(text.size() - 1));
        break;

    case 4:
        if (text.at(0) == '>')
            text[0] = 'R';
        if (text.at(0) == '<')
            text[0] = 'L';
        if (text.at(0) == '=')
            text[0] = 'N';

        if (text.size() == 1 && rule.getDirections().contains(text.at(0)))
            item->setText(text);
        else
            item->setText(QString());

        break;

    default:
        item->setText(QString());
    }
}

void MainWindow::on_tapeTable_itemChanged(QTableWidgetItem *item)
{
    QString text = item->text();

    if (text.size() > 1)
          item->setText(text.at(text.size() - 1));
}

void MainWindow::on_actionStart_Pause_triggered()
{

}

void MainWindow::on_rulesTable_customContextMenuRequested(const QPoint &pos)
{
    contextMenu->exec(ui->rulesTable->mapToGlobal(pos));
}

void MainWindow::setRowColor(const QColor & color, int currentRow)
{
    for (int row = 0; row < ui->rulesTable->rowCount(); row++)
        for (int column = 0; column < ui->rulesTable->columnCount(); column++)
        {
            QTableWidgetItem *item = ui->rulesTable->item(row, column);
            if (item == nullptr)
                break;
            if (item->backgroundColor() == color)
                item->setBackgroundColor(Qt::white);
        }

    for (int column = 0; column < ui->rulesTable->columnCount(); column++)
    {
        QTableWidgetItem *item = ui->rulesTable->item(currentRow, column);

        if (item == nullptr)
        {
            item = new QTableWidgetItem();
            ui->rulesTable->setItem(currentRow, column, item);
        }

        item->setBackgroundColor(color);
    }
}

void MainWindow::on_actionInitial_triggered()
{
    int currentRow = ui->rulesTable->rowAt(ui->rulesTable->mapFromGlobal(contextMenu->pos()).y());
    setRowColor(green, currentRow);
}

void MainWindow::on_actionFinal_triggered()
{
    int currentRow = ui->rulesTable->rowAt(ui->rulesTable->mapFromGlobal(contextMenu->pos()).y());
    setRowColor(red, currentRow);
}
