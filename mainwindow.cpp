#include "mainwindow.h"

MainWindow::MainWindow(const QString _appDir, const QString _version)
    //: QMainWindow(parent)
{
    dxc = new DXClusterTWidget(_appDir, "dxfun.com", 8000, this);
    appDir = _appDir;
    softwareVersion = _version;

    configured = false;
    setupDialog = new SetupDialog(!configured);

    db = new DataBase(softwareVersion);
    world = new World(appDir);

    if (!db->createConnection())
    {
        //qDebug() << "MainWindow::MainWindow: 4" << endl;
        world->create(appDir);
        return;
    }
    else
    {
        world->create(appDir);
        //db->updateIfNeeded(); // Check if we need to update the DB
    }

    initClass();

    // <MENU>
    setupMenu = menuBar()->addMenu(tr("&Setup"));
    setupAct = new QAction(tr("&Setup..."), this);
    setupMenu->addAction(setupAct);
    setupAct->setMenuRole(QAction::PreferencesRole);
    connect(setupAct, SIGNAL(triggered()), this, SLOT(slotSetup()));

    // </MENU>

    setCentralWidget(dxc);
}

MainWindow::~MainWindow()
{

}

void MainWindow::initClass()
{


}

void MainWindow::slotSetup()
{
    qDebug() << "MainWindow::slotSetup" << endl;
    setupDialog->exec();
}
