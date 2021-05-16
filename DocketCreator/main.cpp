#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setStyle(QStyleFactory::create("Fusion"));
    /*QPalette p;
    p = qApp->palette();
    p.setColor(QPalette::Window, QColor(53,53,53));
    p.setColor(QPalette::Button, QColor(53,53,53));
    p.setColor(QPalette::Highlight, QColor(142,45,197));
    p.setColor(QPalette::ButtonText, QColor(255,255,255));
    p.setColor(QPalette::WindowText, QColor(255,255,255));
    p.setColor(QPalette::Base, QColor(255,255,255));
    qApp->setPalette(p);*/

    QTranslator translator;
    translator.load(
                QLocale(),
                QLatin1String("DocketCreator"),
                QLatin1String("_"),
                a.applicationDirPath(),
                QLatin1String(".qm"));
    a.installTranslator(&translator);

    MainWindow w;
    w.show();
    return a.exec();
}
