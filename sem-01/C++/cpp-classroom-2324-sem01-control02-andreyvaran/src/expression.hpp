    #pragma once
    #ifndef EXPRESSIONS_HPP
    #define EXPRESSIONS_HPP

    #include <iostream>
    #include <stdexcept>
    #include <map>
    #include <memory>

    class Context {
    public:
        int64_t getVar(const std::string &name) const;

        void setVar(const std::string &name, int64_t value);

        bool varIsSet(const std::string &name) const;

    private:
        std::map<std::string, int64_t> variables;
    };

    class Expression {
    public:

        virtual void print(std::ostream &os) const = 0;

        virtual int64_t eval() const = 0;

        virtual int64_t eval(const Context &ctx) const = 0;

        virtual ~Expression() = default;

        static void *operator new(size_t size);

        static void operator delete(void *ptr);

        virtual Expression* simplified(const Context &ctx) const = 0;

    };


    class Val : public Expression {
    public:
        Val(int64_t value);

        void print(std::ostream &os) const override;

        int64_t eval() const override;

        int64_t eval(const Context &ctx) const override;

        static void *operator new(size_t size);

        static void operator delete(void *ptr);

        Expression* simplified(const Context &ctx) const override;

    private:
        int64_t value;
    };

    class Var : public Expression {
    public:
        Var(const std::string &name);

        void print(std::ostream &os) const override;

        int64_t eval() const override;

        int64_t eval(const Context &ctx) const override;

        static void *operator new(size_t size);

        static void operator delete(void *ptr);

        Expression* simplified(const Context &ctx) const override;

    private:
        std::string name;
    };

    class Add : public Expression {
    public:
        Add(Expression *left, Expression *right);

        Add(const Add &rhs) = delete;

        Add &operator=(const Add &rhs) = delete;

        void print(std::ostream &os) const override;

        int64_t eval() const override;

        int64_t eval(const Context &ctx) const override;

        ~Add();

        static void *operator new(size_t size);

        static void operator delete(void *ptr);

        Expression* simplified(const Context &ctx) const override;


    private:
        Expression *left;
        Expression *right;
    };

    class Mul : public Expression {
    public:
        Mul(Expression *left, Expression *right);

        Mul(const Mul &rhs) = delete;

        Mul &operator=(const Mul &rhs) = delete;

        void print(std::ostream &os) const override;

        int64_t eval() const override;

        int64_t eval(const Context &ctx) const override;

        ~Mul();

        static void *operator new(size_t size);

        static void operator delete(void *ptr);

        Expression* simplified(const Context &ctx) const override;


    private:
        Expression *left;
        Expression *right;
    };

    #endif
