#include "Engine/Renderer/MeshPrimitive.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Math/Frustum.hpp"

MeshBuilder MeshPrimitive::MakeCube( const Vec3& sideLengths /*= Vec3::ONES*/,
                                     const Rgba& tint /*= Rgba::WHITE*/,
                                     const Vec3& centerPos /*= Vec3::ZEROS*/,
                                     const AABB2& uvTop /*= AABB2::ZEROS_ONES*/,
                                     const AABB2& uvSide /*= AABB2::ZEROS_ONES*/,
                                     const AABB2& uvBottom /*= AABB2::ZEROS_ONES */ )
{
    // face vertex order
    //                7-------6
    //               /|      /|
    //              3-|-----2 |
    //              | 4-----|-5
    //              |/   o  |/
    //              0-------1

    // face order
    // front back left right bottom top
    MeshBuilder mb{};
    mb.Reserve( 24, 36 );

    Vec3 halfLengths = sideLengths * 0.5f;
    Vec3 positions[8];

    // position vertex order
    // front layer   back layer
    // 3 2           7 6
    // 0 1           4 5
    //Front face m_position
    positions[0] = Vec3( -1, -1, -1 );
    positions[1] = Vec3( +1, -1, -1 );
    positions[2] = Vec3( +1, +1, -1 );
    positions[3] = Vec3( -1, +1, -1 );
    //Back face m_position
    positions[4] = Vec3( -1, -1, +1 );
    positions[5] = Vec3( +1, -1, +1 );
    positions[6] = Vec3( +1, +1, +1 );
    positions[7] = Vec3( -1, +1, +1 );

    //Resize and move positions
    for( uint posIdx = 0; posIdx < 8; ++posIdx )
        positions[posIdx] = positions[posIdx] * halfLengths + centerPos;

    // which position each index uses notice that 2 and 3 are swapped to
    // make it easy for the the cubeBorder shader to generate local uvs
    //  2 3
    //  0 1

    uint vertIdxToPosIdx[24] ={
        //Back
        0,1,3,2,
        //Front
        5,4,6,7,
        //Left
        4,0,7,3,
        //Right
        1,5,2,6,
        //Bottom
        4,5,0,1,
        //Top
        3,2,7,6
    };

    mb.BeginSubMesh();
    mb.SetColor( tint );


    uint posIdx;
    Vec3 sideNormals[4] ={ Vec3::BACKWARD,Vec3::FORWARD,Vec3::LEFT,Vec3::RIGHT };
    Vec3 sideTangents[4] ={ Vec3::RIGHT,Vec3::LEFT,Vec3::BACKWARD,Vec3::FORWARD };
    //Set side face verts
    for( uint sideIdx = 0; sideIdx < 4; ++sideIdx )
    {
        mb.SetNormal( sideNormals[sideIdx] );
        mb.SetTangent( sideTangents[sideIdx] );

        mb.SetUV( uvSide.mins );
        posIdx = vertIdxToPosIdx[sideIdx * 4 + 0];
        mb.PushPos( positions[posIdx] );

        mb.SetUV( uvSide.GetMaxXMinY() );
        posIdx = vertIdxToPosIdx[sideIdx * 4 + 1];
        mb.PushPos( positions[posIdx] );

        mb.SetUV( uvSide.GetMinXMaxY() );
        posIdx = vertIdxToPosIdx[sideIdx * 4 + 2];
        mb.PushPos( positions[posIdx] );

        mb.SetUV( uvSide.maxs );
        posIdx = vertIdxToPosIdx[sideIdx * 4 + 3];
        mb.PushPos( positions[posIdx] );
    }

    //bot and top face verts
    mb.SetNormal( Vec3::DOWN );
    mb.SetTangent( Vec3::RIGHT );
    mb.SetUV( uvBottom.mins );
    mb.PushPos( positions[4] );
    mb.SetUV( uvBottom.GetMaxXMinY() );
    mb.PushPos( positions[5] );
    mb.SetUV( uvBottom.GetMinXMaxY() );
    mb.PushPos( positions[0] );
    mb.SetUV( uvBottom.maxs );
    mb.PushPos( positions[1] );

    mb.SetNormal( Vec3::UP );
    mb.SetTangent( Vec3::RIGHT );
    mb.SetUV( uvTop.mins );
    mb.PushPos( positions[3] );
    mb.SetUV( uvTop.GetMaxXMinY() );
    mb.PushPos( positions[2] );
    mb.SetUV( uvTop.GetMinXMaxY() );
    mb.PushPos( positions[7] );
    mb.SetUV( uvTop.maxs );
    mb.PushPos( positions[6] );

    // push indices
    for( uint faceIdx = 0; faceIdx < 6; ++faceIdx )
    {
        uint verIdxOffset = faceIdx * 4;
        mb.AddFaceIdx( 0 + verIdxOffset,
                       1 + verIdxOffset,
                       3 + verIdxOffset,
                       2 + verIdxOffset );
    }

    mb.EndSubMesh();
    return mb;
}

MeshBuilder MeshPrimitive::MakeQuad( const AABB2& bounds /*= AABB2::NEG_ONES_ONES*/,
                                     const Rgba& tint /*= Rgba::WHITE*/,
                                     const AABB2& uvs /*= AABB2::ZEROS_ONES*/ )
{
    MeshBuilder mb{};
    mb.Reserve( 4, 6 );

    mb.BeginSubMesh();

    mb.AddQuad2D( bounds, tint, uvs );

    mb.EndSubMesh();
    return mb;
}

MeshBuilder MeshPrimitive::MakeUVSphere( float radius,
                                         int stacks,
                                         int slices,
                                         const Rgba& tint /*= Rgba::WHITE*/,
                                         const Vec3 centerPos /*= Vec3::ZEROS */ )
{
    int rows = stacks + 1;
    int cols = slices + 1;
    MeshBuilder mb{};
    mb.Reserve( rows * cols, stacks * slices * 6 );
    mb.BeginSubMesh();
    mb.SetColor( tint );

    // Generate points
    for( int stack = 0; stack <= stacks; stack++ ) // stacks are elevation
    {
        float v = (float) ( stack ) / stacks;
        float elevation = v * 180 - 90; // elevation [-90 90]
        for( int slice = 0; slice <= slices; slice++ ) // slices are azimuth
        {
            float u = (float) ( slice ) / slices;
            float azimuth = u * 360; // azimuth [0 360]

            mb.SetUV( Vec2( u, v ) );
            Vec3 pos = SphericalToCartesian( 1, elevation, azimuth );
            mb.SetNormal( pos );

            // tangent calculation
            float elevationOffset = 0;
            if( elevation == 90 )
                elevationOffset = -0.01f;
            else if( elevation == -90 )
                elevationOffset = 0.01f;

            Vec3 posRight = SphericalToCartesian( 1, elevation + elevationOffset,
                                                  azimuth + 0.1f );
            Vec3 posLeft = SphericalToCartesian( 1, elevation + elevationOffset,
                                                 azimuth - 0.1f );
            Vec3 tangent = ( posRight - posLeft ).GetNormalized();
            mb.SetTangent( tangent );

            pos = pos * radius + centerPos;
            mb.PushPos( pos );
        }
    }

    //azim1 azim2
    // ^    ^
    // 3----2 > elev2
    // |  / |
    // | /  |
    // 0----1 > elev1

//     12 13 14 15
//     8  9  10 11
//     4  5  6  7
//     0  1  2  3

    // make faces
    for( int stack = 0; stack < stacks; stack++ ) // rows are elevation
    {
        for( int slice = 0; slice < slices; slice++ ) // cols are azimuth
        {
            mb.AddFaceIdx(
                slice + stack * cols,
                slice + 1 + stack * cols,
                slice + 1 + ( stack + 1 ) * cols,
                slice + ( stack + 1 ) * cols
            );
        }
    }

    mb.EndSubMesh();
    return mb;
}

MeshBuilder MeshPrimitive::MakeLineStrip( const std::vector<Vec3>& points,
                                          const Rgba& startColor, const Rgba& endColor )
{
    MeshBuilder mb{};

    mb.Reserve( points.size(), 0 );
    mb.BeginSubMesh( DrawPrimitive::LINE_STRIP, false );

    for( int pointIdx = 0; pointIdx < points.size(); ++pointIdx )
    {
        float percentToEnd = float( pointIdx ) / ( points.size() - 1 );
        Rgba color = Lerp( startColor, endColor, percentToEnd );
        mb.SetColor( color );
        mb.PushPos( points[pointIdx] );
    }
    mb.EndSubMesh();
    return mb;
}

MeshBuilder MeshPrimitive::MakeFrustum( const Frustum& frustum )
{
    // just make 6 planes, don't really care about efficiency

    MeshBuilder mb{};
    mb.BeginSubMesh();
    mb.SetColor( Rgba::WHITE );
    //                7-------6
    //               /|      /|
    //              3-|-----2 |
    //              | 4-----|-5
    //              |/   o  |/
    //              0-------1
    const Vec3* corners = frustum.GetCorners();
    mb.AddFace( corners[1], corners[5], corners[6], corners[2] );
    mb.AddFace( corners[4], corners[0], corners[3], corners[7] );
    mb.AddFace( corners[3], corners[2], corners[6], corners[7] );
    mb.AddFace( corners[4], corners[5], corners[1], corners[0] );
    mb.AddFace( corners[7], corners[6], corners[5], corners[4] );
    mb.AddFace( corners[0], corners[1], corners[2], corners[3] );
    mb.EndSubMesh();

    mb.GenerateNormals();

    return mb;

}
