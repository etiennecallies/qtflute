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

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include "masterthread.h"
#include "comm.h"
#include "partition.h"
#include "Accordeur/accordage.h"
#include "Accordeur/micro.h"
#include "param_lecture.h"

QT_BEGIN_NAMESPACE

class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QComboBox;

QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(Comm & comm, Partition & parti, ParamLecture & pl,  Accordage & ac, ParamAccordage & pac, AnalyseMicro & mic,  QWidget *parent = 0);

    // éléments clés
    Comm & comm;
    Partition & partoche;
    ParamLecture & pl;
    Accordage & ac;
    ParamAccordage & pac;
    AnalyseMicro & mic;

    void prise_controle();
    void importer_accordage();

private slots:
    void lire_partition();
    void accorder_auto();

    void transaction();
    void showResponse(const QString &s,  const QString &t);
    void processError(const QString &s);
    void processTimeout(const QString &s);
    void enableControl();
    void disableControl();

private:
    void setControlsEnabled(bool enable);

private:
    int transactionCount;
    QLabel * serialPortLabel_motor;
    QComboBox *serialPortComboBox_motor;

    QLabel *serialPortLabel_arduino; // new MHX
    QComboBox *serialPortComboBox_arduino;

    QLabel *waitResponseLabel;
    QSpinBox *waitResponseSpinBox;
    QLabel *requestLabel;
    QLineEdit *requestLineEdit;
    QLabel *trafficLabel;
    QLabel *statusLabel;
    QPushButton *runButton;
    QPushButton *accordButton;
    QPushButton *playButton;

    MasterThread thread;
};

#endif // DIALOG_H