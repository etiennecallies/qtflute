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
Dialog::Dialog(Comm & Tco, Partition & Tparti, ParamLecture & Tpl,  Accordage & Tac, ParamAccordage & Tpac, AnalyseMicro & Tmic,  QWidget *parent)
    : QDialog(parent)

      // éléments clés
    , comm(Tco)
    , partoche(Tparti), pl(Tpl)
    , ac(Tac), pac(Tpac), mic(Tmic)
      // fin

    , transactionCount(0)
    , serialPortLabel_motor(new QLabel(tr("Motor port:")))
    , serialPortComboBox_motor(new QComboBox())
    , serialPortLabel_arduino(new QLabel(tr("Arduino port:"))) // new MHX
    , serialPortComboBox_arduino(new QComboBox())
    , waitResponseLabel(new QLabel(tr("Wait response, msec:")))
    , waitResponseSpinBox(new QSpinBox())
    , requestLabel(new QLabel(tr("Request:")))
    , requestLineEdit(new QLineEdit(tr("start")))
    , trafficLabel(new QLabel(tr("No traffic.")))
    , statusLabel(new QLabel(tr("Status: Not running.")))
    , runButton(new QPushButton(tr("Exec")))
    , accordButton(new QPushButton(tr("Accorder")))
    , playButton(new QPushButton(tr("Jouer")))

{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        serialPortComboBox_motor->addItem(info.portName());
        serialPortComboBox_arduino->addItem(info.portName());
    }
        serialPortComboBox_motor->setCurrentText(DEF_PORT_MOTEUR);
        serialPortComboBox_arduino->setCurrentText(DEF_PORT_ARDUINO);

    waitResponseSpinBox->setRange(0, 10000);
    waitResponseSpinBox->setValue(1000);

    int row = 0;

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(requestLabel, row, 0);
    mainLayout->addWidget(requestLineEdit, row, 1, 1, 2);

    mainLayout->addWidget(waitResponseLabel, ++row, 0);
    mainLayout->addWidget(waitResponseSpinBox, row, 1);

    mainLayout->addWidget(serialPortLabel_motor, ++row, 0);
    mainLayout->addWidget(serialPortComboBox_motor, row, 1);

    mainLayout->addWidget(serialPortLabel_arduino, ++row, 0);
    mainLayout->addWidget(serialPortComboBox_arduino, row, 1);

    mainLayout->addWidget(trafficLabel, ++row, 0, 1, 4);
    mainLayout->addWidget(statusLabel, ++row, 0, 1, 5);

    mainLayout->addWidget(runButton, 0, 3, 2, 1);
    mainLayout->addWidget(accordButton, 1, 3, 2, 1);
    mainLayout->addWidget(playButton, 2, 3, 2, 1);

    setLayout(mainLayout);

    setWindowTitle(tr("Flutronic"));
    requestLineEdit->setFocus();

    connect(accordButton, SIGNAL(clicked()),
            this, SLOT(accorder_auto()));
    connect(playButton, SIGNAL(clicked()),
            this, SLOT(lire_partition()));
    connect(runButton, SIGNAL(clicked()),
            this, SLOT(transaction()));

    connect(&thread, SIGNAL(response(QString, QString)),
            this, SLOT(showResponse(QString, QString)));
    connect(&thread, SIGNAL(error(QString)),
            this, SLOT(processError(QString)));
    connect(&thread, SIGNAL(timeout(QString)),
            this, SLOT(processTimeout(QString)));
    connect(&thread, SIGNAL(enable()),
            this, SLOT(enableControl()));
    connect(&thread, SIGNAL(disable()),
            this, SLOT(disableControl()));
}

void Dialog::transaction()
{
//    setControlsEnabled(false);
    statusLabel->setText(tr("Status: Running, connected to ports %1 and %2.")
                         .arg(serialPortComboBox_motor->currentText(), serialPortComboBox_arduino->currentText()));
    thread.transaction(serialPortComboBox_motor->currentText(),
                       serialPortComboBox_arduino->currentText(), // new MHX
                       waitResponseSpinBox->value(),
                       requestLineEdit->text());
}

// MHX: temp debug masterthread....
// comm prend les commandes des ports séries
void Dialog::prise_controle()
{
    thread.serial[0].close();
    thread.serial[1].close();
    comm.connect();
}

void Dialog::accorder_auto()
{
    //if(ac != NULL && comm != NULL && mic != NULL && tfreq != NULL && pacc != NULL)
        prise_controle();
        ac.trouver_notes(comm, mic, pac);
        comm.deconnect();

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

void Dialog::importer_accordage()
{
    QFile tab_acc(TABLE_ACCORD_FICH);
    if(tab_acc.exists()) {
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
    //if(partoche != NULL && ac != NULL && pl != NULL && comm != NULL){

    // si table d'accordage non vide
    if(!ac.empty()) {
        prise_controle();
        partoche.lire(comm, ac, pl);
        comm.deconnect();
    }
    //}
}

void Dialog::enableControl()
{
    setControlsEnabled(true);
}

void Dialog::disableControl()
{
    setControlsEnabled(false);
}

void Dialog::showResponse(const QString &s, const QString &t)
{
//    setControlsEnabled(true);
    trafficLabel->setText(tr("Traffic, transaction #%1:"
                             "\n\r-request: %2"
                             "\n\r-response: %3")
                          .arg(++transactionCount).arg(s).arg(t));
}

void Dialog::processError(const QString &s)
{
//    setControlsEnabled(true);
    statusLabel->setText(tr("Status: Not running, %1.").arg(s));
    trafficLabel->setText(tr("No traffic."));
}

void Dialog::processTimeout(const QString &s)
{
//    setControlsEnabled(true);
    statusLabel->setText(tr("Status: Running, %1.").arg(s));
    trafficLabel->setText(tr("No traffic."));
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
