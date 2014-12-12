#ifndef PARSER_H
#define PARSER_H

#include <cmath>
#include <vector>
#include <string>
#include <stack>
#include <map>

#include "common.h"

struct term_t {
    ld value;
    std::string name;
    bool is_variable;

    term_t() {

    }

    term_t(ld x) {
        value = x;
    }

    term_t(std::string n, bool is_var = false) {
        name = n;
        is_variable = is_var;
    }

    void evaluate(std::stack<ld> &st, ld x_value);
};

typedef std::vector<term_t> expr_t;

ld evaluate(expr_t &e, ld x_value);

expr_t parse(std::string str);

#endif // AST_H
