/**
 * @file
 * @brief Implementation of AST building functions
 */
#include <cassert>
#include <cstdio>
#include <cstring>
#include <stack>
#include <stdexcept>
#include <vector>
#include "ast.h"
#include "tokenizer.h"

void ASTNode::print(int depth) const {
    for (int i = 0; i < depth; ++i) {
        printf("\t");
    }
    token->print();
    printf("\n");

    for (size_t i = 0; i < childrenNumber; ++i) {
        children[i]->print(depth + 1);
    }
}

void ASTNode::dotPrint(FILE* dotFile, int& nodeId) const {
    if (token->getType() == TokenType::CONSTANT_VALUE) {
        fprintf(dotFile, "%d [label=\"%lg\", shape=box, style=filled, color=\"grey\", fillcolor=\"#FFFEC9\"];\n", nodeId, dynamic_cast<ConstantValueToken*>(token.get())->getValue());
        ++nodeId;
    } else {
        auto operatorToken = dynamic_cast<OperatorToken*>(token.get());
        fprintf(dotFile, "%d [label=\"%s\", shape=box, style=filled, color=\"grey\", fillcolor=\"#C9E7FF\"];\n", nodeId, OperatorTypeStrings[operatorToken->getOperatorType()]);
        int childrenNodeId = nodeId + 1;
        for (size_t i = 0; i < childrenNumber; ++i) {
            fprintf(dotFile, "%d->%d\n", nodeId, childrenNodeId);
            children[i]->dotPrint(dotFile, childrenNodeId);
        }
        nodeId = childrenNodeId;
    }
}

double ASTNode::calculate() const {
    switch (childrenNumber) {
        case 0:
            return token->calculate(0);
        case 1:
            return token->calculate(1, children[0]->calculate());
        case 2:
            return token->calculate(2, children[0]->calculate(), children[1]->calculate());
        default:
            throw std::logic_error("Unsupported arity of operator. Only unary and binary are supported yet");
    }
}

void ASTNode::visualize(const char* dotFileName, const char* imageFileName) const {
    assert(dotFileName != nullptr);
    assert(imageFileName != nullptr);
    assert(dotFileName != imageFileName);

    FILE* graphvizTextFile = fopen(dotFileName, "w");
    fprintf(graphvizTextFile, "digraph AST {\n");
    int nodeId = 0;
    dotPrint(graphvizTextFile, nodeId);
    fprintf(graphvizTextFile, "\"= %lg\" [shape=box];\n", calculate());
    fprintf(graphvizTextFile, "}\n");
    fclose(graphvizTextFile);

    char dotCommand[1000];
    sprintf(dotCommand, "dot -Tpng -O%s %s", imageFileName, dotFileName); // FIXME: .png file has is named '*.dot.png' somehow
    system(dotCommand);
    char xdgOpenCommand[1000];
    sprintf(xdgOpenCommand, "xdg-open %s.png", dotFileName);
    system(xdgOpenCommand);
}

static inline void connectWithOperands(std::stack<std::shared_ptr<ASTNode> >& astNodes, const std::shared_ptr<Token>& parentNodeToken);

std::shared_ptr<ASTNode> buildAST(char* expression) {
    return buildAST(tokenize(expression));
}

std::shared_ptr<ASTNode> buildAST(const std::vector<std::shared_ptr<Token> >& infixNotationTokens) {
    std::stack<std::shared_ptr<Token> > stack;
    std::stack<std::shared_ptr<ASTNode> > astNodes;

    for (auto& token : infixNotationTokens) {
        if (token->getType() == TokenType::CONSTANT_VALUE) {
            astNodes.push(std::make_shared<ASTNode>(token));
        } else if (token->getType() == TokenType::PARENTHESIS) {
            auto parenthesisToken = dynamic_cast<ParenthesisToken*>(token.get());
            if (parenthesisToken->isOpen()) {
                stack.push(token);
            } else {
                while (!stack.empty() && (stack.top()->getType() != TokenType::PARENTHESIS)) {
                    auto& topToken = stack.top();
                    stack.pop();
                    connectWithOperands(astNodes, topToken);
                }
                if (stack.empty()) {
                    throw std::invalid_argument("Missing open parenthesis");
                }
                assert(dynamic_cast<ParenthesisToken*>(stack.top().get())->isOpen()); // There should be only open parentheses on the stack
                stack.pop();
            }
        } else if (token->getType() == TokenType::OPERATOR) {
            auto currentToken = dynamic_cast<OperatorToken*>(token.get());
            int currentTokenPrecedence = currentToken->getPrecedence();
            while (!stack.empty() && stack.top()->getType() == TokenType::OPERATOR) {
                int topTokenPrecedence = dynamic_cast<OperatorToken*>(stack.top().get())->getPrecedence();
                if (topTokenPrecedence > currentTokenPrecedence ||
                    (
                        (topTokenPrecedence == currentTokenPrecedence) &&
                        (currentToken->getOperatorType() != OperatorType::UNARY_ADDITION) && // UNARY_ADDITION and ARITHMETIC_NEGATION are right-associative
                        (currentToken->getOperatorType() != OperatorType::ARITHMETIC_NEGATION)
                    )
                ) {
                    connectWithOperands(astNodes, stack.top());
                    stack.pop();
                } else {
                    break;
                }
            }
            stack.push(token);
        } else {
            throw std::logic_error("Unsupported token type");
        }
    }

    while (!stack.empty()) {
        auto& token = stack.top();
        stack.pop();
        if (token->getType() == TokenType::PARENTHESIS) {
            throw std::invalid_argument("Unclosed parenthesis");
        }
        connectWithOperands(astNodes, token);
    }
    if (astNodes.size() != 1) {
        throw std::invalid_argument("Too much operands");
    }
    return astNodes.top();
}

static inline void connectWithOperands(std::stack<std::shared_ptr<ASTNode> >& astNodes, const std::shared_ptr<Token>& parentNodeToken) {
    assert(parentNodeToken->getType() == TokenType::OPERATOR);

    size_t parentNodeArity = dynamic_cast<OperatorToken*>(parentNodeToken.get())->getArity();
    if (astNodes.size() < parentNodeArity) {
        throw std::invalid_argument("Too little operands"); // TODO: More verbose error context (put stack size and parentNode operatorType?)
    }

    if (parentNodeArity == 1) {
        auto child = astNodes.top();
        astNodes.pop();
        astNodes.push(std::make_shared<ASTNode>(parentNodeToken, child));
    } else if (parentNodeArity == 2) {
        auto rightChild = astNodes.top();
        astNodes.pop();
        auto leftChild = astNodes.top();
        astNodes.pop();
        astNodes.push(std::make_shared<ASTNode>(parentNodeToken, leftChild, rightChild));
    } else {
        throw std::logic_error("Unsupported arity of operator. Only unary and binary are supported yet");
    }
}
