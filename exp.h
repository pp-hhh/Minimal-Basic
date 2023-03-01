#ifndef EXP_H
#define EXP_H

#include <statements.h>
#include <iostream>
#include <vector>
#include <stack>
#include <cmath>
#include <unordered_map>

/*
 * Class: exp
 * -----------------
 * This class is used to represent a node in an expression tree.
 * Expression itself is an abstract class.  Every Expression object
 * is therefore created using one of the three concrete subclasses:
 * ConstantExp, IdentifierExp, or CompoundExp.
 */


enum Exp_Type{
    exp_var,
    exp_num,
    exp_op,
    exp_state,
    exp_str
};

class exp
{
public:
    exp();


    exp* parse_line(std::vector<std::string>& line_split, std::vector<std::vector<std::string>>& expr_tokens, Statement state);

    std::string get_val();
    exp* get_child_x(int x);

    int eval_exp(exp* root);

    void store_var(std::string var_name, int val);
private:
    int eval_op(int num1, int num2, Operators& op);

private:
    Exp_Type type;
    std::string val;
    exp* child[4];

    exp(Exp_Type t, std:: string& s){
        type = t;
        val = s;
        child[0] = nullptr;
        child[1] = nullptr;
        child[2] = nullptr;
        child[3] = nullptr;
    }

    exp* parse_expr(std::vector<std::string>& expr_tokens);

    Exp_Type type_confirm(std::string& token);

    bool operator_comp(std::string& op1, std::string& op2);
public:
    Operators string_to_op(std::string& op);

    //变量存储
    std::unordered_map<std::string, int> variable_hash;
private:
    void pop_op_process(std::stack<std::string>& operators, std::stack<exp*>& operands);

};

#endif // EXP_H
