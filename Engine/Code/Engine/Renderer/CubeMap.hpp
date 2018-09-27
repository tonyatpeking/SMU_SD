#pragma once
#include "Engine/Core/Types.hpp"
#include "Texture.hpp"
#include "Engine/Renderer/RendererEnums.hpp"

class CubeMap : public Texture
{
public:
    virtual ~CubeMap();
    CubeMap( const string& imageFilePath );
    CubeMap( Image* image );

    virtual void MakeFromImage( Image* image ) override;


private:
};
