#pragma once
#include <string>
#include <map>
#include "Engine/Core/EngineCommonH.hpp"
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
        const string& rootPath,
        const string& defines = "" );
    static ShaderProgram* CreateOrGetFromStrings(
        const string& name,
        const string& vertShader,
        const string& fragShader,
        const string& defines = "",
        const string& vsFilepath = "",
        const string& fsFilepath = "",
        //this is if you want correct error reporting for built in shaders
        int vsFileLineOffset = 0,
        int fsFileLineOffset = 0,
        const string& generatedFilepath = "");
    static void UpdateAllShadersFromDisk();
    // this will not load the shaderProgram if it has not been loaded yet
    static ShaderProgram* GetProgram( const string& name, bool disableWarning = false );

    void UpdateShaderFromFile(
        bool updateDefines = false, const string& defines = "" );

    void UpdateShaderFromString(
        const string& vertShader,
        const string& fragShader,
        bool updateDefines = false,
        const string& defines = "" );

    uint GetHandle() const { return m_programHandle; };

    static map < string, ShaderProgram* > s_loadedShaders;

    bool m_isOverrideProgram = false;

private:
    ShaderProgram();
    ~ShaderProgram();

    bool CreateOrUpdate(
        const string& vertShader,
        const string& fragShader,
        bool updateDefines = false,
        const string& defines = "" );
    void LogShaderError(uint shaderId, const ShaderSourceBuilder& builder, uint type);
    uint CompileShaderFromString( const string& str, uint type, const string& defines );
    string m_defines;
    string m_vsFilepath;
    string m_fsFilepath;
    string m_generatedFilepath;
    int m_vsFileLineOffset = 0;
    int m_fsFileLineOffset = 0;
    uint m_programHandle = NULL;
    string m_rootPath = "";
    bool m_fromFile = false;
    // if is debug program, will ignore missing input layout when trying to use
    // Renderable::BindInputLayoutForProgram
};