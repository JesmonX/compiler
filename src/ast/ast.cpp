#include "ast/ast.h"
#include "ir/ir.h"
#include <iostream>
#include <fmt/core.h>
#include <cassert>


std::string prefix(int level)
{
    std::string s;
    for (int i = 0; i < level; i++)
        s += "    ";
    return s;
}

Type* typehandler(std::string ty)
{
    switch (ty[0])
    {
        case "i"[0]:
            return Type::getIntegerTy();
        case "v"[0]:
            return Type::getUnitTy();
        default:
            std::cout << "type er";
    }
}