#ifndef DIALOG_H
#define DIALOG_H

#include "ui_flutronic.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QString>

#include "comm.h"
#include "partition.h"
#include "Accordeur/accordage.h"
#include "Accordeur/micro.h"
#include "param_lecture.h"

namespace Ui {
    class Flutronic;
}

class Flutronic : public QMainWindow
{
    Q_OBJECT

public:
    Flutronic(ParamLecture & pl,  ParamAccordage & pac, AnalyseMicro & mic, QWidget *parent = 0);

    // éléments clés
    Comm comm;
    Partition partoche;
    ParamLecture & pl;
    Accordage ac;
    ParamAccordage & pac;
    AnalyseMicro & mic;

    void importer_accordage();

private slots:
    void connecter_comm();
    void lire_partition();
    void accorder_auto();
    void changer_params();
    void ouvrir_browser();
    void open_harmony_assistant();
    void charger_partition();

    /*void showResponse(const QString &s,  const QString &t);
    void processError(const QString &s);
    void processTimeout(const QString &s);*/
    void enableControl();
    void disableControl();

protected:
    void setControlsEnabled(bool enable);

protected:
    Ui::Flutronic * ui;
    QString fname_parti;
};

#endif // DIALOG_H
