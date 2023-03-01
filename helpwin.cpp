#include "helpwin.h"
#include "ui_helpwin.h"

helpwin::helpwin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::helpwin)
{
    ui->setupUi(this);
}

helpwin::~helpwin()
{
    delete ui;
}
