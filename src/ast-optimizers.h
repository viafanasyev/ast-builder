/**
 * @file
 * @brief Definition of AST optimizers
 */
#ifndef AST_BUILDER_AST_OPTIMIZERS_H
#define AST_BUILDER_AST_OPTIMIZERS_H

#include <memory>
#include <vector>
#include "ast.h"

class Optimizer {

public:
    virtual std::shared_ptr<ASTNode> optimize(std::shared_ptr<ASTNode>& node) const = 0;
};

class CompositeOptimizer : public Optimizer {

private:
    std::vector<std::shared_ptr<Optimizer> > optimizers;

public:
    void addOptimizer(const std::shared_ptr<Optimizer>& optimizer) {
        optimizers.push_back(optimizer);
    }

    std::shared_ptr<ASTNode> optimize(std::shared_ptr<ASTNode>& node) const override {
        for (auto& optimizer : optimizers) {
            node = optimizer->optimize(node);
        }
        return node;
    }
};

/**
 * Optimizer for unary addition. Removes nodes with unary addition because they are useless.
 */
class UnaryAdditionOptimizer : public Optimizer {

public:
    std::shared_ptr<ASTNode> optimize(std::shared_ptr<ASTNode>& node) const override;
};

/**
 * Optimizer for double arithmetic negations. All double negations are removed.
 */
class ArithmeticNegationOptimizer : public Optimizer {

public:
    std::shared_ptr<ASTNode> optimize(std::shared_ptr<ASTNode>& node) const override;
};

#endif // AST_BUILDER_AST_OPTIMIZERS_H
