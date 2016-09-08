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
