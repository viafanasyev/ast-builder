/**
 * @file
 */
#include <cstring>
#include <memory>
#include "ast.h"
#include "ast-optimizers.h"

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
    try {
        std::shared_ptr<ASTNode> ASTRoot = buildAST(argv[1]);

        if (optimized) {
            auto optimizer = std::make_shared<CompositeOptimizer>();
            optimizer->addOptimizer(std::make_shared<UnaryAdditionOptimizer>());
            optimizer->addOptimizer(std::make_shared<ArithmeticNegationOptimizer>());
            ASTRoot = optimizer->optimize(ASTRoot);
        }

        const char* fileName = "expression";
        ASTRoot->visualize(fileName);
        ASTRoot->texify(fileName);
    } catch (std::invalid_argument& ex) {
        fprintf(stderr, "Invalid expression: %s", ex.what());
    } catch (std::logic_error& ex) {
        fprintf(stderr, "Invalid expression: %s", ex.what());
    }
}
