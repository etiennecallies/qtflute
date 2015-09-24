/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "dialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QGridLayout>

#include <QtSerialPort/QSerialPortInfo>

QT_USE_NAMESPACE

//Dialog::Dialog(QWidget *parent)
Dialog::Dialog(ParamLecture & Tpl, ParamAccordage & Tpac, AnalyseMicro & Tmic,  QWidget * parent)
    : QDialog(parent)
      ui(new IHM::Flutronic)
{

      // éléments clés
    , pl(Tpl)
    , pac(Tpac), mic(Tmic)
      // fin

    , serialPortLabel_motor(new QLabel(tr("Motor port:")))
    , serialPortComboBox_motor(new QComboBox())
    , serialPortLabel_arduino(new QLabel(tr("Arduino port:"))) // new MHX
    , serialPortComboBox_arduino(new QComboBox())
    , connectButton(new QPushButton(tr("OK")))
    , accordButton(new QPushButton(tr("Accorder")))
    , playButton(new QPushButton(tr("Jouer")))

{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        serialPortComboBox_motor->addItem(info.portName());
        serialPortComboBox_arduino->addItem(info.portName());
    }
    serialPortComboBox_motor->setCurrentText(DEF_PORT_MOTEUR);
    serialPortComboBox_arduino->setCurrentText(DEF_PORT_ARDUINO);

    int row = 0;

    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->addWidget(serialPortLabel_motor, ++row, 0);
    mainLayout->addWidget(serialPortComboBox_motor, row, 1);

    mainLayout->addWidget(serialPortLabel_arduino, ++row, 0);
    mainLayout->addWidget(serialPortComboBox_arduino, row, 1);

    mainLayout->addWidget(connectButton, 0, 3, 2, 1);
    mainLayout->addWidget(accordButton, 1, 3, 2, 1);
    mainLayout->addWidget(playButton, 2, 3, 2, 1);

    setLayout(mainLayout);

    setWindowTitle(tr("Flutronic"));
    requestLineEdit->setFocus();

    connect(connectButton, SIGNAL(clicked()),
            this, SLOT(connecter_comm()));
    connect(accordButton, SIGNAL(clicked()),
            this, SLOT(accorder_auto()));
    connect(playButton, SIGNAL(clicked()),
            this, SLOT(lire_partition()));

    connect(&comm, SIGNAL(enable()),
            this, SLOT(enableControl()));
    connect(&comm, SIGNAL(disable()),
            this, SLOT(disableControl()));
}

void Dialog::connecter_comm()
{
    comm.connect(serialPortComboBox_motor->currentText(), serialPortComboBox_arduino->currentText());
    QTest::qWait(100);

    comm.init();
    qDebug() << "Communications prêtes : " << comm.pret;
}

void Dialog::charger_partition()
{
    partoche.charger_fichier(ui->lineEdit->text());

    if(comm.pret)
        partoche.precharger_premiere_note(comm,ac);
}

void Dialog::changer_params()
{
    pl.tempsBase = .value();
    pl.fracTempsTransi = .value();
}

void Dialog::accorder_auto()
{
    //if(ac != NULL && comm != NULL && mic != NULL && tfreq != NULL && pacc != NULL)
    if(comm.pret)
    {
        ac.trouver_notes(comm, mic, pac);

        // export des notes enregistrées
        QFile tab_acc(TABLE_ACCORD_FICH);
        tab_acc.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        if(tab_acc.isWritable())
        {
            QTextStream s(&tab_acc);
            qDebug() << "export des " << ac.exporter_table(s) << " notes.";
            tab_acc.close();
        }
    }
}

void Dialog::importer_accordage()
{
    QFile tab_acc(TABLE_ACCORD_FICH);
    if(tab_acc.exists())
    {
        tab_acc.open(QIODevice::ReadOnly | QIODevice::Text);

        if(tab_acc.isReadable())
        {
            QTextStream s(&tab_acc);
            qDebug() << "import des " << ac.importer_table(s) << " notes.";
            tab_acc.close();
        }
    }
}

void Dialog::lire_partition()
{
    // si table d'accordage non vide
    if(!ac.empty() && comm.pret)
        partoche.lire(comm, ac, pl);
    else
        qDebug() << "Aucune note dans la table d'accordage...";
}

void Dialog::enableControl()
{
    setControlsEnabled(true);
}
void Dialog::disableControl()
{
    setControlsEnabled(false);
}

void Dialog::setControlsEnabled(bool enable)
{
    runButton->setEnabled(enable);
    playButton->setEnabled(enable);
    accordButton->setEnabled(enable);
    serialPortComboBox_motor->setEnabled(enable);
    serialPortComboBox_arduino->setEnabled(enable);
    waitResponseSpinBox->setEnabled(enable);
    requestLineEdit->setEnabled(enable);
}
