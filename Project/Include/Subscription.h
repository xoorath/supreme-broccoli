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


    Subscription::Token Subscription::Add(Lambda0 func)
    {
        IncrementingToken++;
        Subscribers[IncrementingToken] = func;
        return IncrementingToken;
    }

    void Subscription::Remove(Token token)
    {
        auto found = Subscribers.find(token);
        if (found != Subscribers.end())
        {
            Subscribers.erase(found);
        }
    }

    void Subscription::Execute()
    {
        for (auto &kvp : Subscribers)
        {
            kvp.second();
        }
    }
}