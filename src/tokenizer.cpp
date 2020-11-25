/**
 * @file
 * @brief Implementation of tokenizer functions
 */
#include <cassert>
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

void ParenthesisToken::print() const {
    Token::print();
    printf(" %s", open ? "OPEN" : "CLOSE");
}

void OperatorToken::print() const {
    Token::print();
    printf(" ARITY=%zu, PRECEDENCE=%zu, TYPE=%s", arity, precedence, OperatorTypeStrings[operatorType]);
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
