#include "runner.h"

runner::runner(QObject *parent) : QThread(parent)
{
    line_numbers.clear();
    line_numbers.resize(0);
    split_expreesion_store.clear();
    expressions_store.clear();
}


void runner::parse_line(int line_num){
    Statement cur_state = store_statement_type[line_num];
    std::vector<std::string> line_split = split_expreesion_store[line_num];
    std::vector<std::vector<std::string>> expr_tokens = expressions_store[line_num];
    class exp* node = exp_->parse_line(line_split, expr_tokens, cur_state);
    store_tree[line_num] = node;
}


int runner::string_convert(std::string& input_line){
    //判断语句是否有行号
    bool have_line_num = tokenizer->is_have_line_num(input_line);
    std::string input_without_line_num;
    if(have_line_num){ //有行号的输入
        int line_number = tokenizer->split_line_num(input_line);
        //如果行号之前没出现过，插入行号存储vector
        store_line_number(line_number);

        //split input_line without line_number
        int start_id = input_line.find_first_of(' ', 0) + 1;
        int length = input_line.size() - start_id;
        input_without_line_num = input_line.substr(start_id, length);
        code_display_content[line_number] = input_without_line_num; //存储codedisplay要显示的内容
        Statement state_op = tokenizer->state_confirm(input_without_line_num);
        store_statement_type[line_number] = state_op;
        //split input into tokens
        split_expreesion_store[line_number] = tokenizer->split_expression(input_without_line_num);

        store_expreesions(line_number);

        return line_number;
    }else{ //无行号的输入(let/print/input类型 or RUN/LOAD/LIST/CLEAR/HELP/QUIT or 不合法类型)
        //TODO 判断是否是合法的无行号输入
        Statement state_op = tokenizer->state_confirm(input_line);
        if(state_op >= 8){ //RUN/LOAD/LIST/CLAER/HELP/QUIT
            is_cmd = true;
            return state_op;
        }


        return INT_MIN;
    }
}


void runner::store_line_number(int line_num){
    if(line_numbers.size() == 0){
        line_numbers.push_back(line_num);
        return;
    }
    int left = 0, right = line_numbers.size();
    while(left < right){
        int mid = (right - left) / 2 + left;
        if(line_numbers[mid] < line_num){
            left = mid + 1;
        }else if(line_numbers[mid] > line_num){
            right = mid;
        }else if(line_numbers[mid] == line_num){
            return;
        }
    }
    line_numbers.insert(line_numbers.begin() + left, line_num);
}


QString runner::print_line(int line_num){
    std::string line_content = "";
    line_content += std::to_string(line_num);
    line_content += " ";
    line_content += code_display_content[line_num];
    return QString::fromStdString(line_content);
}


void runner::store_expreesions(int line_num){
    Statement state_op = store_statement_type[line_num];
    switch (state_op) {
    case Let:{
        std::string expr = split_expreesion_store[line_num][2];
        std::vector<std::string> tokens = tokenizer->split_expr_to_tokens(expr);
        expressions_store[line_num].push_back(tokens);
        break;
    }
    case Print:{
        std::string expr = split_expreesion_store[line_num][1];
        std::vector<std::string> tokens = tokenizer->split_expr_to_tokens(expr);
        expressions_store[line_num].push_back(tokens);
        break;
    }
    case If:{
        std::string expr1 = split_expreesion_store[line_num][3], expr2 = split_expreesion_store[line_num][4];
        std::vector<std::string> tokens1 = tokenizer->split_expr_to_tokens(expr1), tokens2 = tokenizer->split_expr_to_tokens(expr2);
        expressions_store[line_num].push_back(tokens1);
        expressions_store[line_num].push_back(tokens2);
        break;
    }
    }
}


Statement runner::get_state_op(int &line_index){
    return store_statement_type[line_numbers[line_index]];
}


void runner::run(){
    int line_index = 0;
    run_program(line_index);
}

void runner::run_program(int &line_index){
    output.clear();
    int n = line_numbers.size();
    while(line_index < n){

        pauseLock.lock();
        if(is_input){
            input_cond.wait(&pauseLock);
        }
        pauseLock.unlock();


        Statement state_op = get_state_op(line_index);
        switch (state_op) {
        case Rem:{
            line_index++;
            continue;
            break;
        }
        case Let:{
            run_let(line_index);
            break;
        }
        case Print:{
            run_print(line_index);
            //ui->textBrowser->append(QString::fromStdString(res));
            break;
        }
        case Input:{
            //TODO
            is_input = true;
            run_input(line_index);
            line_index++;
            break;
        }
        case Goto:{
            run_goto(line_index);
            break;
        }
        case If:{
            run_if(line_index);
            break;
        }
        case End:{
            line_index = n;
            return;
        }
        }
    }
}


void runner::run_let(int &line_index){
    int cur_line_num = line_numbers[line_index];
    //TODO calculate
    int res = exp_->eval_exp(store_tree[cur_line_num]->get_child_x(1));
    std::string variable = split_expreesion_store[cur_line_num][1];
    exp_->store_var(variable, res);
    line_index++;
}

void runner::run_print(int &line_index){
    int cur_line_num = line_numbers[line_index];
    class exp* cur = store_tree[cur_line_num]->get_child_x(0);
    int res = exp_->eval_exp(cur);
    line_index++;
    output.push_back(std::to_string(res));
    emit toPrint();
}

void runner::run_goto(int &line_index){
    int cur_line_num = line_numbers[line_index];
    int line_togo = stoi(split_expreesion_store[cur_line_num][1]);
    int line_togo_index = find_line_num_index(line_togo);
    line_index = line_togo_index;
}

void runner::run_if(int &line_index){
    int cur_line_num = line_numbers[line_index];
    std::vector<std::string> expr1 = expressions_store[cur_line_num][0];
    std::vector<std::string> expr2 = expressions_store[cur_line_num][1];
    int res1 = exp_->eval_exp(store_tree[cur_line_num]->get_child_x(0));
    int res2 = exp_->eval_exp(store_tree[cur_line_num]->get_child_x(2));
    Cmp cmp = string_to_cmp(split_expreesion_store[cur_line_num][2]);
    //verify the condition
    if(is_true(res1, res2, cmp)){
        int line_togo = stoi(split_expreesion_store[cur_line_num][1]);
        int line_togo_index = find_line_num_index(line_togo);
        line_index = line_togo_index;
    }else{
        line_index++;
    }
}

void runner::run_input(int &line_index){
    int cur_line_num = line_numbers[line_index];
    std::string var = split_expreesion_store[cur_line_num][1];
    //TODO print data update
    output.push_back(" ? ");
    input_var = var;
}

Cmp runner::string_to_cmp(std::string &str){
    if(str == "="){
        return Equal;
    }else if(str == ">"){
        return Greater;
    }else if(str == "<"){
        return Less;
    }
}

bool runner::is_true(int num1, int num2, Cmp op){
    switch (op) {
    case Equal:{
        return num1 == num2;
    }
    case Greater:{
        return num1 > num2;
    }
    case Less:{
        return num1 < num2;
    }
    }
}

int runner::find_line_num_index(int &line_num){
    int left = 0, right = line_numbers.size();
    while(left < right){
        int mid = (right - left) / 2 + left;
        if(line_num == line_numbers[mid]){
            return mid;
        }else if(line_numbers[mid] > line_num){
            right = mid;
        }else if(line_numbers[mid] < line_num){
            left = mid + 1;
        }
    }
}

void runner::clear_all(){
    is_input = false;
    output.clear();
    store_tree.clear();
    line_numbers.clear();
    split_expreesion_store.clear();
    store_statement_type.clear();
    expressions_store.clear();
    code_display_content.clear();
    exp_->variable_hash.clear();
    tokenizer->variables.clear();
}
