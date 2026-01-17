#ifndef AST_NODE_HPP 
#define AST_NODE_HPP 
#include <memory> 
struct ASTNode { 
    virtual ~ASTNode() = default; 
}; 

using ASTNodePtr = std::shared_ptr<ASTNode>; 
#endif #ifndef AST_EXPR_HPP 