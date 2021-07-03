#include <QtGui>
#include <QApplication>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString version = "0.0.1";
    QDir d1 = QDir();
    QString mondxDir;

#ifdef Q_OS_WIN
    //qDebug() << "WINDOWS DETECTED!"  << endl;
    mondxDir = QDir::homePath()+"/mondx";  // We create the \mondx for the logs and data

#else
    //qDebug() << "NO WINDOWS DETECTED!"  << endl;
    mondxDir = QDir::homePath()+"/.mondx";  // We create the ~/.mondx
#endif


    if (!QDir::setCurrent (mondxDir) )
    {
        if (d1.mkdir(mondxDir))
        {
            if (QDir::setCurrent (mondxDir) )
            {
            }
            else
            {

            }
        }
        else
        {
        }
    }
    else
    {
    }



    MainWindow w(mondxDir, version);
    w.show();

    return a.exec();
}
