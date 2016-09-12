#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QApplication a(argc, argv);

    QTranslator myTranslator;
    myTranslator.load("Mary-s-Turinga-" + QLocale::system().name(), "translations");
    a.installTranslator(&myTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
