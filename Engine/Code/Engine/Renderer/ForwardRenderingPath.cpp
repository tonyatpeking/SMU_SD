#include <algorithm>
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/Frustum.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/DrawCall.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Profiler.hpp"
void ForwardRenderingPath::Render( RenderSceneGraph* scene )
{
    m_scene = scene;

    PROFILER_PUSH( MakeShadowMap );
    // Render ShadowMaps
    auto& lights = scene->GetLights();
    for( auto& go : lights )
    {
        Light* light = (Light*) go;
        if( light->IsCastShadow() )
        {
            RenderShadowMapForLight( light );
        }
    }
    PROFILER_POP();

    PROFILER_PUSH( RenderSceneForCameras );
    //Sort by camera draw order
    //scene->SortCameras();
    auto& cameras = scene->GetCameras();
    for( auto camera : cameras )
    {
        RenderSceneForCamera( camera );
    }
    // optimization
    // - Sort by material and state... search radix sort rendering
    PROFILER_POP();

}

void ForwardRenderingPath::RenderShadowMapForLight( Light* light )
{
    m_renderer->BindShadowTextureAsInput( false );

    UpdateShadowCamera( light, m_renderer->GetMainCamera() );

    Camera* shadowCamera = m_renderer->GetShadowCamera();

    m_renderer->UseCamera( shadowCamera );

    m_renderer->ClearDepth();

    m_renderer->SetShadowMapVP( shadowCamera->GetVPMatrix() );

    m_renderer->SetOverrideShader( ShaderPass::GetDepthOnlyShader() );

    auto& renderables = m_scene->GetRenderables();
    for( auto renderable : renderables )
    {
        if( !renderable->GetMesh() )
            continue;
        uint meshCount = renderable->GetMesh()->GetSubMeshCount();
        // Loop sub-mesh/materials
        for( uint subMeshID = 0; subMeshID < meshCount; ++subMeshID )
        {
            m_renderer->DrawRenderablePass( renderable,
                                            subMeshID,
                                            0 );

        }
    }

    // end override
    m_renderer->SetOverrideShader( nullptr );

    m_renderer->BindShadowTextureAsInput( true );
}

void ForwardRenderingPath::RenderSceneForCamera( Camera* camera )
{
    m_renderer->UseCamera( camera );

    for( auto& go : m_scene->GetGameObjects() )
        go->PreRender( camera );

    //ClearBasedOnCameraOptions( camera );

    std::vector<DrawCall> drawCalls;

    // Generate the draw calls
    auto& renderables = m_scene->GetRenderables();
    for( auto renderable : renderables )
    {
        // setup draw call(s) for this renderable
        uint meshCount = renderable->GetMesh()->GetSubMeshCount();

        // Loop sub-mesh/materials
        for( uint subMeshID = 0; subMeshID < meshCount; ++subMeshID )
        {
            Material* mat = renderable->GetMaterial( subMeshID );
            uint shaderPassCount = mat->GetShaderPassCount();

            // Loop shader passes
            for( uint shaderPassID = 0; shaderPassID < shaderPassCount; ++shaderPassID )
            {
                // Create draw call
                DrawCall drawCall( renderable, subMeshID, shaderPassID );
                if( mat->GetShaderPass( shaderPassID )->UsesLights() )
                {
                    int lights[MAX_LIGHTS];
                    ComputeMostContributingLights( renderable->GetPosition(), lights );
                    drawCall.SetLights( lights );
                }
                drawCalls.push_back( drawCall );
            }

        }
    }

    // Sort draw calls by layer/queue
    SortDrawCalls( drawCalls );
    // sort alpha by distance to camera - extra

    for( DrawCall& drawCall : drawCalls )
    {
        EnableLightsForDrawCall( drawCall );
        m_renderer->DrawRenderablePass( drawCall.m_renderable,
                                        drawCall.m_subMeshID,
                                        drawCall.m_shaderPassID );


    }

    //
    for( Material *effect : camera->m_effects )
    {
        m_renderer->ApplyEffect( effect );
    }

}

namespace
{
// sorting struct and compare function used in ComputeMostContributingLights
struct LightSortStruct
{
    uint lightIdx;
    float lightContribution;
};

bool ContributionCompare( const LightSortStruct& lhs, const LightSortStruct& rhs )
{
    return lhs.lightContribution > rhs.lightContribution;
};
}

void ForwardRenderingPath::ComputeMostContributingLights( const Vec3& pos,
                                                          int out_lightIndices[] )
{


    std::vector<LightSortStruct> lightsSorted;
    std::vector<GameObject*>& lights = m_scene->GetLights();
    lightsSorted.reserve( lights.size() );
    for( uint lightIdx = 0; lightIdx < lights.size(); ++lightIdx )
    {
        Light* light = (Light*) lights[lightIdx];
        float contribution = light->GetContributionToPoint( pos );
        lightsSorted.push_back( LightSortStruct{ lightIdx,contribution } );
    }
    std::sort( lightsSorted.begin(), lightsSorted.end(), ContributionCompare );


    for( uint outLightIdx = 0; outLightIdx < MAX_LIGHTS; ++outLightIdx )
    {
        if( outLightIdx >= lightsSorted.size() )
            out_lightIndices[outLightIdx] = -1;
        else
            out_lightIndices[outLightIdx] = lightsSorted[outLightIdx].lightIdx;
    }
}

void ForwardRenderingPath::SortDrawCalls( std::vector<DrawCall>& out_drawCalls )
{
    std::vector<Ints> queueBuckets;
    queueBuckets.resize( RenderQueue::COUNT );
    for( uint drawCallIdx = 0; drawCallIdx < out_drawCalls.size(); drawCallIdx++ )
    {
        DrawCall& dc = out_drawCalls[drawCallIdx];
        queueBuckets[dc.m_queue].push_back( drawCallIdx );
    }


    std::map<int, Ints> sortOrderBuckets;
    for( int queueBucketIdx = 0; queueBucketIdx < queueBuckets.size(); ++queueBucketIdx )
    {
        Ints& bucket = queueBuckets[queueBucketIdx];
        for( int idxInBucket = 0; idxInBucket < bucket.size(); ++idxInBucket )
        {
            uint dcIdx = bucket[idxInBucket];
            DrawCall& dc = out_drawCalls[dcIdx];
            sortOrderBuckets[dc.m_sortOrder].push_back( dcIdx );
        }
    }

    std::vector<DrawCall> sortedDrawCalls;
    sortedDrawCalls.reserve( out_drawCalls.size() );

    for( auto& pair : sortOrderBuckets )
    {
        Ints& bucket = pair.second;
        for( auto& dcIdx : bucket )
        {
            sortedDrawCalls.push_back( out_drawCalls[dcIdx] );
        }
    }
    out_drawCalls = sortedDrawCalls;
}

void ForwardRenderingPath::EnableLightsForDrawCall( const DrawCall& drawCall )
{
    m_renderer->DisableAllLights();
    for( int lightSlot = 0; lightSlot < MAX_LIGHTS; ++lightSlot )
    {
        int lightIdx = drawCall.m_lightIndices[lightSlot];
        if( lightIdx == -1 )
            continue;

        Light* light = (Light*) m_scene->GetLights()[lightIdx];
        m_renderer->SetLight( lightSlot, light );
    }
}

void ForwardRenderingPath::UpdateShadowCamera( Light* light, Camera* mainCamera )
{
    Vec4 pointInDistance = Vec4( 0.0f, 0.0f, 32.0f, 1.0f );
    Vec4 clipPoint = mainCamera->GetProjMatrix() * pointInDistance;
    Vec4 ndcPoint = clipPoint / clipPoint.w;

    //float shadowMapCoverageNDC = 0.95f; // Range is [-1, 1]

    Camera* shadowCam = Renderer::GetDefault()->GetShadowCamera();

    Mat4 lightModel = light->GetTransform().GetLocalToParent();


    Mat4 shadowCamRot = lightModel.GetRotationalPart();
    Mat4 shadowCamRotInv = shadowCamRot.InverseRotation();

    Frustum mainCamFrustum = Frustum::FromMatrixAABB3(
        mainCamera->GetVPMatrix(),
        AABB3( Vec3::NEG_ONES, Vec3( 1, 1, ndcPoint.z ) ) );

    // transform frustum points to light's space
    AABB3 lightSpaceAABB{};
    const Vec3* frustumCorners = mainCamFrustum.GetCorners();

    for( int cornerIdx = 0; cornerIdx < 8; ++cornerIdx )
    {
        Vec3 lightSpacePoint = shadowCamRotInv.TransformPosition(
            frustumCorners[cornerIdx] );
        lightSpaceAABB.StretchToIncludePoint( lightSpacePoint );

    }

    AABB2 orthoBounds = AABB2( Vec2::ZEROS, lightSpaceAABB.GetWidth(),
                               lightSpaceAABB.GetHeight() );
    float halfOrthoDepth = lightSpaceAABB.GetDepth() / 2.f;
    float near = -halfOrthoDepth -100;
    float far = halfOrthoDepth;
    shadowCam->SetProjectionOrtho( orthoBounds, near, far );

    Mat4 shadowCamView = shadowCamRot;
    Vec3 lightSpaceLocalCenter = lightSpaceAABB.GetCenter();
    Vec3 lightSpaceWorldCenter = shadowCamRot.TransformPosition( lightSpaceLocalCenter );

    shadowCamView.T = Vec4( lightSpaceWorldCenter, 1 );
    shadowCam->GetTransform().SetLocalToParent( shadowCamView );

    //shadowCam->SetFrustumVisible( true, Rgba::BLUE_CYAN );

}

