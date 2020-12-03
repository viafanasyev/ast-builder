/**
 * @file
 * @brief Definition of mathematical functions for AST
 */
#ifndef AST_BUILDER_AST_MATH_H
#define AST_BUILDER_AST_MATH_H

#include <memory>
#include "ast.h"
#include "tokenizer.h"

std::shared_ptr<ASTNode> differentiate(const std::shared_ptr<ASTNode>& root, const char* differentiatedVariableName);

#endif // AST_BUILDER_AST_MATH_H
