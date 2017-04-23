#pragma once

#include <Include/Types.h>

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


template<typename T>
class Subscription1
{
public:
    typedef int32 Token;

    Token Add(Lambda<void(T)> func) {
        IncrementingToken++;
        Subscribers[IncrementingToken] = func;
        return IncrementingToken;
    }
    void Remove(Token token) {
        auto found = Subscribers.find(token);
        if (found != Subscribers.end())
        {
            Subscribers.erase(found);
        }
    }

    void Execute(T param) {
        for (auto &kvp : Subscribers)
        {
            kvp.second(param);
        }
    }

private:
    Token IncrementingToken = 0;
    Map<Token, Lambda<void(T)>> Subscribers;
};

}