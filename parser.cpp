#include "parser.h"

term_t next_token(const std::string &str, size_t &cur);

expr_t parse(std::string str) {
    std::map<std::string, bool> rhand;
    rhand["^"] = true;
    rhand["_"] = true;

    std::map<std::string, char> priority;
    priority[""] = 100; // numbers
    priority["x"] = 100; // numbers
    priority["abs"] = 4;
    priority["sqrt"] = 4;
    priority["sin"] = 4;
    priority["cos"] = 4;
    priority["tan"] = 4;
    priority["^"] = 3;
    priority["_"] = 3;
    priority["*"] = 2;
    priority["/"] = 2;
    priority["+"] = 1;
    priority["-"] = 1;

    str = "(" + str + ")";

    expr_t result;

    term_t ctok;

    std::stack<term_t> st;
    size_t cur = 0;
    while (cur < str.length()) {
        ctok = next_token(str, cur);

        if (ctok.name == "(" || ctok.name.empty() || ctok.is_variable) {
            st.push(ctok);
        } else if (ctok.name == ")") {
            while (!st.empty() && st.top().name != "(") {
                result.push_back(st.top());
                st.pop();
            }
            if (!st.empty()) {
                st.pop();
            }
        } else {
            while (true) {
                if (st.empty()) {
                    break;
                }
                if (priority[ctok.name] < priority[st.top().name]) {
                    result.push_back(st.top());
                    st.pop();
                    continue;
                } else if (rhand[ctok.name] && priority[ctok.name] == priority[st.top().name]) {
                    result.push_back(st.top());
                    st.pop();
                    continue;
                }
                break;
            }
            st.push(ctok);
        }
    }
    return result;
}


term_t next_token(const std::string &str, size_t &cur) {
    while (cur < str.size() && str[cur] == ' ') {
        ++cur;
    }
    if (cur >= str.length()) {
        return term_t(0);
    }
    term_t tok;
    bool isnum = false;
    bool cnumdot = false;
    ld cnummul = 1;

    tok.value = 0;
    tok.is_variable = false;
    while (cur < str.length()) {
        if ((str[cur] >= '0' && str[cur] <= '9') || str[cur] == '.') {
            if (!tok.name.empty()) {
                break;
            }
            isnum = true;

            if (str[cur] == '.') {
                cnumdot = true;
            } else if (cnumdot) {
                cnummul /= 10;
                tok.value += cnummul * (str[cur]-'0');
            } else {
                tok.value = tok.value * 10 + str[cur] - '0';
            }
        } else if (isnum) {
            break;
        } else if (!tok.name.empty() && (!isalpha(tok.name[0]) || !isalpha(str[cur]))) {
            break;
        } else {
            tok.name.push_back(str[cur]);
        }
        cur++;
    }
    if (tok.name == "x") {
        tok.is_variable = true;
    }
    while (cur < str.size() && str[cur] == ' ') {
        ++cur;
    }

    return tok;
}


ld evaluate(expr_t &e, ld x_value) {
    std::stack<ld> st;
    for (size_t i = 0; i < e.size(); ++i) {
        e[i].evaluate(st, x_value);
    }
    if (st.empty()) {
        return 0;
    }
    return st.top();
}


void term_t::evaluate(std::stack<ld> &st, ld x_value) {
    if (name.empty()) {
        st.push(value);
    } else if (is_variable) {
        st.push(x_value);
    } else if (name == "+" || name == "-" || name == "*" || name == "/" || name == "^") {
        ld b = st.top();
        st.pop();
        ld a = st.top();
        st.pop();
        ld r = 0;
        switch(name[0]) {
        case '+': r = a+b; break;
        case '-': r = a-b; break;
        case '*': r = a*b; break;
        case '/': r = a/b; break;
        case '^': r = std::pow(a, b); break;
        }

        st.push(r);
    } else if (name == "abs" || name == "sqrt" || name == "tan" || name == "sin" || name == "cos" || name == "_") {
        ld v = st.top();
        st.pop();
        if (name == "abs") {
            st.push(std::abs(v));
        } else if (name == "sqrt") {
            st.push(std::sqrt(v));
        } else if (name == "tan") {
            st.push(std::tan(v));
        } else if (name == "sin") {
            st.push(std::sin(v));
        } else if (name == "cos") {
            st.push(std::cos(v));
        } else if (name == "_") {
            st.push(-v);
        }
    }
}
