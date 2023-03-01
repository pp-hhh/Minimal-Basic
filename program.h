#ifndef PROGRAM_H
#define PROGRAM_H

#include <QString>
#include <QObject>
#include <vector>
#include <unordered_map>
#include <tokenizer.h>
#include <exp.h>
#include <statements.h>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

//declare and implement a stored program

class program : public QThread
{
    Q_OBJECT
signals:
    void toPrint();

public:
    void pause();
    void resume();


    virtual void run();

    QMutex pauseLock;
    QWaitCondition input_cond;

public:

    program();
    //处理输入的语句（tokenizer）
    tokenizer* tokenizer;
    int string_convert(std::string& input_line);

    //run program
    void run_program(int& line_index);

    //void run_prog(int& line_index);
    void run_let(int& line_index);
    void run_print(int& line_index);
    void run_goto(int& line_index);
    void run_if(int& line_index);

    bool is_input = false;
    int input_line_index;
    void run_input(int& line_index);

    std::vector<std::string> output; //存储输出框的内容

private:
    bool is_true(int num1, int num2, Cmp op);
    Cmp string_to_cmp(std::string& str);
    //print tree

public:
    //parser
    void parse_line(int line_num);
    class exp* exp_ = new class exp();
public:
    std::unordered_map<int, class exp*> store_tree; //存储语法树

    std::vector<int> line_numbers; //存储行号（按从小到大的顺序）
    Statement get_state_op(int& line_index);

private:
    void store_line_number(int line_num);
    std::unordered_map<int, std::vector<std::string>> split_expreesion_store; //存储程序语句（分离expression的程度）

public:
    std::unordered_map<int, Statement> store_statement_type; //存储有行号的程序语句的类型

private:


    void store_expreesions(int line_num);
    std::unordered_map<int, std::vector<std::vector<std::string>>> expressions_store; //存储程序语句的expreesion（expression已切割）

    std::unordered_map<int, std::string> code_display_content; //存储在代码框显示的内容，key = linenumber， value = content
public:
    QString print_line(int line_num); //打印某行语句
private:
    int find_line_num_index(int& line_num);
};

#endif // PROGRAM_H
