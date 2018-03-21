#pragma once
#include <string>
#include <list>

enum ASTNodeType {
    AST_NONTERMINAL,
    AST_IDENTIFIER,
    AST_NUMBER,
    AST_TYPE
};

struct ASTNode {
    ASTNodeType nodeType = AST_NONTERMINAL;
    std::list<ASTNode*> children;
};

struct TypeNode : ASTNode {
    TypeNode() {
        nodeType = AST_TYPE;
    }
    std::string typeString;
};

struct IdentifierNode : ASTNode {
    IdentifierNode() {
        nodeType = AST_IDENTIFIER;
    }
    std::string name;
};

struct NumberNode : ASTNode {
    NumberNode() {
        nodeType = AST_NUMBER;
    }
    int value;
};

/*
struct ParameterNode : ASTNode {
    TypeNode * paramType;
    IdentifierNode * paramName;
};

struct StatementNode : ASTNode {
};

struct FunctionNode : ASTNode {
    TypeNode * returnType;
    IdentifierNode * functionName;
    std::list<ParameterNode *> parameters;
    std::list<StatementNode *> statements;
};

struct ProgramNode : ASTNode {
    // Can be either FunctionDefinitionNode or DeclarationNode
    std::list<void*> programBody;
};

struct DeclarationNode : ASTNode {
    TypeNode * type;
    IdentifierNode * identifier;
};

struct ExpressionNode : ASTNode {
    ExpressionType type;
    ExpressionNode * firstParam;
    ExpressionNode * secondParam;
    // std::list<ExpressionNode> args;
};

struct AssignmentNode : ASTNode {
    IdentifierNode * id;
    ExpressionNode * newValue;
    ExpressionNode * arraySelectorExpression;
};
*/