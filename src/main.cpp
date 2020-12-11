/**
 * @file
 */
#include <cstring>
#include <memory>
#include "ast.h"
#include "ast-math.h"
#include "ast-optimizers.h"
#include "recursive_parser.h"
#include "SyntaxError.h"

void outputAST(const std::shared_ptr<ASTNode>& root, const char* fileName) {
    root->visualize(fileName);
    root->texify(fileName);
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Invalid arguments number (argc = %d)", argc);
        return -1;
    }
    if ((argc == 3) && (strcmp(argv[2], "--optimized") != 0)) {
        fprintf(stderr, "Invalid option '%s'. Only '--optimized' is supported", argv[2]);
        return -1;
    }

    bool optimized = (argc == 3);

    auto optimizer = std::make_shared<CompositeOptimizer>();
    optimizer->addOptimizer(std::make_shared<UnaryAdditionOptimizer>());
    optimizer->addOptimizer(std::make_shared<ArithmeticNegationOptimizer>());
    optimizer->addOptimizer(std::make_shared<TrivialOperationsOptimizer>());

    try {
        std::shared_ptr<ASTNode> ASTRoot = buildASTRecursively(argv[1]);
        if (optimized) ASTRoot = optimizer->optimize(ASTRoot);
        outputAST(ASTRoot, "expression");

        ASTRoot = differentiate(ASTRoot, "x");
        if (optimized) ASTRoot = optimizer->optimize(ASTRoot);
        outputAST(ASTRoot, "expression-derivative");
    } catch (const std::invalid_argument& ex) {
        fprintf(stderr, "Invalid expression: %s", ex.what());
    } catch (const std::logic_error& ex) {
        fprintf(stderr, "Invalid expression: %s", ex.what());
    } catch (const SyntaxError& ex) {
        fprintf(stderr, "Syntax error: %s", ex.what());
    }
}
