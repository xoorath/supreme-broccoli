#include "../Include/Subscription.h"

namespace XO
{

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