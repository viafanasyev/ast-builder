/**
 * @file
 * @brief Definition of tokens that can be parsed and a tokenizer functions
 */
#ifndef AST_BUILDER_TOKENIZER_H
#define AST_BUILDER_TOKENIZER_H

#include <cctype>
#include <climits>
#include <memory>
#include <vector>

enum TokenType {
    CONSTANT_VALUE,
    PARENTHESIS,
    OPERATOR,
};

static const char* const TokenTypeStrings[] = {
    "CONSTANT_VALUE",
    "PARENTHESIS",
    "OPERATOR",
};

class Token {

private:
    const TokenType type;

public:
    explicit Token(TokenType type_) : type(type_) { }
    virtual ~Token() = default;

    TokenType getType() const {
        return type;
    }

    virtual void print() const;
};

class ConstantValueToken : public Token {

private:
    const double value;

public:
    explicit ConstantValueToken(double value_) : Token(CONSTANT_VALUE), value(value_) { }

    double getValue() const {
        return value;
    }

    void print() const override;
};

class ParenthesisToken : public Token {

private:
    const bool open;

public:
    explicit ParenthesisToken(bool open_) : Token(PARENTHESIS), open(open_) { }

    bool isOpen() const {
        return open;
    }

    bool isClose() const {
        return !open;
    }

    void print() const override;
};

enum OperatorType {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    ARITHMETIC_NEGATION,
    UNARY_ADDITION,
};

static const char* const OperatorTypeStrings[] = {
    "ADDITION",
    "SUBTRACTION",
    "MULTIPLICATION",
    "DIVISION",
    "ARITHMETIC_NEGATION",
    "UNARY_ADDITION",
};

class OperatorToken : public Token {

private:
    const size_t arity;
    const size_t precedence;
    const OperatorType operatorType;

public:
    OperatorToken(size_t arity_, size_t precedence_, OperatorType operatorType_) :
        Token(OPERATOR), arity(arity_), precedence(precedence_), operatorType(operatorType_) { }

    size_t getArity() const {
        return arity;
    }

    size_t getPrecedence() const {
        return precedence;
    }

    OperatorType getOperatorType() const {
        return operatorType;
    }

    void print() const override;
};

class AdditionOperator : public OperatorToken {

public:
    AdditionOperator() : OperatorToken(2, 1, ADDITION) { }
};

class SubtractionOperator : public OperatorToken {

public:
    SubtractionOperator() : OperatorToken(2, 1, SUBTRACTION) { }
};

class MultiplicationOperator : public OperatorToken {

public:
    MultiplicationOperator() : OperatorToken(2, 2, MULTIPLICATION) { }
};

class DivisionOperator : public OperatorToken {

public:
    DivisionOperator() : OperatorToken(2, 2, DIVISION) { }
};

class ArithmeticNegationOperator : public OperatorToken {

public:
    ArithmeticNegationOperator() : OperatorToken(1, 1000, ARITHMETIC_NEGATION) { }
};

class UnaryAdditionOperator : public OperatorToken {

public:
    UnaryAdditionOperator() : OperatorToken(1, 1000, UNARY_ADDITION) { }
};

/**
 * Splits the expression into Token objects.
 * @param expression expression to tokenize
 * @return vector of parsed tokens.
 * @throws std::invalid_argument if invalid symbol met.
 */
std::vector<std::shared_ptr<Token>> tokenize(char* expression);

#endif // AST_BUILDER_TOKENIZER_H
