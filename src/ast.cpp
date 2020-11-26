/**
 * @file
 * @brief Implementation of AST building functions
 */
#include <cassert>
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
            int currentTokenPrecedence = dynamic_cast<OperatorToken*>(token.get())->getPrecedence();
            while (!stack.empty() && stack.top()->getType() == TokenType::OPERATOR) {
                int topTokenPrecedence = dynamic_cast<OperatorToken*>(stack.top().get())->getPrecedence();
                if (topTokenPrecedence >= currentTokenPrecedence) {
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
        throw std::invalid_argument(
                "Too little operands"); // TODO: More verbose error context (put stack size and parentNode operatorType?)
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
