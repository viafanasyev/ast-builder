/**
 * @file
 * @brief Implementation of AST optimizers
 */
#include <memory>
#include "ast.h"
#include "ast-optimizers.h"
#include "tokenizer.h"

std::shared_ptr <ASTNode> UnaryAdditionOptimizer::optimize(std::shared_ptr <ASTNode>& node) const {
    bool hasChanges = false;
    do {
        hasChanges = false;
        auto token = node->getToken().get();
        if (token->getType() == TokenType::OPERATOR) {
            auto operatorToken = dynamic_cast<OperatorToken*>(token);
            if (operatorToken->getOperatorType() == OperatorType::UNARY_ADDITION) {
                assert(node->getChildrenNumber() == 1);
                node = node->getChildren()[0];
                hasChanges = true;
            }
        }
    } while (hasChanges);

    auto children = node->getChildren();
    size_t childrenNumber = node->getChildrenNumber();
    for (size_t i = 0; i < childrenNumber; ++i) {
        children[i] = optimize(children[i]);
    }
    return node;
}

std::shared_ptr <ASTNode> ArithmeticNegationOptimizer::optimize(std::shared_ptr <ASTNode>& node) const {
    bool hasChanges = false;
    do {
        hasChanges = false;
        auto token = node->getToken().get();
        if (token->getType() == TokenType::OPERATOR) {
            auto operatorToken = dynamic_cast<OperatorToken*>(token);
            if (operatorToken->getOperatorType() == OperatorType::ARITHMETIC_NEGATION) {
                assert(node->getChildrenNumber() == 1);

                auto child = node->getChildren()[0];
                auto childToken = child->getToken().get();
                if (childToken->getType() == TokenType::OPERATOR) {
                    auto childOperatorToken = dynamic_cast<OperatorToken*>(childToken);
                    if (childOperatorToken->getOperatorType() == OperatorType::ARITHMETIC_NEGATION) {
                        assert(child->getChildrenNumber() == 1);
                        node = child->getChildren()[0];
                        hasChanges = true;
                    }
                }

            }
        }
    } while (hasChanges);

    auto children = node->getChildren();
    size_t childrenNumber = node->getChildrenNumber();
    for (size_t i = 0; i < childrenNumber; ++i) {
        children[i] = optimize(children[i]);
    }
    return node;
}
