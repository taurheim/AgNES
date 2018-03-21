#pragma once
#include <string>
#include <list>

enum ASTNodeType {
    AST_UNDEFINED,
    AST_PROGRAM,
    AST_FUNCTION,
    AST_DECLARATION,
    AST_IDENTIFIER,
    AST_NUMBER,
    AST_TYPE
};

struct ASTNode {
    explicit ASTNode(ASTNodeType type) {
        nodeType = type;
    }
    ASTNodeType nodeType;
    std::list<ASTNode*> children;
};

struct TypeNode : ASTNode {
    TypeNode() : ASTNode(AST_TYPE) {
    }
    std::string typeString;
};

struct IdentifierNode : ASTNode {
    IdentifierNode() : ASTNode(AST_IDENTIFIER) {
    }
    std::string name;
};

struct NumberNode : ASTNode {
    NumberNode() : ASTNode(AST_NUMBER) {
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