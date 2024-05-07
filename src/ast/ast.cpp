#include "ast/ast.h"
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
