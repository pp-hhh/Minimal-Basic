#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->btnClearCode, SIGNAL(clicked()), this, SLOT(clearAll()));
    connect(ui->cmdLineEdit, SIGNAL(returnPressed()), this, SLOT(codeLineEdit_return()));
    connect(ui->btnRunCode, SIGNAL(clicked()), this, SLOT(run_program()));
    connect(ui->cmdLineEdit, SIGNAL(returnPressed()), this, SLOT(printRes()));
    connect(ui->btnLoadCode, SIGNAL(clicked()), this, SLOT(load_file()));

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(printRes()));
    timer->start(100);

    runner_ = new class runner();

    helpwin_ = new helpwin();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load_file(){
    QString fileName = QFileDialog::getOpenFileName(this, "load");
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Failed to open file: " + file.errorString());
        return;
    }
    isLoading = true;
    QTextStream stream(&file);
    while(!stream.atEnd()){
        QString line = stream.readLine();
        input = line;
        codeLineEdit_return();
    }
    isLoading = false;
}

void MainWindow::printRes(){
    if(!runner_->output.empty()){
        ui->textBrowser->clear();
        for(std::string s : runner_->output){
            if(s == " ? "){ //waiting for input
                if(runner_->output.size() == 1){
                    ui->textBrowser->insertPlainText(" ? ");
                }else{
                    ui->textBrowser->insertPlainText("\n ? ");
                }
                break;
            }else{
                ui->textBrowser->append(QString::fromStdString(s));
            }
        }
    }
}

//清除 ui:代码框&结果框&语法树框; 内部数据存储 的所有内容
void MainWindow::clearAll(){
    ui->textBrowser->clear();
    ui->CodeDisplay->clear();
    ui->treeDisplay->clear();
    //TODO 清除内部数据存储的内容
    runner_->clear_all();
}

//当输入框检测到回车 将此时输入框的内容: 显示到代码框并清空输入框; 传给xxx存储&处理语句
void MainWindow::codeLineEdit_return(){
    try {
        if(!isLoading){
            input = ui->cmdLineEdit->text();
        }

        std::string input_line = input.toStdString();
        ui->cmdLineEdit->clear();
        if(runner_->is_input){
            runner_->output.pop_back();
            runner_->output.push_back(" ? " + input_line);
            printRes();

            runner_->exp_->store_var(runner_->input_var, stoi(input_line));

            runner_->is_input = false;
            runner_->resume();
        }else{
            int line_num = runner_->string_convert(input_line);
            if(runner_->is_cmd){
                process_cmd(get_value(line_num));
            }else if(line_num != INT_MIN){
                runner_->parse_line(line_num);
                print_code_display();
            }
        }
    } catch (QString error) {

    }
}

Statement MainWindow::get_value(int key){
    switch (key) {
    case 8:{
        return Run;
    }
    case 9:{
        return Load;
    }
    case 10:{
        return List;
    }
    case 11:{
        return Clear;
    }
    case 12:{
        return Help;
    }
    case 13:{
        return Quit;
    }
    }
}

void MainWindow::process_cmd(Statement cmd){
    switch (cmd) {
    case Run:{
        run_program();
        break;
    }
    case Load:{
        load_file();
        break;
    }
    case Clear:{
        clearAll();
        break;
    }
    case Help:{
        helpwin_->show();
        break;
    }
    case Quit:{
        exit(0);
        break;
    }
    }
}

void MainWindow::print_code_display(){
    ui->CodeDisplay->clear();
    for(int line_num : runner_->line_numbers){
        ui->CodeDisplay->append(runner_->print_line(line_num));
    }
}

void MainWindow::print_tree_first_line(int line_num, class exp *exp_){
    std::string content = std::to_string(line_num) + " " + exp_->get_val();
    ui->treeDisplay->append(QString::fromStdString(content));
}

void MainWindow::print_tree_display(){
    ui->treeDisplay->clear();
    for(int line_num : runner_->line_numbers){
        Statement state_op = runner_->store_statement_type[line_num];
        class exp* cur = runner_->store_tree[line_num];
        switch (state_op) {
        case Rem:{
            print_tree_first_line(line_num, cur);
            ui->treeDisplay->append(QString::fromStdString(std::string(4, ' ') + cur->get_child_x(0)->get_val()));
            break;
        }
        case Let:{
            print_tree_first_line(line_num, cur);
            //print variable
            ui->treeDisplay->append(QString::fromStdString(std::string(4, ' ') + cur->get_child_x(0)->get_val()));
            //TODO print expression
            print_expreesion_tree(cur->get_child_x(1), 1);
            break;
        }
        case Print:{
            print_tree_first_line(line_num, cur);
            //TODO print expression;
            print_expreesion_tree(cur->get_child_x(0), 1);
            break;
        }
        case Input:{
            print_tree_first_line(line_num, cur);
            ui->treeDisplay->append(QString::fromStdString(std::string(4, ' ') + cur->get_child_x(0)->get_val()));
            break;
        }
        case Goto:{
            print_tree_first_line(line_num, cur);
            ui->treeDisplay->append(QString::fromStdString(std::string(4, ' ') + cur->get_child_x(0)->get_val()));
            break;
        }
        case If:{
            print_tree_first_line(line_num, cur);
            //TODO print expression1
            print_expreesion_tree(cur->get_child_x(0), 1);
            //print op
            ui->treeDisplay->append(QString::fromStdString(std::string(4, ' ') + cur->get_child_x(1)->get_val()));
            //TODO print expreesion2
            print_expreesion_tree(cur->get_child_x(2), 1);
            //print line_num
            ui->treeDisplay->append(QString::fromStdString(std::string(4, ' ') + cur->get_child_x(3)->get_val()));
            break;
        }
        case End:{
            print_tree_first_line(line_num, cur);
            break;
        }
        }
    }
}

void MainWindow::print_expreesion_tree(class exp *exp_, int height){
    if(!exp_) return;

    std::string cur = std::string(height*4, ' ') + exp_->get_val();
    ui->treeDisplay->append(QString::fromStdString(cur));

    print_expreesion_tree(exp_->get_child_x(0), height + 1);
    print_expreesion_tree(exp_->get_child_x(1), height + 1);
}

void MainWindow::run_program(){
    print_tree_display();
    runner_->start();
}
