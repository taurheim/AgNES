#pragma once
#include <string>
#include <list>

enum ASTNodeType {
    PARAMETER,
};

enum CType {
    CTYPE_INT,
    CTYPE_CHAR,
    CTYPE_VOID
};

enum ExpressionType {
    ADD
};

struct ASTNode {
    ASTNodeType nodeType;
};

struct TypeNode : ASTNode {
    CType type;
};

struct IdentifierNode : ASTNode {
    std::string name;
};

struct NumberNode : ASTNode {
    int value;
};

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