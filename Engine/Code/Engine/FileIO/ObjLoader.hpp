#pragma once
#include "Engine/Core/Types.hpp"

class MeshBuilder;
class Vec3;
class Vec2;


class ObjLoader
{
public:
    static MeshBuilder LoadFromFile( const char* filePath );
private:

    enum LineType
    {
        UNKNOWN,
        POSITION,
        NORMAL,
        UV,
        FACE_ELEMENT,
        MATERIAL_SEPARATOR,
    };

    struct FaceElement
    {
        int m_posIdx = -1;
        int m_normalIdx = -1;
        int m_uvIdx = -1;
    };

    typedef vector<FaceElement> FaceElements;

    ObjLoader() {};
    MeshBuilder LoadFromFileInternal( const char* filePath );
    bool LoadFileToLines( const char* filePath );
    void ParseLinesToBuffers();
    Vec3 ParseLineToVec3( uint lineIdx );
    Vec2 ParseLineToVec2( uint lineIdx );
    FaceElements ParseLineToFaceElements( uint lineIdx );
    void LogParseFailure();
    // just an empty FaceElement vector
    FaceElements& MaterialSeparator();

    // this also replaces the v,vt,vn,f with spaces
    LineType GetLineType( uint lineIdx );
    Strings m_fileLines;
    vector<Vec3> m_posBuffer;
    vector<Vec2> m_uvBuffer;
    vector<Vec3> m_normalBuffer;
    vector<FaceElements> m_faceBuffer;
    string m_filePath;
    uint m_errorLine = 0;
    bool m_skippedFirstMaterial = false;
};
