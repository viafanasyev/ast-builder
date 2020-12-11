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
            switch (operatorType) {
                case ARITHMETIC_NEGATION:
                    return std::make_shared<ASTNode>(std::make_shared<ArithmeticNegationOperator>(), child);
                case UNARY_ADDITION:
                    return std::make_shared<ASTNode>(std::make_shared<UnaryAdditionOperator>(), child);
                default:
                    throw std::logic_error("Unsupported unary operator type");
            }
        } else if (operatorToken->getArity() == 2) {
            const auto leftChild  = copy(root->getChildren()[0]);
            const auto rightChild = copy(root->getChildren()[1]);
            switch (operatorType) {
                case ADDITION:
                    return std::make_shared<ASTNode>(std::make_shared<AdditionOperator>(), leftChild, rightChild);
                case SUBTRACTION:
                    return std::make_shared<ASTNode>(std::make_shared<SubtractionOperator>(), leftChild, rightChild);
                case MULTIPLICATION:
                    return std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), leftChild, rightChild);
                case DIVISION:
                    return std::make_shared<ASTNode>(std::make_shared<DivisionOperator>(), leftChild, rightChild);
                case POWER:
                    return std::make_shared<ASTNode>(std::make_shared<PowerOperator>(), leftChild, rightChild);
                default:
                    throw std::logic_error("Unsupported binary operator type");
            }
        } else {
            throw std::logic_error("Unsupported arity of operator. Only unary and binary are supported yet");
        }
    } else if (rootTokenType == FUNCTION) {
        const auto functionToken = dynamic_cast<FunctionToken*>(root->getToken().get());
        const FunctionType functionType = functionToken->getFunctionType();
        if (functionToken->getArity() == 1) {
            std::shared_ptr<ASTNode> child = copy(root->getChildren()[0]);
            switch (functionType) {
                case SIN:
                    return std::make_shared<ASTNode>(std::make_shared<SinFunction>(), child);
                case COS:
                    return std::make_shared<ASTNode>(std::make_shared<CosFunction>(), child);
                case TG:
                    return std::make_shared<ASTNode>(std::make_shared<TgFunction>(), child);
                case CTG:
                    return std::make_shared<ASTNode>(std::make_shared<CtgFunction>(), child);
                case LN:
                    return std::make_shared<ASTNode>(std::make_shared<LnFunction>(), child);
                default:
                    throw std::logic_error("Unsupported unary function type");
            }
        } else {
            throw std::logic_error("Unsupported arity of function. Only unary are supported yet");
        }
    } else {
        throw std::logic_error("Unsupported token type");
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
            } else if  (operatorType == POWER) {
                const auto leftChildType  = root->getChildren()[0]->getToken()->getType();
                const auto rightChildType = root->getChildren()[1]->getToken()->getType();
                if (leftChildType == CONSTANT_VALUE && rightChildType == CONSTANT_VALUE) { // (C^C)' = 0
                    return std::make_shared<ASTNode>(std::make_shared<ConstantValueToken>(0));
                } else if (rightChildType == CONSTANT_VALUE) { // (f(x)^C)' = C * f(x)^(C - 1) * (f(x))'
                    const auto decConst = std::make_shared<ASTNode>(std::make_shared<ConstantValueToken>(rightChildCopy->calculate() - 1));
                    const auto leftMultiplier  = std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), rightChildCopy, leftChildDerivative);
                    const auto rightMultiplier = std::make_shared<ASTNode>(std::make_shared<PowerOperator>(), leftChildCopy, decConst);
                    return std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), leftMultiplier, rightMultiplier);
                } else if (leftChildType == CONSTANT_VALUE) { // (C^f(x))' = ln(C) * C^f(x) * f(x)'
                    const auto rootCopy = copy(root);
                    const auto lnConst = std::make_shared<ASTNode>(std::make_shared<LnFunction>(), leftChildCopy);
                    const auto leftMultiplier = std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), lnConst, rightChildDerivative);
                    return std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), leftMultiplier, rootCopy);
                } else { // TODO: (f(x) ^ g(x))' = f(x)^(g(x) - 1) * (g(x) * f(x)' + f(x) * ln(f(x)) * g(x)')
                    throw std::logic_error("Derivative of f(x)^g(x) is not supported yet");
                }
            } else {
                throw std::logic_error("Unsupported binary operator type");
            }
        } else {
            throw std::logic_error("Unsupported arity of operator. Only unary and binary are supported yet");
        }
    } else if (rootTokenType == FUNCTION) {
        const auto functionToken = dynamic_cast<FunctionToken*>(root->getToken().get());
        const FunctionType functionType = functionToken->getFunctionType();
        if (functionToken->getArity() == 1) {
            std::shared_ptr<ASTNode> childDerivative = differentiate(root->getChildren()[0], differentiatedVariableName);
            std::shared_ptr<ASTNode> childCopy = copy(root->getChildren()[0]);
            if (functionType == SIN) { // sin(f(x))' = f(x)' * cos(f(x))
                auto funcDerivative = std::make_shared<ASTNode>(std::make_shared<CosFunction>(), childCopy);
                return std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), childDerivative, funcDerivative);
            } else if (functionType == COS) { // cos(f(x))' = f(x)' * -sin(f(x))
                auto funcDerivative = std::make_shared<ASTNode>(std::make_shared<SinFunction>(), childCopy);
                funcDerivative = std::make_shared<ASTNode>(std::make_shared<ArithmeticNegationOperator>(), funcDerivative);
                return std::make_shared<ASTNode>(std::make_shared<MultiplicationOperator>(), childDerivative, funcDerivative);
            } else if (functionType == TG) { // tg(f(x))' = f(x)' / cos(f(x))^2
                auto funcDerivative = std::make_shared<ASTNode>(std::make_shared<CosFunction>(), childCopy);
                funcDerivative = std::make_shared<ASTNode>(std::make_shared<PowerOperator>(), funcDerivative, std::make_shared<ASTNode>(std::make_shared<ConstantValueToken>(2)));
                return std::make_shared<ASTNode>(std::make_shared<DivisionOperator>(), childDerivative, funcDerivative);
            } else if (functionType == CTG) { // ctg(f(x))' = f(x)' / -sin(f(x))^2
                auto funcDerivative = std::make_shared<ASTNode>(std::make_shared<SinFunction>(), childCopy);
                funcDerivative = std::make_shared<ASTNode>(std::make_shared<PowerOperator>(), funcDerivative, std::make_shared<ASTNode>(std::make_shared<ConstantValueToken>(2)));
                funcDerivative = std::make_shared<ASTNode>(std::make_shared<ArithmeticNegationOperator>(), funcDerivative);
                return std::make_shared<ASTNode>(std::make_shared<DivisionOperator>(), childDerivative, funcDerivative);
            } else if (functionType == LN) { // ln(f(x))' = f(x)' / f(x)
                return std::make_shared<ASTNode>(std::make_shared<DivisionOperator>(), childDerivative, childCopy);
            } else {
                throw std::logic_error("Unsupported unary function type");
            }
        } else {
            throw std::logic_error("Unsupported arity of function. Only unary are supported yet");
        }
    } else {
        throw std::logic_error("Unsupported token type");
    }
}