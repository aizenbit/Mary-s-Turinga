#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QTableWidgetItem>
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

    void loadRulesToTable();
    void restoreRulesFromTable();
    void on_rulesTable_itemChanged(QTableWidgetItem *item);
    void on_tapeTable_itemChanged(QTableWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QVector<Rule> rules;
};

#endif // MAINWINDOW_H
