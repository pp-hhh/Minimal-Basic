#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <runner.h>
#include <QTimer>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <helpwin.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void clearAll();
    void codeLineEdit_return();
    void run_program();
public slots:
    void printRes();
    void load_file();

private:
    Ui::MainWindow *ui;
    runner* runner_;

    QTimer* timer;

    bool isLoading = false;
    QString input;

    void print_code_display();
    void print_tree_display();
    void print_tree_first_line(int line_num, class exp* exp_);
    void print_expreesion_tree(class exp* exp_, int height);

    void process_cmd(Statement cmd);
    Statement get_value(int key);

    helpwin* helpwin_;

    void run(int& line_num);
};
#endif // MAINWINDOW_H
