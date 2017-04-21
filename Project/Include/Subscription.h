#pragma once

#include "Types.h"

namespace XO
{
class Subscription
{
public:
    typedef int32 Token;

    Token Add(Lambda0 func);
    void Remove(Token token);

    void Execute();

private:
    Token IncrementingToken = 0;
    Map<Token, Lambda0> Subscribers;
};
}