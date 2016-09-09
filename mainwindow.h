#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include "rule.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionHelp_triggered();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

private:
    Ui::MainWindow *ui;
    QVector<Rule> rules;
};

#endif // MAINWINDOW_H
