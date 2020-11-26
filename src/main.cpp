/**
 * @file
 */
#include <memory>
#include "ast.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Invalid arguments number (argc = %d)", argc);
        return -1;
    }
    try {
        std::shared_ptr<ASTNode> ASTRoot = buildAST(argv[1]);

        const char* dotFileName = "graph.dot";
        const char* imageFileName = "graph.png";
        ASTRoot->visualize(dotFileName, imageFileName);
    } catch (std::invalid_argument& ex) {
        fprintf(stderr, "Invalid expression: %s", ex.what());
    } catch (std::logic_error& ex) {
        fprintf(stderr, "Invalid expression: %s", ex.what());
    }
}
