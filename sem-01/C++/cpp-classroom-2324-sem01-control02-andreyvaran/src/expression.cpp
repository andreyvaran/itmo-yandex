#include "expression.hpp"


void Context::setVar(const std::string &name, int64_t value) {
    variables[name] = value;
}

int64_t Context::getVar(const std::string &name) const {
    if (variables.find(name) != variables.end()) {
        return variables.at(name);
    }
    throw std::runtime_error("Variable not found");
}

bool Context::varIsSet(const std::string &name) const {
    return variables.find(name) != variables.end();
}

Var::Var(const std::string &name) : name(name) {}

void Var::print(std::ostream &os) const {
    os << name;
}

int64_t Var::eval() const {
    throw std::runtime_error("Variable not found in context");
}

int64_t Var::eval(const Context &ctx) const {
    if (ctx.varIsSet(name)) {
        return ctx.getVar(name);
    } else {
        throw std::runtime_error("Variable not found in context");
    }
}

Expression* Var::simplified(const Context& ctx) const {
    if (ctx.varIsSet(name)) {
        return new Val(ctx.getVar(name));
    } else {
        return new Var(name);
    }
}


Val::Val(int64_t value) : value(value) {}

void Val::print(std::ostream &os) const {
    os << value;
}

int64_t Val::eval() const {
    return value;
}

int64_t Val::eval(const Context &ctx) const {
    return value;
}

Expression* Val::simplified(const Context& ctx) const {
    return new Val(value);
}

Add::Add(Expression *left, Expression *right) : left(left), right(right) {}

void Add::print(std::ostream &os) const {
    os << "(";
    left->print(os);
    os << " + ";
    right->print(os);
    os << ")";
}

int64_t Add::eval() const {
    return left->eval() + right->eval();
}

int64_t Add::eval(const Context &ctx) const {
    return left->eval(ctx) + right->eval(ctx);
}

Add::~Add() {
    delete left;
    delete right;
}

Expression* Add::simplified(const Context& ctx) const {
    Expression* left_simplified = left->simplified(ctx);
    Expression* right_simplified = right->simplified(ctx);

    if (dynamic_cast<Val*>(left_simplified) && dynamic_cast<Val*>(right_simplified)) {
        int64_t result = left_simplified->eval() + right_simplified->eval();
        delete left_simplified;
        delete right_simplified;
        return new Val(result);
    }

    return new Add(left_simplified, right_simplified);
}

Mul::Mul(Expression *left, Expression *right) : left(left), right(right) {}

void Mul::print(std::ostream &os) const {
    left->print(os);
    os << " * ";
    right->print(os);
}

int64_t Mul::eval() const {
    return left->eval() * right->eval();
}


int64_t Mul::eval(const Context &ctx) const {
    return left->eval(ctx) * right->eval(ctx);
}

Mul::~Mul() {
    delete left;
    delete right;
}
Expression* Mul::simplified(const Context& ctx) const {
    Expression* left_simplified = left->simplified(ctx);
    Expression* right_simplified = right->simplified(ctx);

    if (dynamic_cast<Val*>(left_simplified) && dynamic_cast<Val*>(right_simplified)) {
        int64_t result = left_simplified->eval() * right_simplified->eval();
        delete left_simplified;
        delete right_simplified;
        return new Val(result);
    }

    return new Mul(left_simplified, right_simplified);
}
