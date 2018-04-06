#pragma once
#include <string>
#include <vector>
#include "varType.h"

enum ASTNodeType {
    AST_UNDEFINED,
    AST_PROGRAM,
    AST_FUNCTION,
    AST_DECLARATION,
    AST_IDENTIFIER,
    AST_NUMBER,
    AST_TYPE,
    AST_PARAMETERLIST,
    AST_PARAMETER,
    AST_STATEMENT,
    AST_ASSIGNMENT,
    AST_EXPRESSION,
    AST_BINARYEXPRESSION,
    AST_OPERATOR,
    AST_KEYWORD,
    AST_VARDECLARATION,
    AST_FUNCTIONBODY,
    AST_ARRAYINDEX,
    AST_CHAR,
};

enum StatementType {
    STMT_WHILE,
    STMT_FOR,
    STMT_IF,
    STMT_RETURN,
    STMT_ASSIGN,
    STMT_FUNCTIONCALL,
    STMT_BLOCK,
    STMT_EMPTY,
    
    // NES-specific statements
    STMT_NES_WAITFORFRAME,
    STMT_NES_SETSPRITEX,
    STMT_NES_SETSPRITEY,
};

struct ASTNode {
    explicit ASTNode(ASTNodeType type) {
        nodeType = type;
    }
    ASTNodeType nodeType;
    std::vector<ASTNode*> children;
};

struct TypeNode : ASTNode {
    TypeNode() : ASTNode(AST_TYPE) {
    }
    VarType varType;
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

struct CharNode : ASTNode {
    CharNode() : ASTNode(AST_CHAR) {
    }
    char value;
};

struct OpNode : ASTNode {
    OpNode() : ASTNode(AST_OPERATOR) {
    }
    std::string operation; // TODO enum
};

struct StatementNode : ASTNode {
    StatementNode() : ASTNode(AST_STATEMENT) {
    }
    StatementType type;
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