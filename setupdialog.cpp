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

#include "setupdialog.h"


SetupDialog::SetupDialog(const bool _firstTime)
{
    qDebug() << "SetupDialog::SetupDialog 1" << endl;
    dxclusterServersComboBox = new QComboBox;

    addClusterButton = new QPushButton(tr("Add"));
    deleteClusterButton = new QPushButton(tr("Delete"));

    closeButton = new QPushButton(tr("Cancel"));
    okButton = new QPushButton(tr("OK"));

    connect(closeButton, SIGNAL(clicked()), this, SLOT(slotCancelButtonClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(slotOkButtonClicked()));

    firstTime = _firstTime;

    initClass();

}
SetupDialog::~SetupDialog()
{

}

void SetupDialog::initClass()
{


    addDXClusters();

    dxclusterServersComboBox->setToolTip(tr("Select the DX-Cluster server you want to use."));
    addClusterButton->setToolTip(tr("Click to add a new DX-Cluster server."));
    deleteClusterButton->setToolTip(tr("Click to delete the currently selected DX-Cluster server."));
    okButton->setToolTip(tr("Click to accept the setup."));
    closeButton->setToolTip(tr("Click to cancel."));

    QHBoxLayout *serversButtonsLayout = new QHBoxLayout;
    serversButtonsLayout->addSpacerItem(new QSpacerItem(10,0,QSizePolicy::Expanding,QSizePolicy::Maximum));
    serversButtonsLayout->addWidget(addClusterButton);
    serversButtonsLayout->addWidget(deleteClusterButton);

    QVBoxLayout *serversLayout = new QVBoxLayout;
    serversLayout->addWidget(dxclusterServersComboBox);
    serversLayout->addLayout(serversButtonsLayout);

    connect(addClusterButton, SIGNAL(clicked()), this, SLOT(slotAddButtonClicked()) );
    connect(deleteClusterButton, SIGNAL(clicked()), this, SLOT(slotDeleteButtonClicked()) );

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(closeButton);

    QVBoxLayout *setupLayout = new QVBoxLayout;
    setupLayout->addLayout(serversLayout);
    setupLayout->addLayout(buttonsLayout);

    setLayout(setupLayout);
}

void SetupDialog::slotAddButtonClicked()
{
    qDebug() << "SetupDialog::slotAddButtonClicked()" << endl;

}

void SetupDialog::slotDeleteButtonClicked()
{
    qDebug() << "SetupDialog::slotDeleteButtonClicked()" << endl;

}

void SetupDialog::slotOkButtonClicked()
{
    qDebug() << "SetupDialog::slotOkButtonClicked()" << endl;


    QDialog::accept();

}

void SetupDialog::slotCancelButtonClicked()
{
    qDebug() << "SetupDialog::slotCancelButtonClicked()" << endl;
    close();
}

void SetupDialog::addDXClusters()
{

    dxclusterServersComboBox->addItem("dxfun.com:8000");
    dxclusterServersComboBox->addItem("dxspots.com:23");
    dxclusterServersComboBox->addItem("eadx.org:23");
    dxclusterServersComboBox->addItem("k1ttt.net:23");
    dxclusterServersComboBox->addItem("telnet.reversebeacon.net:7300");
    dxclusterServersComboBox->addItem("dx.ea7urc.org:8000");
    dxclusterServersComboBox->addItem("ei7mre.ath.cx:7300");

}
