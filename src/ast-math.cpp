/**
 * @file
 * @brief Implementation of mathematical functions for AST
 */
#include <memory>
#include "ast.h"
#include "ast-math.h"
#include "tokenizer.h"

static inline std::shared_ptr<ASTNode> copy(const std::shared_ptr<ASTNode>& root) {
    const TokenType rootTokenType = root->getToken()->getType();
    if (rootTokenType == CONSTANT_VALUE) {
        return std::make_shared<ASTNode>(std::make_shared<ConstantValueToken>(dynamic_cast<ConstantValueToken*>(root->getToken().get())->getValue()));
    } else if (rootTokenType == VARIABLE) {
        return std::make_shared<ASTNode>(VariableToken::getVariableByName(dynamic_cast<VariableToken*>(root->getToken().get())->getName()));
    } else if (rootTokenType == OPERATOR) {
        const auto operatorToken = dynamic_cast<OperatorToken*>(root->getToken().get());
        const OperatorType operatorType = operatorToken->getOperatorType();
        if (operatorToken->getArity() == 1) {
            std::shared_ptr<ASTNode> child = copy(root->getChildren()[0]);
            if (operatorType == ARITHMETIC_NEGATION) {
                return std::make_shared<ASTNode>(std::make_shared<ArithmeticNegationOperator>(), child);
            } else if (operatorType == UNARY_ADDITION) {
                return std::make_shared<ASTNode>(std::make_shared<UnaryAdditionOperator>(), child);
            } else {
                throw std::logic_error("Unsupported unary operator type");
            }
        } else if (operatorToken->getArity() == 2) {
            const auto leftChild  = copy(root->getChildren()[0]);
            const auto rightChild = copy(root->getChildren()[1]);
            if (operatorType == ADDITION) {
                return std::make_shared<ASTNode>(std::make_shared<AdditionOperator>(), leftChild, rightChild);
            } else if (operatorType == SUBTRACTION) {
                return std::make_shared<ASTNode>(std::make_shared<SubtractionOperator>(), leftChild, rightChild);
            } else if (operatorType == MULTIPLICATION) {
                return std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), leftChild, rightChild);
            } else if (operatorType == DIVISION) {
                return std::make_shared<ASTNode>(std::make_shared<DivisionOperator>(), leftChild, rightChild);
            } else if  (operatorType == POWER) {
                return std::make_shared<ASTNode>(std::make_shared<PowerOperator>(), leftChild, rightChild);
            } else {
                throw std::logic_error("Unsupported binary operator type");
            }
        } else {
            throw std::logic_error("Unsupported arity of operator. Only unary and binary are supported yet");
        }
    } else {
        throw std::logic_error("Unsupported toke type");
    }
}

std::shared_ptr<ASTNode> differentiate(const std::shared_ptr<ASTNode>& root, const char* differentiatedVariableName) {
    const TokenType rootTokenType = root->getToken()->getType();
    if (rootTokenType == CONSTANT_VALUE) { // C' = 0
        return std::make_shared<ASTNode>(std::make_shared<ConstantValueToken>(0));
    } else if (rootTokenType == VARIABLE) { // x' = 1, y' = y'
        const auto variableToken = dynamic_cast<VariableToken*>(root->getToken().get());
        const char* variableName = variableToken->getName();
        if (strcmp(variableName, differentiatedVariableName) == 0) {
            return std::make_shared<ASTNode>(std::make_shared<ConstantValueToken>(1));
        } else {
            const size_t variableNameLen = strlen(variableName);
            char* newVariableName = (char*)calloc(variableNameLen + 2, sizeof(char));
            newVariableName[variableNameLen] = '\'';
            for (size_t i = 0; i < variableNameLen; ++i) {
                newVariableName[i] = variableName[i];
            }
            return std::make_shared<ASTNode>(VariableToken::getVariableByName(newVariableName));
        }
    } else if (rootTokenType == OPERATOR) {
        const auto operatorToken = dynamic_cast<OperatorToken*>(root->getToken().get());
        const OperatorType operatorType = operatorToken->getOperatorType();
        if (operatorToken->getArity() == 1) {
            std::shared_ptr<ASTNode> childDerivative = differentiate(root->getChildren()[0], differentiatedVariableName);
            if (operatorType == ARITHMETIC_NEGATION) { // (-f(x))' = -(f(x))'
                return std::make_shared<ASTNode>(std::make_shared<ArithmeticNegationOperator>(), childDerivative);
            } else if (operatorType == UNARY_ADDITION) { // (+f(x))' = +(f(x))'
                return std::make_shared<ASTNode>(std::make_shared<UnaryAdditionOperator>(), childDerivative);
            } else {
                throw std::logic_error("Unsupported unary operator type");
            }
        } else if (operatorToken->getArity() == 2) {
            const auto leftChildDerivative  = differentiate(root->getChildren()[0], differentiatedVariableName);
            const auto rightChildDerivative = differentiate(root->getChildren()[1], differentiatedVariableName);
            const auto leftChildCopy  = copy(root->getChildren()[0]);
            const auto rightChildCopy = copy(root->getChildren()[1]);
            if (operatorType == ADDITION) { // (f(x) + g(x))' = f(x)' + g(x)'
                return std::make_shared<ASTNode>(std::make_shared<AdditionOperator>(), leftChildDerivative, rightChildDerivative);
            } else if (operatorType == SUBTRACTION) { // (f(x) - g(x))' = f(x)' - g(x)'
                return std::make_shared<ASTNode>(std::make_shared<SubtractionOperator>(), leftChildDerivative, rightChildDerivative);
            } else if (operatorType == MULTIPLICATION) { // (f(x) * g(x))' = (f(x)' * g(x)) + (f(x) * g(x)')
                const auto leftSubTree  = std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), leftChildDerivative, rightChildCopy);
                const auto rightSubTree = std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), leftChildCopy, rightChildDerivative);
                return std::make_shared<ASTNode>(std::make_shared<AdditionOperator>(), leftSubTree, rightSubTree);
            } else if (operatorType == DIVISION) { // (f(x) / g(x))' = ((f(x)' * g(x)) - (f(x) * g(x)')) / (g(x) * g(x))
                const auto leftSubTree  = std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), leftChildDerivative, rightChildCopy);
                const auto rightSubTree = std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), leftChildCopy, rightChildDerivative);
                const auto numerator    = std::make_shared<ASTNode>(std::make_shared<SubtractionOperator>(), leftSubTree, rightSubTree);
                const auto denominator  = std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), rightChildCopy, rightChildCopy);
                return std::make_shared<ASTNode>(std::make_shared<DivisionOperator>(), numerator, denominator);
            } else if  (operatorType == POWER) { // TODO
                throw std::logic_error("Derivative of power operator is not supported yet");
            } else {
                throw std::logic_error("Unsupported binary operator type");
            }
        } else {
            throw std::logic_error("Unsupported arity of operator. Only unary and binary are supported yet");
        }
    } else {
        throw std::logic_error("Unsupported toke type");
    }
}