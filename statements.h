#ifndef STATEMENTS_H
#define STATEMENTS_H

//declare & implement statements
#include <vector>
#include <string>

enum Statement{
    Rem,
    Let,
    Print,
    Input,
    Goto,
    If,
    Then,
    End,

    Run,
    Load,
    List,
    Clear,
    Help,
    Quit,

    Condition,
    Number,
    Other,
    Eof
};

enum Operators{
    Numbers,
    Var,

    Add,
    Minus,

    Multi,
    Divide,

    Power,

    Paren_start,
    Paren_end,
    Eoe
};

enum Cmp{
    Greater,
    Less,
    Equal
};

class statements
{
public:
    statements();

};

#endif // STATEMENTS_H
