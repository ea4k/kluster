#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "dxcluster.h"
#include "dxclustert.h"
#include "setupdialog.h"
#include "database.h"
#include "world.h"
#include <QtWidgets>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString _appDir, const QString _version);
    //MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void slotSetup();

private:
    DXClusterTWidget *dxc;

    void initClass();
    bool configured;

    // <SETUPDIALOG>
    SetupDialog *setupDialog;
    QMenu *setupMenu;
    QAction *setupAct;
    // </SETUPDIALOG>

    QString softwareVersion, appDir;

    DataBase *db;
    World *world;

};

#endif // MAINWINDOW_H
