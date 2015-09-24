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

#include "flutronic.h"
#include <QProcess>
#include <iostream>

QT_USE_NAMESPACE

//Flutronic::Flutronic(QWidget *parent)
Flutronic::Flutronic(ParamLecture & Tpl, ParamAccordage & Tpac, AnalyseMicro & Tmic,  QWidget * parent)
    :  QMainWindow(parent)
     , ui(new Ui::Flutronic)

      // éléments clés
    , pl(Tpl)
    , pac(Tpac), mic(Tmic)
      // fin
{
    ui->setupUi(this);

    //Ajout de l'icone
    QPixmap logo("logo.png");
    ui->logoFlutronic->setPixmap(logo);

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->port_moteur->addItem(info.portName());
        ui->port_arduino->addItem(info.portName());
    }
    ui->port_moteur->setCurrentText(DEF_PORT_MOTEUR);
    ui->port_arduino->setCurrentText(DEF_PORT_ARDUINO);

    connect(ui->connectButton, SIGNAL(clicked()),
            this, SLOT(connecter_comm()));
    connect(ui->accordButton, SIGNAL(clicked()),
            this, SLOT(accorder_auto()));
    connect(ui->playButton, SIGNAL(clicked()),
            this, SLOT(lire_partition()));
    connect(ui->parti_parcourir, SIGNAL(clicked()),
            this, SLOT(ouvrir_browser()));
    connect(ui->fich_partition, SIGNAL(textChanged(QString)), //textChanged(QString)
            this, SLOT(charger_partition()));

    connect(ui->tpsBase, SIGNAL(valueChanged(int)),
            this, SLOT(changer_params()));
    connect(ui->harmonyAssistant, SIGNAL(clicked()),
            this, SLOT(open_harmony_assistant()));

    // cas défaut
    ui->playButton->setEnabled(false);
    ui->fich_partition->setEnabled(false);
    ui->parti_parcourir->setEnabled(false);

/*
    connect(&comm, SIGNAL(enable()),
            this, SLOT(enableControl()));
    connect(&comm, SIGNAL(disable()),
            this, SLOT(disableControl()));*/
}

void Flutronic::ouvrir_browser()
{
    QString fname = QFileDialog::getOpenFileName(this, "Choississez votre fichier partition", "../abc", "*.abc");
    ui->fich_partition->setText(fname);
}

void Flutronic::connecter_comm()
{
    comm.connect(ui->port_moteur->currentText(), ui->port_arduino->currentText());
    comm.init();
    qDebug() << "Communications prêtes : " << comm.pret;

    if(comm.pret){
        ui->fich_partition->setEnabled(true);
        ui->parti_parcourir->setEnabled(true);
    }
}

void Flutronic::charger_partition()
{
    QString fname = ui->fich_partition->text();
    qDebug() << "charger parti";
    if(!fname.isEmpty()){
        partoche.charger_fichier(fname.toStdString());

        if(comm.pret) {
            partoche.precharger_premiere_note(comm,ac);
            ui->playButton->setEnabled(true);
        }
    }
}

void Flutronic::changer_params()
{
    pl.tempsBase = 60000 / ui->tpsBase->value();
}

void Flutronic::accorder_auto()
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

void Flutronic::importer_accordage()
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

void Flutronic::lire_partition()
{
    // si table d'accordage non vide
    if(!ac.empty() && comm.pret)
        partoche.lire(comm, ac, pl);
    else
        qDebug() << "Aucune note dans la table d'accordage...";
}

void Flutronic::enableControl()
{
    setControlsEnabled(true);
}
void Flutronic::disableControl()
{
    setControlsEnabled(false);
}

void Flutronic::open_harmony_assistant()
{
//    QProcess * process = new QProcess(this);
//    process->start("C:\\Program Files (x86)\\Harmony Assistant\\harmony.exe");
    system("\"C:\\Program Files (x86)\\Harmony Assistant\\harmony.exe\"");
//    QProcess::execute("start \"C:\\Program Files (x86)\\Harmony Assistant\\harmony.exe\"");
    std::cout << "hello harmony" << std::endl;
}

void Flutronic::setControlsEnabled(bool enable)
{
/*    runButton->setEnabled(enable);
    playButton->setEnabled(enable);
    accordButton->setEnabled(enable);
    serialPortComboBox_motor->setEnabled(enable);
    serialPortComboBox_arduino->setEnabled(enable);
    waitResponseSpinBox->setEnabled(enable);
    requestLineEdit->setEnabled(enable);*/
}
