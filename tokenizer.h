#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <QString>
#include <vector>
#include <unordered_map>
#include <statements.h>

//将输入框输入的语句/文件读入的语句 转化成 1空格恰当的语句，存储后便于在代码框显示; 2拆分成tokens后存储,便于后续处理

class tokenizer
{
public:
    tokenizer();

    std::unordered_map<std::string, int> variables; //存储变量和值

    //判断语句是否有行号
    bool is_have_line_num(std::string& input_line);
    //如果有行号，提取行号
    int split_line_num(std::string& input_line);
    //将带行号的程序语句中的expression拆分出来（此时参数line已不带行号）
    std::vector<std::string> split_expression(std::string& line);
    Statement state_confirm(std::string& line);
    std::string delete_space(std::string& line);
    //转化成空格恰当的语句，存储后便于在代码框显示（一定有行号，无行号语句不会在代码框显示）
    std::string string_convert_std(std::string& input_line);
    //拆分expreesion
    std::vector<std::string> split_expr_to_tokens(std::string& expr);
    bool is_calu_operator(char& ch);
    bool is_variable_first_char(char& ch);
};

#endif // TOKENIZER_H
