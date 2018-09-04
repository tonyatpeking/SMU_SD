#include "Engine/FileIO/ObjLoader.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/FileIO/IOUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"


MeshBuilder ObjLoader::LoadFromFile( const char* filePath )
{
    ObjLoader objLoader = ObjLoader();
    return objLoader.LoadFromFileInternal( filePath );
}

MeshBuilder ObjLoader::LoadFromFileInternal( const char* filePath )
{
    m_filePath = std::string( filePath );
    MeshBuilder mb = MeshBuilder{};

    if( !LoadFileToLines( filePath ) )
        return mb;
    ParseLinesToBuffers();

    mb.BeginSubMesh();

    for( int faceIdx = 0; faceIdx < m_faceBuffer.size(); ++faceIdx )
    {
        FaceElements& face = m_faceBuffer[faceIdx];

        // Material separator
        if( face.empty() )
        {
            mb.EndSubMesh();
            mb.BeginSubMesh();
            continue;
        }

        uint startVertIdx = mb.GetVertCount();
        for( int faceEleIdx = 0; faceEleIdx < face.size(); ++faceEleIdx )
        {
            FaceElement& faceEle = face[faceEleIdx];
            if( -1 != faceEle.m_normalIdx )
            {
                Vec3 normal = m_normalBuffer[faceEle.m_normalIdx - 1];
                normal.x = -normal.x;
                mb.SetNormal( normal );

                //calculate tangent
                Vec3 tangent = Cross( Vec3::RIGHT, normal );
                if( tangent == Vec3::ZEROS )
                    tangent = Cross( Vec3::UP, normal );
                tangent.NormalizeAndGetLength();
                mb.SetTangent( tangent );
            }
            if( -1 != faceEle.m_uvIdx )
                mb.SetUV( m_uvBuffer[faceEle.m_uvIdx - 1] );
            Vec3 pos = m_posBuffer[faceEle.m_posIdx - 1];
            pos.x = -pos.x;
            mb.PushPos( pos );
        }
        uint endVertIdx = mb.GetVertCount() - 1;
        mb.AddFaceIdxRange( startVertIdx, endVertIdx );
    }

    mb.EndSubMesh();
    return mb;
}

bool ObjLoader::LoadFileToLines( const char* filePath )
{
    String fileString = IOUtils::ReadFileToString( filePath );
    if( fileString.empty() )
    {
        LOG_WARNING( "Failed to load file: " + std::string( filePath ) );
        return false;
    }
    ParseStatus status = StringUtils::ParseToLines( fileString, m_fileLines );
    if( status != PARSE_SUCCESS && status != PARSE_EMPTY )
    {
        LogParseFailure();
        return false;
    }
    return true;
}

void ObjLoader::ParseLinesToBuffers()
{
    for( int lineIdx = 0; lineIdx < m_fileLines.size(); ++lineIdx )
    {
        LineType lineType = GetLineType( lineIdx );
        switch( lineType )
        {
        case UNKNOWN:
            break;
        case POSITION:
            m_posBuffer.push_back( ParseLineToVec3( lineIdx ) );
            break;
        case UV:
            m_uvBuffer.push_back( ParseLineToVec2( lineIdx ) );
            break;
        case NORMAL:
            m_normalBuffer.push_back( ParseLineToVec3( lineIdx ) );
            break;
        case FACE_ELEMENT:
            m_faceBuffer.push_back( ParseLineToFaceElements( lineIdx ) );
            break;
        case MATERIAL_SEPARATOR:
        {
            if( !m_skippedFirstMaterial )
            {
                m_skippedFirstMaterial = true;
                break;
            }
            else
            {
                m_faceBuffer.push_back( MaterialSeparator() );
            }
        }
        default:
            break;
        }
    }
}


Vec3 ObjLoader::ParseLineToVec3( uint lineIdx )
{
    String& str = m_fileLines[lineIdx];
    StringUtils::RemoveOuterWhitespace( str );
    Vec3 v3;
    ParseStatus status = v3.SetFromText( str, " " );
    if( ParseStatus::PARSE_SUCCESS != status )
    {
        m_errorLine = lineIdx;
        LogParseFailure();
    }
    return v3;
}

Vec2 ObjLoader::ParseLineToVec2( uint lineIdx )
{
    String& str = m_fileLines[lineIdx];
    StringUtils::RemoveOuterWhitespace( str );
    Vec2 v2;
    ParseStatus status = v2.SetFromText( str, " " );
    if( ParseStatus::PARSE_SUCCESS != status )
    {
        m_errorLine = lineIdx;
        LogParseFailure();
    }
    return v2;
}


ObjLoader::FaceElements ObjLoader::ParseLineToFaceElements( uint lineIdx )
{
    Strings elementStrings;
    FaceElements faceElements;
    String& str = m_fileLines[lineIdx];
    StringUtils::RemoveOuterWhitespace( str );
    StringUtils::ParseToTokens( str, elementStrings, " ", false );
    if( elementStrings.size() < 3 )
    {
        m_errorLine = lineIdx;
        LogParseFailure();
    }
    for( auto& faceElementStr : elementStrings )
    {
        Ints indices;
        FaceElement faceElement;
        StringUtils::ParseToInts( faceElementStr, indices, "/" );
        if( indices.size() < 1 )
            continue;
        faceElement.m_posIdx = indices[0];
        if( indices.size() > 1 )
            faceElement.m_uvIdx = indices[1];
        if( indices.size() > 2 )
            faceElement.m_normalIdx = indices[2];
        faceElements.push_back( faceElement );
    }
    return faceElements;
}

void ObjLoader::LogParseFailure()
{
    LOG_WARNING( "Parse Failure: " + m_filePath + " Line: "
                 + ToString( (int) m_errorLine ) );
}

ObjLoader::FaceElements& ObjLoader::MaterialSeparator()
{
    static FaceElements s_materialSeparator;
    return s_materialSeparator;
}

ObjLoader::LineType ObjLoader::GetLineType( uint lineIdx )
{
    String& str = m_fileLines[lineIdx];
    // 5 is the minimum to define something valid e.g. v 1 2
    if( str.size() < 5 )
        return UNKNOWN;
    char first = str[0];
    char second = str[1];
    char third = str[2];

    if( 'u' == first && str.find( "usemtl" ) != std::string::npos )
        return MATERIAL_SEPARATOR;

    // setting char to whitespace for easy parsing later on
    str[0] = ' ';
    if( 'v' == first && ' ' == second )
        return POSITION;

    if( 'f' == first && ' ' == second )
        return FACE_ELEMENT;

    str[1] = ' ';

    if( 'v' == first && 'n' == second && ' ' == third )
        return NORMAL;

    if( 'v' == first && 't' == second && ' ' == third )
        return UV;


    return UNKNOWN;
}
