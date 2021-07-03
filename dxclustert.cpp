/***************************************************************************
    dxcluster.cpp  -  description
       -------------------
begin                : jan 2015
copyright            : (C) 2015 by Jaime Robles
email                : jaime@robles.es
***************************************************************************/

/*****************************************************************************
* This file is part of MonDX.                                               *
*                                                                           *
*    MonDX is free software: you can redistribute it and/or modify          *
*    it under the terms of the GNU General Public License as published by   *
*    the Free Software Foundation, either version 3 of the License, or      *
*    (at your option) any later version.                                    *
*                                                                           *
*    MonDX is distributed in the hope that it will be useful,                *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*    GNU General Public License for more details.                           *
*                                                                           *
*    You should have received a copy of the GNU General Public License      *
*    along with MonDX.  If not, see <http://www.gnu.org/licenses/>.          *
*                                                                           *
*****************************************************************************/

#include "dxclustert.h"

DXClusterTWidget::DXClusterTWidget(const QString _dir, QWidget *parent )
{
    mondxDir = _dir;
}

DXClusterTWidget::DXClusterTWidget(const QString _dir, const QString &clusterToConnect, const int portToConnect, QWidget *parent )
          : QWidget(parent)
{
    qDebug() << "DXClusterTWidget::DXClusterTWidget" << endl;

    mondxDir = _dir;
    world = new World(mondxDir);
    world->create(mondxDir);

    dxcView = new QListView();
    inputCommand = new QLineEdit();
    sendButton = new QPushButton();
    clearButton = new QPushButton;
    dxcMainTabWidget = new QTabWidget;

    tcpSocket = new QTcpSocket(this);

    //http://www.qrz.com/db/EA4TV
    server = clusterToConnect;
    port = portToConnect;


    //model = new QStandardItemModel(0,6,this); //2 Rows and 6 Columns
    model = new QStandardItemModel(this);
    initClass();

    //dxcView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    dxcView->setModel(model);


    QHBoxLayout *bottonLayout = new QHBoxLayout;
    //bottonLayout->addSpacerItem(new QSpacerItem(10,0,QSizePolicy::Expanding,QSizePolicy::Maximum));
    //bottonLayout->addSpacerItem(new QSpacerItem(10,0,QSizePolicy::Expanding,QSizePolicy::Maximum));
    bottonLayout->addWidget(inputCommand);
    bottonLayout->addWidget(clearButton);
    bottonLayout->addWidget(sendButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(dxcMainTabWidget);
    //mainLayout->addWidget(dxcView);
    mainLayout->addLayout(bottonLayout);

    setLayout(mainLayout);

}

DXClusterTWidget::~DXClusterTWidget()
{

}
void DXClusterTWidget::initClass()
{
    //dxcView->setHorizontalHeader(QHeaderView::setStretchLastSection(true));
    //treeView->header()->setStretchLastSection(false);
    //treeView->header()->setResizeMode(1, QHeaderView::Stretch);

    int i = dxcMainTabWidget->addTab(dxcView, tr("Spots"));

    //(dxcView->horizontalHeader())->setStretchLastSection(true);

    inputCommand->setToolTip(tr("Enter the commands to be sent to the DXCluster server"));
    sendButton->setToolTip(tr("Click to send to the DXCluster server"));
    //clearButton->setToolTip(tr("Click to clear the input box"));



    sendButton->setText(tr("Connect"));
    clearButton->setText(tr("Clear"));

    dxClusterConnected = false;
    dxClusterAlreadyConnected = false;
    showhf = true;
    showvhf = true;
    showwarc = true;
    showann = true;
    showwwv = true;
    showwcy = true;

    connect(sendButton , SIGNAL(clicked()), this, SLOT(slotClusterSendToServer()) );
    connect(inputCommand, SIGNAL(textChanged(QString)), this, SLOT(slotClusterInputTextChanged()) );




    //connect(dxClusterListWidget, SIGNAL(itemDoubleClicked ( QListWidgetItem *)), this, SLOT(slotClusterDXClusterWidgetItemDoubleClicked( QListWidgetItem * )) );
    //connect(dxClusterListWidget, SIGNAL(itemEntered ( QListWidgetItem *)), this, SLOT(slotClusterDXClusterWidgetItemEntered( QListWidgetItem * )) );
    //connect(dxClusterListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotClusterDXClusterWidgetItemSelected() ) );
}

void DXClusterTWidget::slotClusterDisplayError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "DXClusterTWidget:displayError:" << endl;
     switch (socketError) {
     case QAbstractSocket::RemoteHostClosedError:
         break;
     case QAbstractSocket::HostNotFoundError:
         QMessageBox::warning(this, tr("MonDX DXCluster"),
                                  tr("The host was not found. Please check:\n\n"
                                     "- your network connection;\n"
                                     "- the host name and port settings."));
         break;
     case QAbstractSocket::ConnectionRefusedError:
         QMessageBox::warning(this, tr("MonDX DXCluster"),
                                  tr("The connection was refused by the peer. "
                                     "Make sure the DXCluster server is running, "
                                     "and check that the host name and port "
                                     "settings are correct."));
         break;
     default:
         QMessageBox::warning(this, tr("MonDX DXCluster"),
                                  tr("The following error occurred: %1.")
                                  .arg(tcpSocket->errorString()));
     }

 }
void DXClusterTWidget::slotClusterDataArrived()
{
    qDebug() << "DXClusterTWidget::slotClusterDataArrived" << endl;
    QStringList qs;
    QString flagFileName = QString();
    QString dxClusterString = QString();
    QString dxCall = QString();
    QString dxFrequency = QString();
    QString spotBand = QString();
    QString spotter = QString();
    QString comment = QString();

    QString time = QString();
    bool endsTime = false;
    QDate qdate = QDate::currentDate();
    QString date = qdate.toString("dd-MMM");

    spotBand = "-1";
    //bool isADXSpot = false;

    while ( tcpSocket->canReadLine() ) {
        dxClusterString =  tcpSocket->readLine();
        dxClusterString = dxClusterString.simplified().toUtf8();

        QStringList tokens = dxClusterString.split(" ", QString::SkipEmptyParts);
        if (tokens.size()<2){
            return;
        }
        // It is a "DX de SP0TTER FREC DXCALL"
        //0 = DX, 1 = de, 2 = spotter, 3 = Freq, 4 = dxcall, 5 = comment
        qDebug() << "DXClusterTWidget::slotClusterDataArrived: " << "DXCLUSTER->" << dxClusterString << endl;

        if ((tokens[0] == "DX") && (tokens[1] == "de"))
        {
            qDebug() << "DXClusterTWidget::slotClusterDataArrived: DX DE" << endl;
            spotter = tokens[2];
            spotter.truncate(spotter.size() - 1);
            dxFrequency = tokens[3];
            // Convert KHz to MHz..
            dxFrequency = QString::number( (dxFrequency.toFloat())/1000, 'f', 3);
            dxCall = tokens[4];

            qDebug() << "DXClusterTWidget::slotClusterDataArrived: tokens: " << QString::number(tokens.count()) << endl;

            time = tokens.last();
            endsTime = time.contains("Z", Qt::CaseInsensitive);

            if (endsTime)
            {
                qDebug() << "DXClusterTWidget::slotClusterDataArrived: ends with Z - " << tokens.last() << endl;
            }
            else
            {
                qDebug() << "DXClusterTWidget::slotClusterDataArrived: does not end with Z, it is locator - " << tokens.last() << endl;
                time = tokens[(tokens.count())-2];
            }
//DX de JA1HOX:     7003.0  UN7GP       2059Z                          6
//DX de JA1HOX:     7003.0  UN7GP       2059Z           IN80           7
//DX de JA1HOX:     7003.0  UN7GP       TNS     2059Z                  7
//DX de JA1HOX:     7003.0  UN7GP       Tnx     boris   2059Z          8
//DX de JA1HOX:     7003.0  UN7GP       Tnx     boris   2059Z  IN80    9
//0   1  2          3       4           5       6       7      8

            if (tokens.count() != 6)
            {
                if( (tokens.count() == 7) && (endsTime))
                {}
            }

            qDebug() << "DXClusterTWidget::slotClusterDataArrived: Comment: " << comment << endl;

            flagFileName = getFlagFromDX(dxCall);

            addSpotToList(spotter, dxFrequency, dxCall, comment, date, time);
        }
        else if ((tokens[0] == "To") && (tokens[1] == "ALL"))
        {
            qDebug() << "DXClusterTWidget::slotClusterDataArrived: TO ALL" << endl;
            //dxSpotColor = awards->getDefaultColor();
        }
        else
        {
            qDebug() << "DXClusterTWidget::slotClusterDataArrived: DEFAULT" << endl;
            //dxSpotColor = awards->getDefaultColor();
        }
     }

     qDebug() << "DXClusterTWidget::slotClusterDataArrived - (end): " << dxClusterString << endl;

 }

void DXClusterTWidget::slotClusterSocketConnected()
{
    qDebug() << "DXClusterTWidget::slotClusterSocketConnected" << endl;

    QListWidgetItem *item = new QListWidgetItem();
    //item->setForeground(QBrush(awards->getDefaultColor()));
    item->setText(tr("Connected to server"));
    //dxClusterListWidget->insertItem(0,item);


//    dxClusterSpotItem * item = new dxClusterSpotItem(dxclusterListWidget, i18n("Connected to server"), awards->getDefaultColor());
    dxClusterConnected = true;
    inputCommand->setFocus(Qt::OtherFocusReason);

    if (( dxClusterConnected ) && (!dxClusterAlreadyConnected) ){
        bool ok;
        QString callsignText = QInputDialog::getText(this, tr("MonDX message"), tr("Enter your callsign to connect to the cluster:"), QLineEdit::Normal, "", &ok);
        QString passwordText = QInputDialog::getText(this, tr("MonDX message"), tr("Enter your password to connect to the cluster(just enter if no pasword!):"), QLineEdit::Normal, "", &ok);
        QTextStream os(tcpSocket);
        if ( callsignText.length() > 2 && ok ) {
            os << callsignText << "\n";
        //TODO: Check the DXCluster answer and enter the password if needed.
            sendButton->setText(tr("Disconnect"));
            clearButton->setText(tr("Clear"));
            dxClusterAlreadyConnected = true;
        } else {
            os << tr("Not logged on, you may to enter your callsign again.") << "\n";
            dxClusterAlreadyConnected = false;
        }
        inputCommand->setEnabled(true);
        inputCommand->setToolTip(tr("Enter here the commands to be sent to the DX-Cluster server"));

    }
}

void DXClusterTWidget::slotClusterSocketConnectionClosed()
{
    qDebug() << "DXClusterTWidget::slotClusterSocketConnectionClosed" << endl;
    QListWidgetItem *item = new QListWidgetItem();
    //item->setForeground(QBrush(awards->getDefaultColor()));
    item->setText(tr("Connection closed by the server"));
    //dxClusterListWidget->insertItem(0,item);
    dxClusterConnected = false;
    dxClusterAlreadyConnected = false;
    sendButton->setText(tr("Connect"));
    inputCommand->setDisabled(true);
    inputCommand->setToolTip(tr("Click on Connect to connect to the DX-Cluster server"));
    //connect(inputCommand, SIGNAL(returnPressed()), this, SLOT(slotClusterSendToServer()) );
    disconnect (inputCommand, SIGNAL(returnPressed()), this, SLOT(slotClusterSendToServer()) );
}

void DXClusterTWidget::slotClusterSendToServer()
{
    qDebug() << "DXClusterTWidget::slotClusterSendToServer()" << endl;
    if (!dxClusterConnected)
    {

        connectToDXCluster();
        return; // If we try to connect...
    }
    if ( inputCommand ->text().length() < 1 )
    {
        qDebug() << "DXClusterTWidget::slotClusterSendToServer() - Vacio..." << endl;
        QTextStream os(tcpSocket);
        os << "bye\n";
        return;
    }
    //  write to the server
    QTextStream os(tcpSocket);
    os << inputCommand ->text() << "\n";
    inputCommand ->clear();
}

void DXClusterTWidget::slotClusterClearLineInput()
{
    qDebug() << "DXClusterTWidget::slotClusterClearLineInput" << endl;

    if ( ((inputCommand->text()).length()) <= 0 )
    {
        if ( dxClusterConnected )
        {
            QTextStream os(tcpSocket);
            os << "bye\n";
        }
        else
        {
        }
    }
    else
    {
        inputCommand->clear();
    }

}

void DXClusterTWidget::slotClusterInputTextChanged()
{
    qDebug() << "DXClusterTWidget::slotClusterInputTextChanged" << endl;

    if ( ((inputCommand->text()).length()) <= 0 )
    {
        sendButton->setText(tr("Disconnect"));
        clearButton->setText(tr("Clear"));
    }
    else if (dxClusterConnected)
    {
        sendButton->setText(tr("Send"));
        clearButton->setText(tr("Clear"));
    }
    else
    {}
}

void DXClusterTWidget::connectToDXCluster()
{
    qDebug() << "DXClusterTWidget::connectToDXCluster" << endl;
    if (dxClusterConnected)
    {
        qDebug() << "DXClusterTWidget::connectToDXCluster: - Already connected!!" << endl;
        return; // If we are connected we don't want to start another connection
    }

    connect(tcpSocket, SIGNAL(connected()), SLOT(slotClusterSocketConnected()) );
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(slotClusterDataArrived() ));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotClusterDisplayError(QAbstractSocket::SocketError)));
    connect(tcpSocket, SIGNAL(disconnected()), SLOT(slotClusterSocketConnectionClosed()) );
    connect(inputCommand, SIGNAL(returnPressed()), this, SLOT(slotClusterSendToServer()) );
    connect(clearButton, SIGNAL(clicked()), this, SLOT(slotClusterClearLineInput()) );

    tcpSocket->connectToHost(server, port );
    //dxClusterListWidget->setSortingEnabled (false);

    //dxClusterSpotItem * item = new dxClusterSpotItem(dxClusterListWidget, tr("Trying to connect to the server\n"), awards->getDefaultColor());
}

void DXClusterTWidget::addSpotToList(const QString _de, const QString _fr, const QString _dx, const QString _co, const QString _da, const QString _ti)
{
    //qDebug() << "DXClusterTWidget::addSpotToList" << _dx << endl;
    QList<QStandardItem*> DX;
    // "DX de " = 6
    // Calls    ~ 15
    // Freq     = 9
    // DX       ~ 15
    // Date     = 6
    // Time     = 5

   //StandardItemModel *iStandardModel = new QStandardItemModel(this);
   //QStandardItem* item1 = new QStandardItem(QIcon("images/shield-280x280.png"),"");
   //QStandardItem* item2 = new QStandardItem(QIcon("images/shield-280x280.png"),"");
   //iStandardModel->appendRow(item1);
    QString dxC = _dx.simplified();
    dxC = dxC.leftJustified(20, ' ');

    QString deCall = _de.simplified();
    deCall = deCall.leftJustified(20, ' ');

    QString line = QString("%1\t on %2\t de %3\t%4\t%5 %6").arg(dxC, -15).arg(_fr, 9).arg(deCall, -15).arg(_co, -25).arg(_da, -6).arg(_ti, -5);
    QString flagPath = QString();
    qDebug() << "DXClusterTWidget::addSpotToList - line: " << line << endl;

    QStandardItem *de = new QStandardItem(line);
   // QStandardItem *ti = new QStandardItem(line2);
    flagPath = QString("/Users/devel/Desktop/mondx/img/") + getFlagFromDX(_dx) + QString(".png");
    de->setIcon(QIcon(flagPath));


    //model->appendRow(DX);
    DX.append(de);
    //DX.append(ti);
    //DX << de << ti;
    //model->appendRow(flag);
    //model->appendRow(DX);
    model->insertRow(0,DX);

    qDebug() << "DXClusterTWidget::addSpotToList - length: " << QString::number(DX.length()) << endl;
    //model->insertRow(0, DX);

}

QString DXClusterTWidget::getFlagFromDX(const QString _dx)
{
    qDebug() << "DXClusterTWidget::getFlagFromDX: " << _dx << " = " <<  world->getQRZEntityName(_dx) << endl;
    int arrlid = world->getQRZARRLId(_dx);

    switch (arrlid) { // Just to prepare or some tasks before reading DATA from UI
    case 246://Sovereign Order of Malta
    return "un";
    break;
    case 247://Spratly
        return "un";
    break;
    case 260: // Monaco
        return "mc";
    break;
    case 4: // Agalega
        return "mu";
    case 165: // Mauricio
        return "mu";
    break;
    case  207: // Rodriguez
    return "mu";
    break;
    case 49: // Equatorial Guinea
        return "gq";
    break;
    case 195: // Annobon
        return "gq";
    case 176: // Fidji
        return "fj";
    break;
    case 489: // Conway reef
        return "fj";
    break;
    case 460: // Rotuma
        return "fj";
    break;
    case 468: // Swaziland
        return "sz";
    case 474: // Tunisia
        return "tn";
    break;
    case 293: // Vietnam
        return "vn";
    break;
    case 107: //Guinea
        return "gn";
    break;
    case 24:    //Bouvet
        return "bv";
    break;
    case 199:   //"Peter 1 Is"
        return "no";
    break;
    case 18: // Azerbaijan
        return "az";
    break;
    case 75: // Georgia
        return "ge";
    break;
    case 514: // Montenegro
        return "me";
    break;
    case 315: // Sri lanka
        return "lk";
    break;
    case 117: // ITU HQ
        return "ch";
    break;
    case 289: // UN HQ
        return "us";
    break;
    case 511: // Timor Leste
        return "tl";
    break;
    case 336: // Israel
        return "il";
    break;
    case 436: // Libya
        return "ly";
    break;
    case 215: // Cyprus
        return "cy";
    break;
    case 470: // Tanzania
        return "tz";
    break;
    case 450: // Nigeria
        return "ng";
    break;
    case 438: // Madagascar
        return "mg";
    break;
    case 444: //Mauritania        
        return "mr";
    break;
    case 187: // Niger
        return "ne";
    break;
    case 483: // Togo
        return "tg";
    break;
    case 190: // Samoa
        return "ws";
    break;
    case 286: // Uganda
        return "ug";
    break;
    case 430: // Kenya
        return "ke";
    break;
    case 456: // Senegal
        return "sn";
    break;
    case 82: // Jamaica
        return "jm";
    break;
    case 492: //Yemen
        return "ye";
    break;
    case 432: //Lesotho
        return "ls";
    break;
    case 440: // Malawi
        return "mw";
    break;
    case 400: // Algeria
        return "dz";
    break;
    case 62: // Barbados
        return "bb";
    break;
    case 159: // Maldives
        return "mv";
    break;
    case 129: // Guyana
        return "gy";
    break;
    case 497: // Croatia
        return "hr";
    break;
    case 424: // Ghana
        return "gh";
    break;
    case 257: // Malta
        return "mt";
    break;
    case 482: // Zambia
        return "zm";
    break;
    case 348: // Kuwait
        return "kw";
    break;
    case 458: // Sierra Leone
        return "sl";
    break;
    case 299: // West Malaysia
        return "my";
    break;
    case 46: // East Malaysia
        return "my";
    break;
    case 369: // Nepal
        return "np";
    break;
    case 414: // Dem Rep Congo
        return "cd";
    break;
    case 404: // Burundi
        return "bi";
    break;
    case 381: // Singapore
        return "sg";
    break;
    case 454: // Rwanda
        return "rw";
    break;
    case 90: // Trinidad & Tobago
        return "tt";
    break;
    case 402: // Botswana
        return "bw";
    break;
    case 160: // Tonga
        return "to";
    break;
    case 370: // Oman
        return "om";
    break;
    case 306: // Bhutan
        return "bt";
    break;
    case 391: // Un Arab Emirates
        return "ae";
    break;
    case 376: // Qatar
        return "qa";
    break;
    case 304: //Bahrain
        return "bh";
    break;
    case 372: // Pakistan
        return "pk";
    break;
    case 506: // Scarboroug Reef
        return "un";
    break;
    case 386: // Taiwan
        return "tw";
    break;
    case 505: //Pratas Is
        return "tw";
    break;
    case 318: // China
        return "cn";
    break;
    case 157: // Nauru
        return "nr";
    break;
    case 203: // Andorra
        return "ad";
    break;
    case 422: // Gambia
        return "gm";
    break;
    case 60: // Bahamas
        return "bs";
    break;
    case 181: // Mozambique
        return "mz";
    break;
    case 112: // Chile
        return "cl";
    break;
    case 217: //San Felix
        return "cl";
    break;
    case 47: // Easter Is
        return "cl";
    break;
    case 125: // Juan Fernandez is
        return "cl";
    break;
    case 13: // Antartica
        return "un";
    break;
    case 70: // Cuba
        return "cu";
    break;
    case 446: //Morocco
        return "ma";
    break;
    case 104: //Bolivia
        return "bo";
    break;
    case 272: //Portugal
        return "pt";
    break;
    case 256: // Madeira
        return "pt";
    break;
    case 149: // Azores
        return "pt";
    break;
    case 144: // Uruguay
        return "uy";
    break;
    case 211: // Sable Is
        return "ca";
    break;
    case 252: // St Paul is
        return "ca";
    break;
    case 401: // Angola
        return "ao";
    break;
    case 409: //Cape Verde
        return "cv";
    break;
    case 411: // Comoros
        return "km";
    break;
    case 230: // Fed Rep Germany
        return "de";
    break;
    case 375: //Philippines
        return "ph";
    break;
    case 51: // Eritrea
        return "er";
    break;
    case 510: // Palestine
        return "ps";
    break;
    case 191: // North Cook
        return "ck";
    break;
    case 234: // South Cook
        return "ck";
    break;
    case 188: // Niue
        return "nu";
    break;
    case 501: // Bosnia
        return "ba";
    break;
    case 281: // Spain
        return "es";
    break;
    case 21: //Balearic is
        return "es";
    break;
    case 29: // Canary Is
        return "canary";
    break;
    case 32: // Ceuta & Melilla
        return "es";
    break;
    case 245: // Ireland
        return "ie";
    break;
    case 14:    // Armenia
        return "am";
    break;
    case 434:   // Liberia
        return "lr";
    break;
    case 330:   // Iran
        return "ir";
    break;
    case 179:   //Moldova
        return "mv";
    break;
    case 52:    // Estonia
        return "ee";
    break;
    case 53:    //Ethiopia
        return "et";
    break;
    case 27:    //Belarus
        return "by";
    break;
    case 135:   //Kyrgyzstan
        return "kg";
    break;
    case 262:   // Turkmenistan
        return "tm";
    break;
    case 227:   // France
        return "fr";
    break;
    case 79:    // Guadeloupe
        return "fr";
    break;
    case 169:   // Mayotte
        return "yt";
    break;
    case 516:   // St Barthelemy
        return "fr";
    break;
    case 162:   // New Caledonia
        return "nc";
    break;
    case 512:   // Chesterfield Is
        return "nc";
    break;
    case 84:    // Martinique
        return "mq";
    break;
    case 175:   // French Polynesia
        return "pf";
    break;
    case 508:   //Austral Is
        return "pf";
    break;
    case 277:   // St Pierre & Miquelon
        return "pm";
    break;
    case 453: //Reunion Is
        return "re";
    break;
    case 213:   // St Marteen
        return "fr";//TODO: Add the wikipedia flag
    break;
    case 99:    // Glorioso is
        return "fr";
    break;
    case 124:   // Juan de nova, Europa
        return "fr"; //TODO: Add the wikipedia flag
    break;
    case 276:   // Tromelin
        return "fr";//TODO: Add the wikipedia flag
    break;
    case 41:    //Crozet
        return "fr";
    break;
    case 131:   //Kerguelen
        return "fr"; //TODO: Add the wikipedia flag http://es.wikipedia.org/wiki/Tierras_Australes_y_Ant%C3%A1rticas_Francesas
    break;
    case 10:    // Amsterdam & St Paul is
        return "fr";    //TODO: Add the wikipedia flag
    break;
    case 298:   // Wallis & Futuna is
        return "wf";
    break;
    case 63:    // French Guiana
        return "gf";    //TODO: Add the wikipedia flag
    break;
    case 223:   // England
        return "england";
    break;
    case 114:   //Isle of Man
        return "gb";    //TODO: Add the wikipedia flag
    break;
    case 265:   // Northern Ireland
        return "northernireland";
    break;
    case 122:   //Jersey
        return "gb";break;
    break;
    case 279:   // Scotland & Shetland is
        return "scotland";
    break;
    case 106:   // Guernsey
        return "gb";  //TODO: Add the wikipedia flag
    break;
    case 294:   // Wales
        return "wales";
    break;
    case 185:   // Solomon
        return "sb";
    break;
    case 507:   // Temotu Province
        return "sb";
    break;
    case 239:   // Hungary
        return "hu";
    break;
    case 287:   // Switzerland
        return "ch";
    break;
    case 251:   // Liechtenstein
        return "li";
    break;
    case 120:   // Ecuador
        return "ec";
    break;
    case 71:    // Galapagos Is
        return "ec";
    break;
    case 78:    // Haiti
        return "ht";
    break;
    case 72:    // Dominican Rep
        return "do";
    break;
    case 116:   // Colombia
        return "co";
    break;
    case 216:   // San Andres & Providencia
        return "co";
    break;
    case 161:   // Malpelo
        return "co";
    break;
    case 137:   // Rep Korea
        return "kr";
    break;
    case 88:    // Panama
        return "pa";
    break;
    case 80:    // Honduras
        return "hn";
    break;
    case 387:   // Thailand
        return "th";
    break;
    case 295:   // Vatican City
        return "va";
    break;
    case 378:   // Saudi Arabia
        return "sa";
    break;
    case 248:   //Italy
        return "it";
    break;
    case 382:   // Djibouti
        return "dj";
    break;
    case 77:    // Grenada
        return "gd";
    break;
    case 109:   // Guinea-Bissau
        return "gw";
    break;
    case 97:    // St Lucia
        return "lc";
    break;
    case 95:    // Dominica
        return "dm";
    break;
    case 98:    // St Vicent
        return "vc";
    break;
    case 339:   // Japan
        return "jp";
    break;
    case 177:   // Minami Torishima
        return "jp";
    break;
    case 192:   // Ogasawara
        return "jp";
    break;
    case 363:   //Mongolia
        return "mn";
    break;
    case 259:   // Svalbard
        return "sj";
    break;
    case 118:   // Jan Mayen
        return "sj";
    break;
    case 342:   // Jordan
        return "jo";
    break;
    case 291:   // United States
        return "us";
    break;
    case 105:   //Guantanamo Bay
        return "us";
    break;
    case 166:   // Mariana is
        return "mp";
    break;
    case 20:    // Baker & Howland is
        return "us";
    break;
    case 103:   // Guam
        return "gu";
    break;
    case 123:   // Johnston is
        return "us";
    break;
    case 174:   // Midway is
        return "us";
    break;
    case 197:   // Palmyra & Jarbis is
        return "us";
    break;
    case 134:   // Kingman Reef
        return "us";
    break;
    case 110:   // Hawaii
        return "us";        //TODO: Add the wikipedia flag
    break;
    case 138:   // Kure is
        return "us";
    break;
    case 9:     // American Samoa
        return "as";
    break;
    case 515:   // Swains is
        return "as";
    break;
    case 297:   // Wake is
        return "us";           //TODO: Add the wikipedia flag
    break;
    case 6:     //Alaska
        return "us";
    break;
    case 182:   // Navassa Is
        return "us";
    break;
    case 285:   // Us Virgin is
        return "vi";
    break;
    case 202:   // Puerto Rico
        return "pr";
    break;
    case 43:    // Desecheo Is
        return "us";
    break;
    case 266:   // Norway
        return "no";
    break;
    case 100:   // Argentina
        return "ar";
    break;
    case 254:   // Luxembourg
        return "lu";
    break;
    case 146:   // Lithuania
        return "lt";
    break;
    case 212:   // Bulgaria
        return "bg";
    break;
    case 136:   // Peru
        return "pe";
    break;
    case 354:   // Lebanon
        return "lb";
    break;
    case 206:   // Austria & Viena Intl HQ
        return "at";
    break;
    case 224:   // Findland
        return "fi";
    break;
    case 5:     // Aland is
        return "fi";
    break;
    case 167:   // Market Reef
        return "fi";
    break;
    case 503:   // Czech Rep
        return "cz";
    break;
    case 504:   // Slovak Rep
        return "sk";
    break;
    case 209:   // Belgium
        return "be";
    break;
    case 237:   // Greenland
        return "gl";
    break;
    case 222:   // Faroe is
        return "dk";            //TODO: Add the wikipedia flag
    break;
    case 221:   // Denmark
        return "dk";
    break;
    case 163:   // Papua New Guinea
        return "pg";
    break;
    case 91:    // Aruba
        return "aw";
    break;
    case 344:   //Dpr Korea
        return "kp";
    break;
    case 263:   // Netherlands
        return "nl";
    break;
    case 517:   // Curacao
        return "cw";          //TODO: Add the wikipedia flag
    break;
    case 520:   // Bonaire
        return "bq";          //TODO: Add the wikipedia flag
    break;
    case 519:   // Saba & St Eustatius
        return "nl";          //TODO: Add the wikipedia flag
    break;
    case 518:   // Sint Marteen
        return "shm";      //TODO: Add the wikipedia flag
    break;
    case 108:   // Brazil
        return "br";
    break;
    case 56:    // Fernando de Noronha
        return "br";
    break;
    case 253:   // St Peter & St Paul
        return "br";
    break;
    case 273:   //Trindade & Martim Vaz
        return "br";
    break;
    case 140:   // Suriname
        return "sr";
    break;
    case 61:    // Franz Josef Land
        return "ru";
    break;
    case 302:   // Western Sahara
        return "eh";        //TODO: Add the wikipedia flag
    break;
    case 305:   // Bangladesh
        return "bd";
    break;
    case 499:   // Slovenia
        return "si";
    break;
    case 379:   // Seychelles
        return "sc";
    break;
    case 219:   // Sao Tome & Principe
        return "st";
    break;
    case 284:   // Sweden
        return "se";
    break;
    case 269:   // Poland
        return "pl";
    break;
    case 466:   // Sudan
        return "sd";
    break;
    case 479:   // Egypt
        return "eg";
    break;
    case 236:   // Greece
        return "gr";
    break;
    case 180:   // Mount Athos
        return "gr";
    break;
    case 45:    // Dodecanese
        return "gr";
    break;
    case 40:    // Crete
        return "gr";
    break;
    case 282:   // Tuvalu
        return "tv";
    break;
    case 301:   // Western Kiribati
        return "ki";
    break;
    case 31:    // Central Kiribati
        return "ki";
    break;
    case 48:    // Eastern Kiribati
        return "ki";
    break;
    case 490:   // Banaba is
        return "ki";
    break;
    case 232:   // Somalia
        return "so";
    break;
    case 278:   // San Marino
        return "sm";
    break;
    case 22:    //Palau
        return "pw";
    break;
    case 390:   // Turkey
        return "tr";
    break;
    case 242:    // Iceland
        return "is";
    break;
    case 76:    // Guatemala
        return "gt";            //TODO: Add the wikipedia flag
    break;
    case 308:   // Costa Rica
        return "cr";
    break;
    case 37:    //Coco is
        return "cr";
    break;
    case 406:   // Cameroon
        return "cm";
    break;
    case 214:   // Corsica
        return "fr";            // TODO: Add the wikipedia flag
    break;
    case 408:   // Central African Rep
        return "cf";
    break;
    case 412:   // Rep of Congo
        return "cg";
    break;
    case 420:   // Gabon
        return "ga";
    break;
    case 410:   // Chad
        return "td";
    break;
    case 428:   // Cote d'Ivoire
        return "ci";
    break;
    case 416:   // Benin
        return "bj";
    break;
    case 442:   // Mali
        return "ml";
    break;
    case 54:    // European Rusia
        return "ru";
    break;
    case 126:   // Kaliningrad
        return "ru";
    break;
    case 15:    //Asiatic Rusia
        return "ru";
    break;
    case 292:   // Uzbekistan
        return "uz";
    break;
    case 130:   // Kazakhstan
        return "kz";
    break;
    case 288:   //Ukraine
        return "ua";
    break;
    case 94:    // Antigua & Barbuda
        return "ag";
    break;
    case 66:    //Belize
        return "bz";
    break;
    case 249:   // St Kitts & Nevis
        return "kn";
    break;
    case 464:   // Namibia
        return "na";
    break;
    case 173:   // Micronesia
        return "fm";
    break;
    case 168:   // Marshall Is
        return "fm";
    break;
    case 345: // Brunei Darusalam
        return "bn";
    break;
    case 1: //Canada
        return "ca";
    break;
    case 150:   //Australia
        return "au";
    break;
    case 111:   // Heard Is
        return "hm";
    break;
    case 153:   // Macquarie is
        return "au";
    break;
    case 38:    // Cocos / Keeling is
        return "cc";
    break;
    case 147:   // Lord Howe is
        return "au";
    break;
    case 171:   // Mellish Reed
        return "au";
    break;
    case 189:   // Norkfolk is
        return "nf";
    break;
    case 303:   // Willis Is
        return "au";
    break;
    case 35:    // Christmas is
        return "cx";
    break;
    case 12:    // Anguilla
        return "ai";
    break;
    case 96:    // Montserrat
        return "ms";
    break;
    case 65:    // British is
        return "vg";
    break;
    case 89:    // Turks & Caicos is
        return "tc";
    break;
    case 172:   // Pitcairn
        return "pn";
    break;
    case 513:   // Ducie is
        return "gb";
    break;
    case 141:   // Falkland is
        return "fk";
    break;
    case 235:   // South Georgia is
        return "gs";
    break;
    case 241:   // South Shetland is
        return "un";
    break;
    case 238:   // South Orkney is
        return "un";
    break;
    case 240:   // South Sandwich Is
        return "gs";
    break;
    case 64:    // Bermuda
        return "bm";
    break;
    case 33:    // Chagos is
        return "io";
    break;
    case 321:   // Hong Kong
        return "hk";
    break;
    case 324:   // India
        return "in";
    break;
    case 11:    // Andaman & Nicobar
        return "in";
    break;
    case 142:   // Lakshadweep Is
        return "in";
    break;
    case 50:    // Mexico
        return "mx";
    break;
    case 204:   // Revilagigedo
        return "mx";
    break;
    case 480:   // Burkina Faso
        return "bf";
    break;
    case 312:   // Cambodia
        return "kh";
    break;
    case 143:   // Laos
        return "la";
    break;
    case 152:   // Macao
        return "mo";
    break;
    case 309:   // Myanmar
        return "mm";
    break;
    case 3:     //Afganistan
        return "af";
    break;
    case 327:   // Indonesia
        return "id";
    break;
    case 333:   //Iraq
        return "iq";
    break;
    case 158:   // Vanuatu
        return "vu";
    break;
    case 384:   // Syria
        return "sy";
    break;
    case 145:   // Latvia
        return "lv";
    break;
    case 86:    // Nicaragua
        return "ni";
    break;
    case 275:   // Romania
        return "ro";
    break;
    case 74:    // EL Salvador
        return "sv";
    break;
    case 296:   // Serbia && Kosovo
        return "rs";
    break;
    case 148:   // Venezuela
        return "ve";
    break;
    case 17:    // Aves Is
        return "ve";
    break;
    case 452:   // Zimbabwe
        return "zw";
    break;
    case 502:   // Macedonia
        return "mk";
    break;
    case 521:   // Rep South Sudan
        return "ss";
    break;
    case 7:     //Albania
        return "al";
    break;
    case 233:   // Gibratar
        return "gi";
    break;
    case 283:   // UK Base Aereas Cyprus
        return "gb";
    break;
    case 250:   // St Helena
        return "sh";
    break;
    case 205:   // Ascension is
        return "sh";
    break;
    case 274:   // Tristan da Cunha & Gough is
        return "sh";
    break;
    case 69:    // Cayman Is
        return "ky";
    break;
    case 270:   // Tokelau Is
        return "tk";
    break;
    case 170:   // New Zeland
        return "nz";
    break;
    case 34:    // Chatham Is
        return "nz";
    break;
    case 133:   // kermadec is
        return "nz";
    break;
    case 16:    // Auckland & Campbell is
        return "nz";
    break;
    case 132:   // Paraguay
        return "py";
    break;
    case 462:   // South Africa
        return "za";
    break;
    case 201:   // Pr Edward & Marion Is
        return "za";
    break;
    default:
            return "un";
        break;

        }

    return world->getQRZEntityName(_dx);

}


