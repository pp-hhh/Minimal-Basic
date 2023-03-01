#include "tokenizer.h"

tokenizer::tokenizer()
{

}


bool tokenizer::is_have_line_num(std::string &input_line){
    if(input_line.size() == 0) return false;

    char first_char = input_line[0];
    return isdigit(first_char);
}


int tokenizer::split_line_num(std::string &input_line){
    int end_index = input_line.find_first_of(' ', 0);
    int line_number = stoi(input_line.substr(0, end_index));
    return line_number;
}


std::vector<std::string> tokenizer::split_expression(std::string &line){
    std::vector<std::string> tokens;
    Statement state_op = state_confirm(line);
    //去除所有空格
    line = delete_space(line);
    int n = line.length();
    switch (state_op) {
    case Rem:{
        tokens.push_back("REM");
        tokens.push_back(line.substr(3, n - 3));
        break;
    }
    case Let:{  //LET, variable_name, expression
        tokens.push_back("LET");
        int equal_index = line.find_first_of("=", 0);
        std::string left_variable = line.substr(3, equal_index - 3);
        tokens.push_back(left_variable);
        std::string expr = line.substr(equal_index + 1, n - equal_index - 1);
        tokens.push_back(expr);
        break;
    }
    case Print:{ //PRINT, expression
        tokens.push_back("PRINT");
        std::string expr = line.substr(5, n - 5);
        tokens.push_back(expr);
        break;
    }
    case Input:{ //INPUT, variable_name
        tokens.push_back("INPUT");
        std::string input_variable = line.substr(5, n - 5);
        tokens.push_back(input_variable);
        break;
    }
    case Goto:{ //GOTO, line_number
        tokens.push_back("GOTO");
        std::string line_togo = line.substr(4, n - 4);
        tokens.push_back(line_togo);
        break;
    }
    case If:{ //IF, line_number, op, expression1, expression2
        tokens.push_back("IF");
        int then_index = line.find("THEN", 0);
        std::string line_togo = line.substr(then_index + 4, n - then_index - 4);
        tokens.push_back(line_togo);
        int op_index = line.find_first_of('=', 0);
        if(op_index == -1){
            op_index = line.find_first_of('<', 0);
            if(op_index != -1){
                tokens.push_back("<");
            }else{
                op_index = line.find_first_of('>', 0);
                tokens.push_back(">");
            }
        }else{
            tokens.push_back("=");
        }
        std::string expr1 = line.substr(2, op_index - 2);
        std:: string expr2 = line.substr(op_index + 1, then_index - op_index - 1);
        tokens.push_back(expr1);
        tokens.push_back(expr2);
        break;
    }
    case End:{ //END
        tokens.push_back("END");
        break;
    }
    }
    return tokens;
}


Statement tokenizer::state_confirm(std::string &line){
    std::string sentinal = line.substr(0, 2);
    if(sentinal == "RE") {
        return Rem;
    }else if(sentinal == "LE"){
        return Let;
    }else if(sentinal == "PR"){
        return Print;
    }else if(sentinal == "IN"){
        return Input;
    }else if(sentinal == "GO"){
        return Goto;
    }else if(sentinal == "IF"){
        return If;
    }else if(sentinal == "EN"){
        return End;
    }else if(sentinal == "RU"){
        return Run;
    }else if(sentinal == "LO"){
        return Load;
    }else if(sentinal == "LI"){
        return List;
    }else if(sentinal == "CL"){
        return Clear;
    }else if(sentinal == "HE"){
        return Help;
    }else if(sentinal == "QU"){
        return Quit;
    }else{
        return Eof;
    }
}


std::string tokenizer::delete_space(std::string &line){
    std::string after_del;
    for(char ch : line){
        if(ch != ' '){
            after_del.push_back(ch);
        }
    }
    return after_del;
}


std::vector<std::string> tokenizer::split_expr_to_tokens(std::string &expr){
    std::vector<std::string> tokens;
    int p = 0, n = expr.length();
    while(p < n){
        char cur = expr[p];
        ++p;
        if(is_calu_operator(cur)){ //+-*/^
            std::string op;
            op.push_back(cur);
            tokens.push_back(op);
        }else if(is_variable_first_char(cur)){ //variable
            std::string variable_name;
            variable_name.push_back(cur);
            while(p < n && !is_calu_operator(expr[p]) && expr[p] != '(' && expr[p] != ')'){
                variable_name.push_back(expr[p]);
                ++p;
            }
            tokens.push_back(variable_name);
        }else if(isdigit(cur)){ //num
            std::string num;
            num.push_back(cur);
            while(p < n && isdigit(expr[p])){
                num.push_back(expr[p]);
                ++p;
            }
            tokens.push_back(num);
        }else if(cur == '('){
            tokens.push_back("(");
        }else if(cur == ')'){
            tokens.push_back(")");
        }else{
            //invalid;
        }
    }
    int total_n = tokens.size();
    for(int i = 0; i < total_n; ++i){
        if(tokens[i] == "-" && (i == 0 || tokens[i - 1] == "(")){
            tokens.insert(tokens.begin() + i, "0");
        }
    }
    return tokens;
}


bool tokenizer::is_calu_operator(char &ch){
    return (ch == '+') || (ch == '-') || (ch == '*') || (ch == '/') || (ch == '^');
}


bool tokenizer::is_variable_first_char(char &ch){
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_');
}
