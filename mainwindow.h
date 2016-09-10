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
    void on_rulesTable_itemChanged(QTableWidgetItem *item);
    void on_tapeTable_itemChanged(QTableWidgetItem *item);
    void on_actionStart_Pause_triggered();
    void on_rulesTable_customContextMenuRequested(const QPoint &pos);
    void on_tapeTable_customContextMenuRequested(const QPoint &pos);
    void on_actionInitial_triggered();
    void on_actionFinal_triggered();
    void on_actionInitial_Celll_triggered();

    void loadRulesToTable();
    void loadCharactersToTape();
    void restoreRulesFromTable();
    void setRowColor(const QColor & color, const int currentRow);
    void setCellColor(const QColor & color, const int currentColumn);
    void addRow();
    void addCell();


private:
    Ui::MainWindow *ui;
    QVector<Rule> rules;
    QVector<Rule> runTimeRules;
    QVector<QChar> tape;
    QVector<QChar> runTimeTape;
    int selectedCharacter, runTimeSelectedCharacter;
    QMenu *rulesContextMenu, *tapeContextMenu;
    bool run;

    QColor green, red;

};

#endif // MAINWINDOW_H
