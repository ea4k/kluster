#include <QtGui>
#include <QApplication>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString version = "0.0.1";
    QDir d1 = QDir();
    QString appDir;

#ifdef Q_OS_WIN
    //qDebug() << "WINDOWS DETECTED!"  << endl;
    appDir = QDir::homePath()+"/kluster";  // We create the \kluster for the logs and data

#else
    //qDebug() << "NO WINDOWS DETECTED!"  << endl;
    appDir = QDir::homePath()+"/.kluster";  // We create the ~/.kluster
#endif


    if (!QDir::setCurrent (appDir) )
    {
        if (d1.mkdir(appDir))
        {
            if (QDir::setCurrent (appDir) )
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



    MainWindow w(appDir, version);
    w.show();

    return a.exec();
}
