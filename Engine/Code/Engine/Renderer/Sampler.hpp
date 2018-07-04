#pragma once
#include "Engine/Core/Types.hpp"

class Sampler
{
public:

    static Sampler* GetPointSampler();
    static Sampler* GetTrilinearSampler();
    static Sampler* GetShadowSampler();

    Sampler();
    ~Sampler();
    bool Create();
    void Destroy();
    uint GetHandle() const { return m_samplerHandle; };
private:
    uint m_samplerHandle = 0;
};