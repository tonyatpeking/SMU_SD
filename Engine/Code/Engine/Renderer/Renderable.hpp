#pragma once
#include "Engine/Math/Mat4.hpp"
#include "Engine/Renderer/InputLayout.hpp"

class Material;
class Mesh;

//  Renderable Hierarchy
//      Mesh
//          Mesh
//              VertexBuffer
//              IndexBuffer
//              DrawInstruction
//
//      ModelMatrix
//
//      Material
//          Textures + Samplers
//          Properties
//          Shader
//              ShaderPass
//                  ShaderProgram
//                  RenderState

class Renderable
{
public:
    Renderable();
    virtual ~Renderable();
    Renderable( const Renderable& ) = delete;
    void operator=( const Renderable& ) = delete;

    void SetModelMatrix( const Mat4& model ) { m_modelMatrix = model; };
    Mat4& GetModelMatrix() { return m_modelMatrix; };
    Vec3 GetPosition();

    void SetMaterial( uint matID, Material* mat );
    Material* GetMaterial( uint matID ) { return m_materials[matID]; };
    uint GetMaterialCount() { return (uint) m_materials.size(); };

    void SetMesh( Mesh* mesh );
    Mesh* GetMesh() { return m_mesh; };

    // this should be removed after we have resource counting
    void DeleteMesh();

    void CreateInputLayoutsIfDirty();
    void FreeInputLayouts();
    void BindInputLayoutForProgram( ShaderProgram* program );

    static Renderable* MakeCube();
    static Renderable* MakeQuad();
    static Renderable* MakeUVSphere();


    vector<Material*> m_materials;
    Mat4 m_modelMatrix;


    //AKA VAO
    vector<InputLayout> m_inputLayouts;

private:
    void SetDirty();
    void ClearDirty();

    Mesh* m_mesh = nullptr;

    bool m_isDirty = true;
    // internal use, assumes program and vertex buffer and already bound
    // binds index buffer inside
    InputLayout CreateInputLayout( uint programHandle );
    // assumes vao, vertex buffer are bound
    void BindMeshToProgram( uint programHandle );
};
