#pragma once
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Core/Rgba.hpp"

class Texture;
class ShaderPass;
class ShaderProgram;

class Material
{
public:
    static Material* CloneDefaultMaterial();

    Material();
    ~Material();
    Material* Clone();
    void SetProgram( ShaderProgram* program );
    void SetDiffuse( const Texture* texture );
    void SetNormal( const Texture* normal );
    void SetRenderState( RenderState state );
    void SetTint( const Rgba& tint ) { m_tint = tint; };
    uint GetShaderPassCount() { return 1; };//#placeholder
    ShaderPass* GetShaderPass( uint shaderPassID ) ;
    void SetShaderPass( uint shaderPassID, ShaderPass* shaderPass ) ;
    uint GetProgramHandle( uint shaderPassID );
    //#TODO parse material from file
    const Texture* m_diffuse = nullptr;
    const Texture* m_normal = nullptr;
    ShaderPass* m_shaderPass = nullptr;
    Rgba m_tint = Rgba::WHITE;
    float m_specularAmount = 0.7f;
    float m_specularPower = 30.f;
    bool m_isResource = true;
};