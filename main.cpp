#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "QtTest_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();

    return a.exec();
}

////测试非GUI
//#include <iostream>
//#include "Registrating.h"
//#include <string>
//using namespace std;
//
//int main() {
//    string srcPattern = "F:\\shixi\\CS\\registration\\registration\\srcImg";
//    string binPattern = "F:\\shixi\\CS\\registration\\registration\\binImg";
//    string dstPattern = "F:\\shixi\\CS\\registration\\registration\\dstImg";
//    char useSemiAuto = 'n';
//    char useFailedImg = 'n';
//    int startPos = 0;
//    Registrator Registrator;
//    Registrator.Registrating(srcPattern, binPattern, dstPattern, useSemiAuto, useFailedImg, startPos);
//
//    return 0;
//}