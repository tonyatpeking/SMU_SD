#pragma once
#include <string>
#include <map>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IRange.hpp"

class ShaderSourceBuilder;

class ShaderProgram
{
public:

    static ShaderProgram* GetDefaultProgram();
    static ShaderProgram* GetInvalidProgram();
    static ShaderProgram* GetDebugProgram();
    static ShaderProgram* GetLitProgram();
    static ShaderProgram* GetDepthOnlyProgram();

    // user of class must check for null and valid handle before calling bind
    void Bind();
    static ShaderProgram* CreateOrGetFromFiles(
        const String& rootPath,
        const String& defines = "" );
    static ShaderProgram* CreateOrGetFromStrings(
        const String& name,
        const String& vertShader,
        const String& fragShader,
        const String& defines = "",
        const String& vsFilepath = "",
        const String& fsFilepath = "",
        //this is if you want correct error reporting for built in shaders
        int vsFileLineOffset = 0,
        int fsFileLineOffset = 0,
        const String& generatedFilepath = "");
    static void UpdateAllShadersFromDisk();
    // this will not load the shaderProgram if it has not been loaded yet
    static ShaderProgram* GetProgram( const String& name, bool disableWarning = false );

    void UpdateShaderFromFile(
        bool updateDefines = false, const String& defines = "" );

    void UpdateShaderFromString(
        const String& vertShader,
        const String& fragShader,
        bool updateDefines = false,
        const String& defines = "" );

    uint GetHandle() const { return m_programHandle; };

    static std::map < String, ShaderProgram* > s_loadedShaders;

    bool m_isOverrideProgram = false;

private:
    ShaderProgram();
    ~ShaderProgram();

    bool CreateOrUpdate(
        const String& vertShader,
        const String& fragShader,
        bool updateDefines = false,
        const String& defines = "" );
    void LogShaderError(uint shaderId, const ShaderSourceBuilder& builder, uint type);
    uint CompileShaderFromString( const String& str, uint type, const String& defines );
    String m_defines;
    String m_vsFilepath;
    String m_fsFilepath;
    String m_generatedFilepath;
    int m_vsFileLineOffset = 0;
    int m_fsFileLineOffset = 0;
    uint m_programHandle = NULL;
    String m_rootPath = "";
    bool m_fromFile = false;
    // if is debug program, will ignore missing input layout when trying to use
    // Renderable::BindInputLayoutForProgram
};