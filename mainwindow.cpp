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
    rulesContextMenu = new QMenu();
    rulesContextMenu->addActions(QList<QAction *>() << ui->actionInitial << ui->actionFinal);
    tapeContextMenu = new QMenu();
    tapeContextMenu->addActions(QList<QAction *>() << ui->actionInitial_Celll);
    addRow();
    addCell();
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

    if (!file.open(QFile::WriteOnly | QFile::Text))
        return;

    QTextStream out(&file);
    for (const Rule & rule : rules)
         out << rule.toString() << '\n';

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

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

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
    ui->rulesTable->setRowCount(0);
    addRow();

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
        if (rule.isInitial())
            setRowColor(green, row);

        if (rule.isFinal())
            setRowColor(red, row);
    }

    ui->rulesTable->update();
}

void MainWindow::restoreRulesFromTable()
{
    rules.clear();

    for (int row = 0; row < ui->rulesTable->rowCount(); row++)
    {
        int column = 0;
        int currentState = -1, nextState = -1;
        QChar currentSymbol, nextSymbol, direction;

        currentState = ui->rulesTable->item(row, column++)->text().toInt();

        if (!ui->rulesTable->item(row, column)->text().isEmpty())
            currentSymbol = ui->rulesTable->item(row, column++)->text().at(0);

        nextState = ui->rulesTable->item(row, column++)->text().toInt();

        if (!ui->rulesTable->item(row, column)->text().isEmpty())
            nextSymbol = ui->rulesTable->item(row, column++)->text().at(0);

        if (!ui->rulesTable->item(row, column)->text().isEmpty())
            direction = ui->rulesTable->item(row, column++)->text().at(0);

        Rule rule(currentState, currentSymbol, nextState, nextSymbol, direction);

        if (ui->rulesTable->item(row, 0)->backgroundColor() == green)
            rule.setInitial(true);

        if (ui->rulesTable->item(row, 0)->backgroundColor() == red)
            rule.setFinal(true);

        if (!rule.isEmpty())
            rules.push_back(rule);
    }
}

void MainWindow::loadCharactersToTape()
{
    tape.clear();

    for (int column = 0; column < ui->tapeTable->columnCount(); column++)
    {
        QTableWidgetItem * item = ui->tapeTable->item(0, column);
        if (!item->text().isEmpty())
        {
            tape.push_back(item->text().at(0));

            if (item->backgroundColor() == green)
                selectedCharacter = column;
        }
        else
            tape.push_back(QChar());
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

    if (item->row() == ui->rulesTable->rowCount() - 1)
        addRow();
}

void MainWindow::on_tapeTable_itemChanged(QTableWidgetItem *item)
{
    QString text = item->text();

    if (text.isEmpty())
        return;

    if (text.size() > 1)
          item->setText(text.at(text.size() - 1));

    if (item->column() == ui->tapeTable->columnCount() - 1)
        addCell();
}

void MainWindow::on_actionStart_Pause_triggered()
{
    loadRulesToTable();
    loadCharactersToTape();
    runTimeRules = rules;
    runTimeTape = tape;

    if (runTimeRules.isEmpty() || runTimeTape.isEmpty())
        return;

    Rule &currentRule = runTimeRules[0];

    for (Rule &rule : runTimeRules)
        if (rule.isInitial())
        {
            currentRule = rule;
            break;
        }

    run = true;

    /*while (!currentRule.isFinal() && run)
    {

    }*/

    run = false;
}

void MainWindow::on_rulesTable_customContextMenuRequested(const QPoint &pos)
{
    rulesContextMenu->exec(ui->rulesTable->mapToGlobal(pos));
}

void MainWindow::on_tapeTable_customContextMenuRequested(const QPoint &pos)
{
    tapeContextMenu->exec(ui->tapeTable->mapToGlobal(pos));
}

void MainWindow::setRowColor(const QColor & color, const int currentRow)
{
    for (int row = 0; row < ui->rulesTable->rowCount(); row++)
        for (int column = 0; column < ui->rulesTable->columnCount(); column++)
            if (ui->rulesTable->item(row, column)->backgroundColor() == color)
                ui->rulesTable->item(row, column)->setBackgroundColor(Qt::white);

    for (int column = 0; column < ui->rulesTable->columnCount(); column++)
        ui->rulesTable->item(currentRow, column)->setBackgroundColor(color);
}

void MainWindow::setCellColor(const QColor & color, const int currentColumn)
{
    for (int column = 0; column < ui->tapeTable->columnCount(); column++)
        if (ui->tapeTable->item(0, column)->backgroundColor() == color)
        {
            ui->tapeTable->item(0, column)->setBackgroundColor(Qt::white);
            break;
        }

    ui->tapeTable->item(0, currentColumn)->setBackgroundColor(color);
}

void MainWindow::on_actionInitial_triggered()
{
    int currentRow = ui->rulesTable->rowAt(ui->rulesTable->mapFromGlobal(rulesContextMenu->pos()).y());
    setRowColor(green, currentRow);
}

void MainWindow::on_actionFinal_triggered()
{
    int currentRow = ui->rulesTable->rowAt(ui->rulesTable->mapFromGlobal(rulesContextMenu->pos()).y());
    setRowColor(red, currentRow);
}

void MainWindow::on_actionInitial_Celll_triggered()
{
    int currentColumn = ui->tapeTable->columnAt(ui->tapeTable->mapFromGlobal(tapeContextMenu->pos()).x());
    setCellColor(green, currentColumn);
}

void MainWindow::addRow()
{
    ui->rulesTable->setRowCount(ui->rulesTable->rowCount() + 1);
    for (int column = 0; column < ui->rulesTable->columnCount(); column++)
        ui->rulesTable->setItem(ui->rulesTable->rowCount() - 1, column,
                                new QTableWidgetItem());
}

void MainWindow::addCell()
{
    ui->tapeTable->setColumnCount(ui->tapeTable->columnCount() + 1);
    ui->tapeTable->setItem(0, ui->tapeTable->columnCount() - 1,
                           new QTableWidgetItem());
}

