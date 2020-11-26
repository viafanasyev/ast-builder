/**
 * @file
 * @brief Implementation of tokenizer functions
 */
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include "tokenizer.h"

void Token::print() const {
    printf("%s", TokenTypeStrings[type]);
}

void ConstantValueToken::print() const {
    Token::print();
    printf(" VALUE=%lf", value);
}

double ConstantValueToken::calculate(size_t argc, ...) const {
    assert(argc == 0);
    return value;
}

void ParenthesisToken::print() const {
    Token::print();
    printf(" %s", open ? "OPEN" : "CLOSE");
}

double ParenthesisToken::calculate(size_t argc __attribute__((unused)), ...) const {
    throw std::logic_error("Parenthesis can't be calculated");
}

void OperatorToken::print() const {
    Token::print();
    printf(" ARITY=%zu, PRECEDENCE=%zu, TYPE=%s", arity, precedence, OperatorTypeStrings[operatorType]);
}

double AdditionOperator::calculate(size_t argc, ...) const {
    assert(argc == 2);
    va_list operands;
    va_start(operands, argc);
    double leftOperand = va_arg(operands, double);
    double rightOperand = va_arg(operands, double);
    va_end(operands);
    return leftOperand + rightOperand;
}

double SubtractionOperator::calculate(size_t argc, ...) const {
    assert(argc == 2);
    va_list operands;
    va_start(operands, argc);
    double leftOperand = va_arg(operands, double);
    double rightOperand = va_arg(operands, double);
    va_end(operands);
    return leftOperand - rightOperand;
}

double MultiplicationOperator::calculate(size_t argc, ...) const {
    assert(argc == 2);
    va_list operands;
    va_start(operands, argc);
    double leftOperand = va_arg(operands, double);
    double rightOperand = va_arg(operands, double);
    va_end(operands);
    return leftOperand * rightOperand;
}

double DivisionOperator::calculate(size_t argc, ...) const {
    assert(argc == 2);
    va_list operands;
    va_start(operands, argc);
    double leftOperand = va_arg(operands, double);
    double rightOperand = va_arg(operands, double);
    va_end(operands);
    return leftOperand / rightOperand;
}

double ArithmeticNegationOperator::calculate(size_t argc, ...) const {
    assert(argc == 1);
    va_list operands;
    va_start(operands, argc);
    double operand = va_arg(operands, double);
    va_end(operands);
    return -operand;
}

double UnaryAdditionOperator::calculate(size_t argc, ...) const {
    assert(argc == 1);
    va_list operands;
    va_start(operands, argc);
    double operand = va_arg(operands, double);
    va_end(operands);
    return operand;
}

static bool addNextToken(char*& expression, std::vector<std::shared_ptr<Token>>& tokens);

/**
 * Splits the expression into Token objects.
 * @param expression expression to tokenize
 * @return vector of parsed tokens.
 * @throws std::invalid_argument if invalid symbol met.
 */
std::vector<std::shared_ptr<Token>> tokenize(char* expression) {
    assert(expression != nullptr);

    std::vector<std::shared_ptr<Token>> tokens;
    while (addNextToken(expression, tokens))
        ;
    return tokens;
}

static bool addNextToken(char*& expression, std::vector<std::shared_ptr<Token>>& tokens) {
    assert(expression != nullptr);

    while (std::isspace(*expression)) {
        ++expression;
    }
    if (*expression == '\0') return false;

    if (*expression == '(') {
        tokens.emplace_back(new ParenthesisToken(true));
        ++expression;
    } else if (*expression == ')') {
        tokens.emplace_back(new ParenthesisToken(false));
        ++expression;
    } else if (*expression == '*') {
        tokens.emplace_back(new MultiplicationOperator());
        ++expression;
    } else if (*expression == '/') {
        tokens.emplace_back(new DivisionOperator());
        ++expression;
    } else if ((*expression == '+') || (*expression == '-')) {
        Token* previousToken = nullptr;
        if (!tokens.empty()) {
            previousToken = tokens[tokens.size() - 1].get();
        }

        bool isBinary = (previousToken != nullptr) && (
                (previousToken->getType() == CONSTANT_VALUE) ||
                ((previousToken->getType() == PARENTHESIS) &&
                 (dynamic_cast<ParenthesisToken*>(previousToken)->isClose()))
        );

        if (isBinary) {
            if (*expression == '+') {
                tokens.emplace_back(new AdditionOperator());
            } else {
                tokens.emplace_back(new SubtractionOperator());
            }
        } else {
            if (*expression == '+') {
                tokens.emplace_back(new UnaryAdditionOperator());
            } else {
                tokens.emplace_back(new ArithmeticNegationOperator());
            }
        }

        ++expression;
    } else if (isdigit(*expression)) {
        double tokenValue = strtod(expression, &expression);
        tokens.emplace_back(new ConstantValueToken(tokenValue));
    } else {
        char message[26];
        snprintf(message, sizeof(message), "Invalid symbol found: '%c'", *expression);
        throw std::invalid_argument(message);
    }

    return true;
}
