#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H
/***************************************************************************
    setupdialog.h  -  description
       -------------------
begin                : jan 2015
copyright            : (C) 2015 by Jaime Robles
email                : jaime@robles.es
***************************************************************************/

/*****************************************************************************
* This file is part of MonDX.                                               *
*                                                                           *
*    MonDx is free software: you can redistribute it and/or modify          *
*    it under the terms of the GNU General Public License as published by   *
*    the Free Software Foundation, either version 3 of the License, or      *
*    (at your option) any later version.                                    *
*                                                                           *
*    MonDX is distributed in the hope that it will be useful,               *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*    GNU General Public License for more details.                           *
*                                                                           *
*    You should have received a copy of the GNU General Public License      *
*    along with MonDX.  If not, see <http://www.gnu.org/licenses/>.         *
*                                                                           *
*****************************************************************************/
#include <QDialog>
#include <QtWidgets>

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    SetupDialog(const bool _firstTime=true);
    //SetupDialog(const QString _configFile, const QString _softwareVersion, const int _page=0, const bool _firstTime = true);
    ~SetupDialog();


public slots:

signals:
   // void exitSignal(const int status); // 1 = OK, -1 = NOK, 2 = Cancel clicked

private slots:
    //void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    //void slotReadConfigData();
    void slotOkButtonClicked();
    void slotCancelButtonClicked();

    void slotAddButtonClicked();
    void slotDeleteButtonClicked();

private:
    void initClass();
    void addDXClusters();

    bool firstTime;



    QComboBox *dxclusterServersComboBox;
    QPushButton *addClusterButton;
    QPushButton *deleteClusterButton;
    QPushButton *okButton;
    QPushButton *closeButton;

};

#endif // SETUPDIALOG_H
