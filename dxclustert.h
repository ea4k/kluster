#ifndef DXCLUSTERT_H
#define DXCLUSTERT_H
/***************************************************************************
    dxclustert.h  -  description
       -------------------
begin                : jan 2015
copyright            : (C) 2015 by Jaime Robles
email                : jaime@robles.es
***************************************************************************/

/*****************************************************************************
* This file is part of Kluster.                                               *
*                                                                           *
*    Kluster is free software: you can redistribute it and/or modify          *
*    it under the terms of the GNU General Public License as published by   *
*    the Free Software Foundation, either version 3 of the License, or      *
*    (at your option) any later version.                                    *
*                                                                           *
*    Kluster is distributed in the hope that it will be useful,               *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*    GNU General Public License for more details.                           *
*                                                                           *
*    You should have received a copy of the GNU General Public License      *
*    along with Kluster.  If not, see <http://www.gnu.org/licenses/>.         *
*                                                                           *
*****************************************************************************/

#include <QWidget>
#include <QTcpSocket>
#include <QtWidgets>
#include "world.h"



class QWidget;
class QTcpSocket;

class DXClusterTWidget : public QWidget
{
     Q_OBJECT

 public:
    DXClusterTWidget(const QString _dir, QWidget *parent );
    DXClusterTWidget(const QString _dir, const QString &clusterToConnect, const int portToConnect, QWidget *parent );

    ~DXClusterTWidget();

private slots:
    void slotClusterDisplayError(QAbstractSocket::SocketError socketError);
    void slotClusterDataArrived();
    void slotClusterSocketConnected();
    void slotClusterSocketConnectionClosed();
    void slotClusterSendToServer();
    void slotClusterClearLineInput();
    void slotClusterInputTextChanged();

signals:
    void dxspotclicked(const QStringList _qs); // DXSpotCall, DX-Freq, doubleClicked

private:
    void initClass();
    void connectToDXCluster();
    void addSpotToList(const QString _de, const QString _fr, const QString _dx, const QString _co, const QString _da, const QString _ti);

    QString getFlagFromDX(const QString _dx);



    bool dxClusterConnected, dxClusterAlreadyConnected;
    bool showhf, showvhf, showwarc, showann, showwwv, showwcy;
    QTabWidget *dxcMainTabWidget;
    QListView *dxcView;
    QStandardItemModel *model;
    QLineEdit *inputCommand;
    QPushButton *sendButton, *clearButton;

    QTcpSocket *tcpSocket;
    QString server, appDir;
    quint16 port;


    World *world;

};

#endif // DXCLUSTER_H
