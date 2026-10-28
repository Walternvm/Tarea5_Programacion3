#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <cmath>
#include <stdexcept>
using namespace std;

class Expression {
public:
    virtual double evaluate(double x) = 0;
    virtual shared_ptr<Expression> derivative() = 0;
    virtual string toString() = 0;
    virtual bool isConstant() {
        return false;
    }
    virtual ~Expression() = default;
};

class Constant: public Expression {
    double valor;
public:
    Constant(double valor): valor(valor) {}

    double evaluate(double x) override {
        return valor;
    }

    shared_ptr<Expression> derivative() override {
        return make_shared<Constant>(0);
    }

    string toString() override {
        ostringstream os;
        os << valor;
        return os.str();
    }

    bool isConstant() override {
        return true;
    }
};

class Variable: public Expression {
public:
    double evaluate(double x) override {
        return x;
    }

    shared_ptr<Expression> derivative() override {
        return make_shared<Constant>(1);
    }

    string toString() override {
        return "x";
    }
};

class BinaryOperation: public Expression {
protected:
    shared_ptr<Expression> izq;
    shared_ptr<Expression> der;
public:
    BinaryOperation(shared_ptr<Expression> izq, shared_ptr<Expression> der): izq(izq), der(der) {
        if (izq == nullptr || der == nullptr)
            throw invalid_argument("Operando nulo");
    }

    bool isConstant() override {
        return izq->isConstant() && der->isConstant();
    }
};

class UnaryFunction: public Expression {
protected:
    shared_ptr<Expression> operando;
public:
    UnaryFunction(shared_ptr<Expression> operando): operando(operando) {
        if (operando == nullptr)
            throw invalid_argument("Operando nulo");
    }

    bool isConstant() override {
        return operando->isConstant();
    }
};

class Add: public BinaryOperation {
public:
    Add(shared_ptr<Expression> izq, shared_ptr<Expression> der): BinaryOperation(izq, der) {}

    double evaluate(double x) override {
        return izq->evaluate(x) + der->evaluate(x);
    }

    shared_ptr<Expression> derivative() override {
        return make_shared<Add>(
            izq->derivative(),
            der->derivative()
        );
    }

    string toString() override {
        return "(" + izq->toString() + " + " + der->toString() + ")";
    }
};

class Multiply: public BinaryOperation {
public:
    Multiply(shared_ptr<Expression> izq, shared_ptr<Expression> der): BinaryOperation(izq, der) {}

    double evaluate(double x) override {
        return izq->evaluate(x) * der->evaluate(x);
    }

    shared_ptr<Expression> derivative() override {
        return make_shared<Add>(
            make_shared<Multiply>(izq->derivative(), der),
            make_shared<Multiply>(izq, der->derivative())
        );
    }

    string toString() override {
        return "(" + izq->toString() + " * " + der->toString() + ")";
    }
};

class Power: public BinaryOperation {
public:
    Power(shared_ptr<Expression> izq, shared_ptr<Expression> der): BinaryOperation(izq, der) {}

    double evaluate(double x) override {
        return pow(izq->evaluate(x), der->evaluate(x));
    }

    shared_ptr<Expression> derivative() override;

    string toString() override {
        return "(" + izq->toString() + " ^ " + der->toString() + ")";
    }
};

class Sin: public UnaryFunction {
public:
    Sin(shared_ptr<Expression> operando): UnaryFunction(operando) {}

    double evaluate(double x) override {
        return sin(operando->evaluate(x));
    }

    shared_ptr<Expression> derivative() override;

    string toString() override {
        return "sin(" + operando->toString() + ")";
    }
};

class Cos: public UnaryFunction {
public:
    Cos(shared_ptr<Expression> operando): UnaryFunction(operando) {}

    double evaluate(double x) override {
        return cos(operando->evaluate(x));
    }

    shared_ptr<Expression> derivative() override {
        return make_shared<Multiply>(
            make_shared<Constant>(-1),
            make_shared<Multiply>(
                make_shared<Sin>(operando),
                operando->derivative()
            )
        );
    }

    string toString() override {
        return "cos(" + operando->toString() + ")";
    }
};

class Ln: public UnaryFunction {
public:
    Ln(shared_ptr<Expression> operando): UnaryFunction(operando) {}

    double evaluate(double x) override {
        double valor = operando->evaluate(x);

        if (valor <= 0)
            throw domain_error("El argumento de ln debe ser positivo");

        return log(valor);
    }

    shared_ptr<Expression> derivative() override {
        return make_shared<Multiply>(
            operando->derivative(),
            make_shared<Power>(operando, make_shared<Constant>(-1))
        );
    }

    string toString() override {
        return "ln(" + operando->toString() + ")";
    }
};

shared_ptr<Expression> Sin::derivative() {
    return make_shared<Multiply>(
        make_shared<Cos>(operando),
        operando->derivative()
    );
}

shared_ptr<Expression> Power::derivative() {
    if (der->isConstant()) {
        double n = der->evaluate(0);

        if (n == 0)
            return make_shared<Constant>(0);

        if (n == 1)
            return izq->derivative();

        return make_shared<Multiply>(
            make_shared<Multiply>(
                make_shared<Constant>(n),
                make_shared<Power>(izq, make_shared<Constant>(n - 1))
            ),
            izq->derivative()
        );
    }

    return make_shared<Multiply>(
        make_shared<Power>(izq, der),
        make_shared<Add>(
            make_shared<Multiply>(
                der->derivative(),
                make_shared<Ln>(izq)
            ),
            make_shared<Multiply>(
                der,
                make_shared<Multiply>(
                    izq->derivative(),
                    make_shared<Power>(izq, make_shared<Constant>(-1))
                )
            )
        )
    );
}

int main() {
    auto x = make_shared<Variable>();

    auto expr = make_shared<Multiply>(
        make_shared<Add>(
            make_shared<Power>(
                x,
                make_shared<Constant>(2)
            ),
            make_shared<Multiply>(
                make_shared<Constant>(3),
                x
            )
        ),
        make_shared<Sin>(x)
    );

    auto deriv = expr->derivative();

    cout << "f(x) = " << expr->toString() << endl;
    cout << "f'(x) = " << deriv->toString() << endl;
    cout << "f(2) = " << expr->evaluate(2) << endl;
    cout << "f'(2) = " << deriv->evaluate(2) << endl;
}
