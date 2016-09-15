#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    launched = false;
    paused = false;
    stopped = true;
    green = QColor("#ccffcc");
    red = QColor("#ffcccc");
    rulesContextMenu = new QMenu();
    rulesContextMenu->addActions(QList<QAction *>() << ui->actionInitial << ui->actionFinal);
    tapeContextMenu = new QMenu();
    tapeContextMenu->addActions(QList<QAction *>() << ui->actionInitial_Celll);
    addRow();
    addCell();
    maxTapeSize = 999;
    initialCharacter = -1;

    delaySpinBox = new QSpinBox();
    delaySpinBox->setSuffix(tr(" ms"));
    delaySpinBox->setRange(0, 10000);
    delaySpinBox->setSingleStep(100);
    delaySpinBox->setPrefix(tr("Delay: "));
    ui->toolBar->addWidget(delaySpinBox);
    version = "1.0.1";
    step = 0;

    ui->rulesTable->resizeColumnsToContents();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete rulesContextMenu;
    delete tapeContextMenu;
    delete delaySpinBox;
}

void MainWindow::on_actionHelp_triggered()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle(tr("About") + " Mary's Turinga");
    aboutBox.setIconPixmap(QPixmap("://resources/aboutIcon.png"));
    aboutBox.setText(tr("<strong>Mary's Turinga</strong> v%1 (Russian: Машина Тьюринга) "
                        "is just another emulator of Turing machine.").arg(version));
    aboutBox.setInformativeText("<p>" + tr("<strong>Instruction:</strong><br>"
                                           "Specify the status and data on the tape. In the context menu (RMB), "
                                           "you can choose the initial and final states, as well as to specify "
                                           "the position of the read head on the tape.<br>"
                                           "Whitespace-characters and null characters equivalents and means no reading "
                                           "from or writing to tape.<br>"
                                           "If you have any questions, you can create an issue "
                                           "<a href=https://github.com/aizenbit/Mary-s-Turinga/issues>here</a>.<br><br>") +
                                  tr("Distributed under "
                                   "<a href=https://github.com/aizenbit/Mary-s-Turinga/blob/master/LICENSE>The MIT License</a>.") +
                                "<br>" + tr("Repository: ") +
                                "<a href=https://github.com/aizenbit/Mary-s-Turinga>https://github.com/aizenbit/Mary-s-Turinga</a><br>" +
                                tr("Icons designed by <a href=http://www.flaticon.com/authors/freepik>Freepik</a> and "
                                "<a href=http://www.flaticon.com/authors/hanan>Hanan</a> from "
                                "<a href=http://www.flaticon.com>FlatIcon</a>.<br></p>"));
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
    restoreCharactersFromTape();

    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        ui->statusBar->showMessage(tr("Cannot save file"), 3000);
        return;
    }

    QTextStream out(&file);
    out << tr("# File created by Mary's Turinga v%1 on %2\n"
              "# Syntax: Current_State;Current_Symbol->Next_State;Next_Symbol;Direction[i][f]\n"
              "# Whitespace Symbols like ' ' (space) are the same as blank or null symbols\n"
              "# Direction can be R, L or N - Right, Left or None.\n"
              "# 'i' means that Current_State is initial state, 'f' means final state.\n"
              "# Symbol '#' is used to mark commentary.\n\n").arg(version).arg(QDateTime::currentDateTime().toString("ddd dd MMM yyyy hh:mm:ss"));

    for (const Rule & rule : rules)
         out << rule.toString() << '\n';

    if (!tape.isEmpty())
    {
        out << "Tape:\n";
        for (int i  = 0; i < tape.size(); i++)
        {
            if (tape.at(i).isNull())
                out << ' ';
            else
            {
                if (tape.at(i) == 'i')
                    out << "\\i";
                else
                    out << tape.at(i);
            }

            if (initialCharacter == i)
                out << "i";
        }
    }

    file.close();
    ui->statusBar->showMessage(tr("Successfully saved"), 3000);
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
    {
        ui->statusBar->showMessage(tr("Cannot open file"), 3000);
        return;
    }

    rules.clear();

    while (!file.atEnd())
    {
        QString line = file.readLine();

        if (!line.isEmpty() && line.at(0) == '#')
            continue;

        if (line.simplified().toLower() == "tape:")
            break;

        Rule rule = Rule::fromString(line);

        if (!rule.isEmpty())
            rules.push_back(rule);
    }

    tape.clear();

    if (!file.atEnd())
    {
        QString line = file.readLine();

        for (int i = 0; i < line.size(); i++)
        {
            if (line.at(i) == 'i' && i > 0)
            {
                initialCharacter = i - 1;
                continue;
            }

            if (i < line.size() - 1 &&
                    line.at(i) == '\\' &&
                    line.at(i + 1).toLower() == 'i')
                i++;

            tape.push_back(line.at(i));
        }

    }
    loadRulesToTable(rules);
    loadCharactersToTape(tape);
    ui->statusBar->showMessage(tr("Successfully loaded"), 3000);
}

void MainWindow::loadRulesToTable(QVector<Rule> rules)
{
    ui->rulesTable->clearContents();
    ui->rulesTable->setRowCount(0);
    addRow();

    if (rules.isEmpty())
        return;

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
            setRowColor(red, row, false);
    }

    ui->rulesTable->update();
}

void MainWindow::restoreRulesFromTable()
{
    rules.clear();

    for (int row = 0; row < ui->rulesTable->rowCount(); row++)
    {
        QChar currentSymbol, nextSymbol, direction;

        QString currentState = ui->rulesTable->item(row, 0)->text();

        if (!ui->rulesTable->item(row, 1)->text().isEmpty())
            currentSymbol = ui->rulesTable->item(row, 1)->text().at(0);

        QString nextState = ui->rulesTable->item(row, 2)->text();

        if (!ui->rulesTable->item(row, 3)->text().isEmpty())
            nextSymbol = ui->rulesTable->item(row, 3)->text().at(0);

        if (!ui->rulesTable->item(row, 4)->text().isEmpty())
            direction = ui->rulesTable->item(row, 4)->text().at(0);

        Rule rule(currentState, currentSymbol, nextState, nextSymbol, direction);

        if (ui->rulesTable->item(row, 0)->backgroundColor() == green)
            rule.setInitial(true);

        if (ui->rulesTable->item(row, 0)->backgroundColor() == red)
            rule.setFinal(true);

        if (!rule.isEmpty())
            rules.push_back(rule);
    }
}

void MainWindow::restoreCharactersFromTape()
{
    tape.clear();

    for (int column = 0; column < ui->tapeTable->columnCount(); column++)
    {
        QTableWidgetItem * item = ui->tapeTable->item(0, column);
        if (!item->text().isEmpty())
        {
            tape.push_back(item->text().at(0));

            if (item->backgroundColor() == green)
                initialCharacter = column;
        }
        else
            tape.push_back(QChar());
    }
}

void MainWindow::loadCharactersToTape(QVector<QChar> tape)
{
    ui->tapeTable->clearContents();
    ui->tapeTable->setColumnCount(0);
    addCell();

    if (tape.isEmpty())
        return;

    for (const QChar & character : tape)

        ui->tapeTable->setItem(0, ui->tapeTable->columnCount() - 1,
                               new QTableWidgetItem(character));

    if (initialCharacter >= 0)
        setCellColor(green, initialCharacter);
}

void MainWindow::on_rulesTable_itemChanged(QTableWidgetItem *item)
{
    QString text = item->text();
    Rule rule;

    if (text.isEmpty())
        return;

    switch (item->column())
    {
    case 0: case 2:
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

void MainWindow::pause(bool pause)
{
    paused = pause;

    if (pause)
    {
        launched = false;
        ui->actionStart_Pause->setIcon(QIcon(":/resources/play-button.png"));
    }
    else
        ui->actionStart_Pause->setIcon(QIcon(":/resources/pause-button.png"));
}

void MainWindow::on_actionStart_Pause_triggered()
{
    ui->actionDebug->setEnabled(false);

    if (!launched && !paused)
    {
        stopped = false;
        launched = true;
        prepareToStart();
        pause(false);
        ui->statusBar->showMessage(tr("Run..."), 3000);
        run();
        return;
    }

    if (paused)
    {
        ui->statusBar->showMessage(tr("Run..."), 3000);
        pause(false);
        launched = true;
        run();
        return;
    }

    pause();
    ui->statusBar->showMessage(tr("Paused on step %1").arg(step), 3000);
}

void MainWindow::prepareToStart()
{
    restoreRulesFromTable();
    restoreCharactersFromTape();
    runTimeRules = rules;
    runTimeTape = tape;
    runTimeSelectedCharacter = initialCharacter;
    step = 0;

    if (runTimeRules.isEmpty() || runTimeTape.isEmpty())
        return;

    states.clear();
    for (Rule &rule : runTimeRules)
        states.insert(rule.getCurrentState(), rule);

    Rule &currentRule = runTimeRules[0];

    for (Rule &rule : runTimeRules)
        if (rule.isInitial())
        {
            currentRule = rule;
            currentState = currentRule.getCurrentState();
            break;
        }
}

void MainWindow::on_actionDebug_triggered()
{
    ui->actionStart_Pause->setEnabled(false);

    if (!launched)
    {
        ui->statusBar->showMessage(tr("Debug started"), 3000);
        prepareToStart();
        launched = true;
        stopped = false;
    }

    bool stop = !singleStep();
    paused = true;

    if (stop)
    {
        ui->actionStart_Pause->setIcon(QIcon(":/resources/play-button.png"));
        ui->actionDebug->setEnabled(false);
        ui->actionStart_Pause->setEnabled(true);
        paused = false;
        launched = false;
    }
}

void MainWindow::on_actionStop_triggered()
{
    ui->statusBar->showMessage(tr("Stopped at step %1").arg(step), 3000);
    launched = false;
    paused = false;
    stopped = true;
    ui->actionStart_Pause->setIcon(QIcon(":/resources/play-button.png"));
    ui->actionDebug->setEnabled(true);
    ui->actionStart_Pause->setEnabled(true);

    qApp->processEvents();
    loadCharactersToTape(tape);
}

void MainWindow::run()
{

    while (launched)
    {
        if (!singleStep())
        {
            break;
        }

        delay(delaySpinBox->value());
    }
}

bool MainWindow::singleStep()
{
    bool stop = true;

    if (!checkCodeAndTape())
    {
        ui->actionStart_Pause->setIcon(QIcon(":/resources/play-button.png"));
        ui->actionDebug->setEnabled(true);
        ui->actionStart_Pause->setEnabled(true);
        launched = false;
        paused = false;
        return false;
    }

    for(Rule &rule : states.values(currentState))
    {
        selectRow(rule);

        if (runTimeTape[runTimeSelectedCharacter] == rule.getCurrentSymbol() ||
                rule.getCurrentSymbol().isNull() || rule.getCurrentSymbol().isSpace())
        {
            if (!rule.isFinal())
                stop = false;

            if(!rule.getNextSymbol().isNull() && !rule.getNextSymbol().isSpace())
                runTimeTape[runTimeSelectedCharacter] = rule.getNextSymbol();

            if(rule.getDirection() == 'R')
                runTimeSelectedCharacter++;

            if(rule.getDirection() == 'L')
                runTimeSelectedCharacter--;

            currentState = rule.getNextState();
            step++;
            break;
        }
    }

    if (stop)
    {
        ui->actionStart_Pause->setIcon(QIcon(":/resources/play-button.png"));
        ui->actionDebug->setEnabled(true);
        ui->actionStart_Pause->setEnabled(true);
        ui->statusBar->showMessage(tr("Done in %1 steps").arg(step), 3000);
        launched = false;
        paused = false;
        return false;
    }

    loadCharactersToTape(runTimeTape);
    selectCell(runTimeSelectedCharacter);

    return true;
}

void MainWindow::selectRow(Rule rule)
{
   for (int i = 0; i < runTimeRules.size(); i++)
       if (rule == runTimeRules[i])
       {
           ui->rulesTable->selectRow(i);
           break;
       }
}

void MainWindow::selectCell(int cell)
{
    if (cell < ui->tapeTable->columnCount())
        ui->tapeTable->selectColumn(cell);
}

bool MainWindow::checkCodeAndTape()
{
    while (runTimeSelectedCharacter >= runTimeTape.size() && runTimeTape.size() < maxTapeSize)
        runTimeTape.push_back(QChar());

    if (step == 0 && runTimeSelectedCharacter < 0)
    {
        ui->statusBar->showMessage(tr("Cannot find initial cell"), 10000);
        return false;
    }

    if((step > 0 && runTimeSelectedCharacter < 0) ||
            runTimeSelectedCharacter >= runTimeTape.size())
    {
        ui->statusBar->showMessage(tr("Out of infinity tape"), 10000);
        return false;
    }

    if (currentState.isEmpty())
    {
        ui->statusBar->showMessage(tr("Cannot find initial state"), 10000);
        return false;
    }

    if (!states.keys().contains(currentState))
    {
        ui->statusBar->showMessage(tr("Cannot find state '%1'").arg(currentState), 10000);
        return false;
    }

    return true;
}

void MainWindow::on_rulesTable_customContextMenuRequested(const QPoint &pos)
{
    if (ui->rulesTable->itemAt(pos)->backgroundColor() == red)
        ui->actionFinal->setChecked(true);
    else
        ui->actionFinal->setChecked(false);
    rulesContextMenu->exec(ui->rulesTable->mapToGlobal(pos));
}

void MainWindow::on_tapeTable_customContextMenuRequested(const QPoint &pos)
{
    tapeContextMenu->exec(ui->tapeTable->mapToGlobal(pos));
}

void MainWindow::setRowColor(const QColor & color, const int currentRow, bool single)
{
    if (single)
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

void MainWindow::on_actionFinal_triggered(bool checked)
{

    int currentRow = ui->rulesTable->rowAt(ui->rulesTable->mapFromGlobal(rulesContextMenu->pos()).y());
    if (checked)
        setRowColor(red, currentRow, false);
    else
        setRowColor(Qt::white, currentRow, false);
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

void MainWindow::delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    qApp->processEvents();
}
