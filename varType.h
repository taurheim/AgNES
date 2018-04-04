#pragma once
#include <map>

enum VarType {
    VT_NONE = 0,
    VT_INT,
    VT_CHAR,
    VT_BOOL,
    VT_ARRAY
    //STRUCT
};

static std::map<VarType, std::string> varTypeToString = {
    {VT_INT, "int"},
    {VT_CHAR, "char"},
    {VT_BOOL, "bool"},
    {VT_ARRAY, "array"},
};