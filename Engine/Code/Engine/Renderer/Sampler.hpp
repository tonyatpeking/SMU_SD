#pragma once
#include "Engine/Core/Types.hpp"

class Sampler
{
public:
    Sampler();
    ~Sampler();
    bool Create();
    void Destroy();
    uint GetHandle() const { return m_samplerHandle; };
private:
    uint m_samplerHandle = 0;
};