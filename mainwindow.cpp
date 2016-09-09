#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

}
