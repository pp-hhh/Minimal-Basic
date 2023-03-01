#include "exp.h"

exp::exp()
{
    variable_hash.clear();
}

std::string exp::get_val(){
    return val;
}

class exp* exp::get_child_x(int x){
    return child[x];
}


class exp* exp::parse_line(std::vector<std::string> &line_split, std::vector<std::vector<std::string> > &expr_tokens, Statement state){
    exp* cur_exp, *left_child, *right_child;
    switch (state) {
    case Let:{
        std::string str = "LET =";
        cur_exp = new exp(exp_state, str);
        left_child = new exp(exp_var, line_split[1]);
        cur_exp->child[0] = left_child;

        std::vector<std::string> tmp = expr_tokens[0];
        right_child = parse_expr(tmp);
        cur_exp->child[1] = right_child;
        break;
    }
    case Print:{
        std::string str = "PRINT";
        cur_exp = new exp(exp_state, str);
        std::vector<std::string> tmp = expr_tokens[0];
        left_child = parse_expr(tmp);
        cur_exp->child[0] = left_child;
        break;
    }
    case Rem:{
        std::string str = "REM";
        cur_exp = new exp(exp_state, str);
        left_child = new exp(exp_str, line_split[1]);
        cur_exp->child[0] = left_child;
        break;
    }
    case Input:{
        std::string str = "INPUT";
        cur_exp = new exp(exp_state, str);
        left_child = new exp(exp_var, line_split[1]);
        cur_exp->child[0] = left_child;
        break;
    }
    case Goto:{
        std::string str = "GOTO";
        cur_exp = new exp(exp_state, str);
        left_child = new exp(exp_num, line_split[1]);
        cur_exp->child[0] = left_child;
        break;
    }
    case If:{
        std::string str = "IF THEN";
        cur_exp = new exp(exp_state, str);
        left_child = parse_expr(expr_tokens[0]);
        right_child = parse_expr(expr_tokens[1]);
        cur_exp->child[0] = left_child;  //expr1
        cur_exp->child[1] = new exp(exp_op, line_split[2]); //op
        cur_exp->child[2] = right_child; //expr2
        cur_exp->child[3] = new exp(exp_num, line_split[1]); //line_num togo
        break;
    }
    case End:{
        std::string str = "END";
        cur_exp = new exp(exp_state, str);
        break;
    }
    }
    return cur_exp;
}

class exp* exp::parse_expr(std::vector<std::string> &expr_tokens){
    std::stack<std::string> operators;
    std::stack<exp*> operands;
    for(std::string& token : expr_tokens){
        if(token == "("){
            operators.push(token);
        }else if(token == ")"){
            //pop直至遇到（ 为止
            //std::string top = operators.top();
            while(!operators.empty() && operators.top() != "("){
                pop_op_process(operators, operands);
            }
            operators.pop(); //pop "("
        }else{
            Exp_Type cur_type = type_confirm(token);

            if(cur_type == exp_num || cur_type == exp_var){
                exp* cur = new exp(cur_type, token);
                operands.push(cur);
            }else{
                //要考虑运算符优先级的问题
                while(!operators.empty() && operator_comp(token, operators.top())){
                    pop_op_process(operators, operands);
                }

                operators.push(token);
            }
        }
    }
    while(!operators.empty()){
        pop_op_process(operators, operands);
    }
    return operands.top();
}

void exp::pop_op_process(std::stack<std::string> &operators, std::stack<exp *>& operands){
    exp* res = new exp(exp_op, operators.top());
    operators.pop();
    res->child[1] = operands.top();
    operands.pop();
    res->child[0] = operands.top();
    operands.pop();
    operands.push(res);
}

bool exp::operator_comp(std::string &op1, std::string &op2){
    Operators op_1 = string_to_op(op1);
    Operators op_2 = string_to_op(op2);

    if(op_1 == Power){
        return false;
    }else if(op_1 == Multi || op_1 == Divide){
        if(op_2 == Power || op_2 == Multi || op_2 == Divide){
            return true;
        }else{
            return false;
        }
    }else if(op_1 == Add || op_1 == Minus){
        return true;
    }
}

Operators exp::string_to_op(std::string &op){
    if(op == "+"){
        return Add;
    }else if(op == "-"){
        return Minus;
    }else if(op == "*"){
        return Multi;
    }else if(op == "/"){
        return Divide;
    }else if(op == "^"){
        return Power;
    }
}

Exp_Type exp::type_confirm(std::string &token){
    if(isdigit(token[0])) return exp_num;
    if(token == "+" || token == "-" || token == "*" || token == "/" || token == "^") return exp_op;
    return exp_var;
}


int exp::eval_exp(exp *root){
    if(root->type == exp_op){
        int left_res = eval_exp(root->child[0]);
        int right_res = eval_exp(root->child[1]);
        Operators op = string_to_op(root->val);
        int res = eval_op(left_res, right_res, op);
        return res;
    }else if(root->type == exp_num){
        return stoi(root->val);
    }else if(root->type == exp_var){
        return variable_hash[root->val];
    }
}

int exp::eval_op(int num1, int num2, Operators &op){
    switch (op) {
    case Add:{
        return num1 + num2;
    }
    case Minus:{
        return num1 - num2;
    }
    case Multi:{
        return num1 * num2;
    }
    case Divide:{
        return num1 / num2;
    }
    case Power:{
        return pow(num1, num2);
    }
    }
}


void exp::store_var(std::string var_name, int val){
    variable_hash[var_name] = val;
}
