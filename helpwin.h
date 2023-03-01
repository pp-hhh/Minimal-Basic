#ifndef HELPWIN_H
#define HELPWIN_H

#include <QWidget>
#include <QDialog>

namespace Ui {
class helpwin;
}

class helpwin : public QDialog
{
    Q_OBJECT

public:
    explicit helpwin(QWidget *parent = nullptr);
    ~helpwin();

private:
    Ui::helpwin* ui;
};

#endif // HELPWIN_H
