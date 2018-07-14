#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/MeshPrimitive.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/GLFunctions.hpp"

Renderable::~Renderable()
{
    for( auto& material : m_materials )
    {
        if( !material->m_isResource )
            delete material;
    }
    FreeInputLayouts();
}

Renderable::Renderable()
{
    m_materials.push_back( Material::CloneDefaultMaterial() );
}

Vec3 Renderable::GetPosition()
{
    return Vec3( m_modelMatrix.T );
}

void Renderable::SetMaterial( uint matID, Material* mat )
{
    // resize and fill with null
    while( m_materials.size() < matID + 1 )
    {
        m_materials.push_back( Material::CloneDefaultMaterial() );
    }

    m_materials[matID] = mat;
    SetDirty();
}

void Renderable::SetMesh( Mesh* mesh )
{
    m_mesh = mesh;
    SetDirty();
}

void Renderable::DeleteMesh()
{
    delete m_mesh;
    m_mesh = nullptr;
}

void Renderable::CreateInputLayoutsIfDirty()
{
    if( !m_isDirty )
        return;
    ClearDirty();
    FreeInputLayouts();
    uint subMeshCount = m_mesh->GetSubMeshCount();
    if( subMeshCount == 0 )
        return;
    uint materialCount = GetMaterialCount();
    if( materialCount < subMeshCount )
    {
        // this will fill in ALL the missing materials
        SetMaterial( subMeshCount - 1, Material::CloneDefaultMaterial() );
    }

    // bind the mesh verts and indices
    m_mesh->m_vertexBuffer.BindBuffer();

    for( uint subMeshID = 0; subMeshID < m_mesh->GetSubMeshCount(); ++subMeshID )
    {
        Material* mat = m_materials[subMeshID];
        uint shaderPassCount =mat->GetShaderPassCount();
        for( uint shaderPassID = 0; shaderPassID < shaderPassCount; ++shaderPassID )
        {
            uint programHandle = mat->GetProgramHandle( shaderPassID );
            mat->GetShaderPass( shaderPassID )->BindProgram();
            m_inputLayouts.push_back(CreateInputLayout( programHandle ));
        }
    }
}

void Renderable::FreeInputLayouts()
{
    for( int layoutIdx = 0; layoutIdx < m_inputLayouts.size(); ++layoutIdx )
    {
        glDeleteVertexArrays( 1, &m_inputLayouts[layoutIdx].m_vaoID );
    }
    m_inputLayouts.clear();
}

void Renderable::SetDirty()
{
    m_isDirty = true;
}

void Renderable::ClearDirty()
{
    m_isDirty = false;
}

InputLayout Renderable::CreateInputLayout( uint programHandle )
{
    InputLayout inputLayout;
    inputLayout.programHandle = programHandle;
    glGenVertexArrays( 1, &inputLayout.m_vaoID );
    glBindVertexArray( inputLayout.m_vaoID );

    BindMeshToProgram( programHandle );

    glBindVertexArray( NULL );
    GL_CHECK_ERROR();
    return inputLayout;
}

void Renderable::BindMeshToProgram( uint programHandle )
{
    m_mesh->m_indexBuffer.BindBuffer();

    const VertexLayout* vertexLayout = m_mesh->m_vertexLayout;
    uint attribCount = vertexLayout->GetAttributeCount();
    for( uint attribIdx = 0; attribIdx < attribCount; ++attribIdx )
    {
        const VertexAttribute *attrib = vertexLayout->GetAttribute( attribIdx );

        GLint bind = glGetAttribLocation( programHandle, attrib->m_name.c_str() );
        if( bind >= 0 )
        {
            glEnableVertexAttribArray( bind );
            glVertexAttribPointer( bind,
                                   attrib->m_elementCount,
                                   ToGLEnum( attrib->m_renderDataType ),
                                   ToGLEnum( attrib->m_isNormalized ),
                                   (uint) vertexLayout->m_stride,
                                   (GLvoid*) attrib->m_memberOffset );

        }
    }
}

void Renderable::BindInputLayoutForProgram( ShaderProgram* program )
{
    uint programHandle = program->GetHandle();
    for( auto& inputLayout : m_inputLayouts )
    {
        if( inputLayout.programHandle == programHandle )
        {
            glBindVertexArray( inputLayout.m_vaoID );
            return;
        }
    }
    if( program->m_isOverrideProgram )
    {
        InputLayout inputLayout = InputLayout::GetGlobalInputLayout();
        m_mesh->m_vertexBuffer.BindBuffer();
        glBindVertexArray( inputLayout.m_vaoID );
        BindMeshToProgram( programHandle );
    }
    else
    {
        LOG_WARNING( "Could not find input layout for shader program" );
    }
}

Renderable* Renderable::MakeCube()
{
    Renderable* renderable = new Renderable();
    renderable->m_mesh = MeshPrimitive::MakeCube().MakeMesh();
    return renderable;
}

Renderable* Renderable::MakeQuad()
{
    Renderable* renderable = new Renderable();
    renderable->m_mesh = MeshPrimitive::MakeQuad().MakeMesh();
    return renderable;
}

Renderable* Renderable::MakeUVSphere()
{
    Renderable* renderable = new Renderable();
    renderable->m_mesh = MeshPrimitive::MakeUVSphere().MakeMesh();
    return renderable;
}
