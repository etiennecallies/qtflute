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

#include <QApplication>

#include "dialog.h"

#include "accord.h"
#include "partition.h"
#include "param_lecture.h"

#include "comm.h"

#define LONG_TOTALE 170

int main(int argc, char *argv[])
{
    // accordage virtuel
    Corresp c_test;
    for(int i=0; i<7*NBR_OCTAVE; i++)
        c_test[i] = (i+1)*LONG_TOTALE/(7*NBR_OCTAVE); // en mm

    ParamLecture pl; // défaut
        pl.maxVit = 400; // en mm/s
        pl.maxAcc = 600; // en mm/s²
        pl.maxDec = pl.maxAcc;

    Partition p("C:\\Users\\etienne\\Documents\\Mines\\Mecatronique\\qtflute\\linmot\\1oct.abc");
        p.base_temps = 500; // en ms

    qDebug() << "Nbr de notes = " << p.notes.size();

    Comm co("COM9", "COM12", 1000);

    QApplication app(argc, argv);
    Dialog dialog;

    dialog.comm = &co;
    dialog.partoche = &p;
    dialog.pl = &pl;
    dialog.cor = &c_test;

    dialog.show();
    return app.exec();
}
