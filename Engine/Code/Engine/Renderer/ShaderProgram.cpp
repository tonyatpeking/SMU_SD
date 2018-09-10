#include <sstream>

#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/Types.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/FileIO/IOUtils.hpp"
#include "Engine/Renderer/GLFunctions.hpp"
#include "Engine/Renderer/ShaderSourceBuilder.hpp"
#include "Engine/Renderer/BuiltinShader.hpp"


// internal functions and vars
namespace
{



// filepath should be the file the string was read from
// if string was not from a file, filepath should indicate where the string came from
// e.g. "Injected defines" to indicate the string was an injected define

uint CreateAndLinkProgram( int vs, int fs );
void LogProgramError( uint program_id );

//--------------------------------------------------------------------------------------
// Function definitions

uint CreateAndLinkProgram( int vs, int fs )
{
    // create the program handle - how you will reference
    // this program within OpenGL, like a texture handle
    uint programId = glCreateProgram();
    if( programId == NULL )
        LOG_WARNING( "glCreateProgram unsuccessful" );
    // Attach the shaders you want to use
    glAttachShader( programId, vs );
    glAttachShader( programId, fs );

    // Link the program (create the GPU program)
    glLinkProgram( programId );

    // Check for link errors - usually a result
    // of incompatibility between stages.
    GLint link_status;
    glGetProgramiv( programId, GL_LINK_STATUS, &link_status );

    if( link_status == GL_FALSE )
    {
        LogProgramError( programId );
        glDeleteProgram( programId );
        programId = 0;
    }

    // no longer need the shaders, you can detach them if you want
    // (not necessary)
    glDetachShader( programId, vs );
    glDetachShader( programId, fs );

    return programId;
}

void LogProgramError( uint programId )
{
    // get the buffer length
    GLint length;
    glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &length );

    // copy the log into a new buffer
    char *buffer = new char[length + 1];
    glGetProgramInfoLog( programId, length, &length, buffer );

    // print it to the output pane
    buffer[length] = NULL;
    LOG_WARNING( buffer );
    // cleanup
    delete buffer;
}

}

// End namespace
//--------------------------------------------------------------------------------------

std::map < String, ShaderProgram* > ShaderProgram::s_loadedShaders;

uint ShaderProgram::CompileShaderFromString(
    const String& str, uint type, const String& defines )
{
    if( str.empty() )
    {
        LOG_WARNING( "Shader is empty" );
        return NULL;
    }

    uint shaderId = glCreateShader( type );
    if( shaderId == NULL )
        LOG_WARNING( "Failed to create shader" );

    ShaderSourceBuilder builder;

    String strCopy = str;
    String filepath;
    String generatedFilepath = m_generatedFilepath;
    if( GL_VERTEX_SHADER == type )
    {
        filepath = m_vsFilepath;
        if( !generatedFilepath.empty() )
            generatedFilepath += ".vs";
    }
    if( GL_FRAGMENT_SHADER == type )
    {
        filepath = m_fsFilepath;
        if( !generatedFilepath.empty() )
            generatedFilepath += ".fs";
    }
    String finalSource = builder.Finalize( strCopy, filepath, defines,
                                        generatedFilepath );
    GLint shaderLength = (GLint) finalSource.size();
    const char *c_str = finalSource.c_str();
    glShaderSource( shaderId, 1, &c_str, &shaderLength );
    glCompileShader( shaderId );

    GLint status;
    glGetShaderiv( shaderId, GL_COMPILE_STATUS, &status );
    if( status == GL_FALSE )
    {
        LogShaderError( shaderId, builder, type );
        glDeleteShader( shaderId );
        shaderId = NULL;
    }

    return shaderId;
}



void ShaderProgram::LogShaderError( uint shaderId, const ShaderSourceBuilder& builder,
                                    uint type )
{

    // figure out how large the buffer needs to be
    GLint length;
    glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &length );

    // make a buffer, and copy the log to it.
    char *buffer = new char[length + 1];
    glGetShaderInfoLog( shaderId, length, &length, buffer );

    // Print it out (may want to do some additional formatting)
    buffer[length] = NULL;

    int errorFileLine = 0;
    String errorMsg;


    if( buffer[0] == 'E' ) // probably Intel formatting
    {
        Strings errorLogLines;
        StringUtils::ParseToTokens( String( buffer ), errorLogLines, "\n", false );
        for( unsigned int errorLineNum = 0;
             errorLineNum < errorLogLines.size(); ++errorLineNum )
        {
            if( errorLogLines[errorLineNum].empty() )
                continue;;
            Strings errorLogTokens;
            StringUtils::ParseToTokens( String( errorLogLines[errorLineNum] ),
                                        errorLogTokens, ":", false );
            errorLogTokens.resize( 3 );
            SetFromString( errorLogTokens[2], errorFileLine );
            errorMsg = String( errorLogLines[errorLineNum] );

        }
    }

    else // probably Nvidia formatting
    {
        Strings errorLogTokens;
        StringUtils::ParseToTokens( String( buffer ), errorLogTokens, "(", false );
        errorLogTokens.resize( 2 );
        String afterParentheses = errorLogTokens[1];
        StringUtils::ParseToTokens( afterParentheses, errorLogTokens, ")", false );
        errorLogTokens.resize( 1 );
        SetFromString( errorLogTokens[0], errorFileLine );
        errorMsg = String( String( buffer ) );
    }
    String realFilepath;
    int realFileLine = builder.GetRealFileLine( errorFileLine, realFilepath ) - 1;

    String originFile;
    String combinedFilepath = m_generatedFilepath;
    if( GL_VERTEX_SHADER == type )
    {
        originFile = m_vsFilepath;
        realFileLine += m_vsFileLineOffset;
        if( !combinedFilepath.empty() )
            combinedFilepath += ".vs";
    }
    if( GL_FRAGMENT_SHADER == type )
    {
        originFile = m_fsFilepath;
        realFileLine += m_fsFileLineOffset;
        if( !combinedFilepath.empty() )
            combinedFilepath += ".fs";
    }

    if( builder.m_hasIncludes )
    {
        if( combinedFilepath.empty() )
            combinedFilepath = originFile;
        combinedFilepath =  IOUtils::GetCurrentDir() + "/" + combinedFilepath + ".combined.txt";
        Log( combinedFilepath, "", errorFileLine, LOG_LEVEL_WARNING, "", errorMsg );
    }

    if( m_fromFile )
        realFilepath = IOUtils::GetCurrentDir() + "/" + realFilepath;

    Log( realFilepath.c_str(), "", realFileLine, LOG_LEVEL_WARNING, "", errorMsg );
    // free up the memory we used.
    delete buffer;
}

ShaderProgram* ShaderProgram::GetDefaultProgram()
{
    static ShaderProgram* s_defaultProgram = nullptr;
    if( !s_defaultProgram )
    {
        s_defaultProgram = CreateOrGetFromStrings(
            BUILTIN_SHADER, BUILTIN_VERTEX_SHADER, BUILTIN_FRAGMENT_SHADER, "",
            BUILTIN_SHADER_FILE, BUILTIN_SHADER_FILE,
            BUILTIN_VERTEX_SHADER_LINE, BUILTIN_FRAGMENT_SHADER_LINE,
            BUILTIN_GENERATE_PATH );
    }
    return s_defaultProgram;
}

ShaderProgram* ShaderProgram::GetInvalidProgram()
{
    static ShaderProgram* s_invalidProgram = nullptr;
    if( !s_invalidProgram )
    {
        s_invalidProgram = CreateOrGetFromStrings(
            INVALID_SHADER, INVALID_VERTEX_SHADER, INVALID_FRAGMENT_SHADER, "",
            BUILTIN_SHADER_FILE, BUILTIN_SHADER_FILE,
            INVALID_VERTEX_SHADER_LINE, INVALID_FRAGMENT_SHADER_LINE,
            INVALID_GENERATE_PATH );
        s_invalidProgram->m_isOverrideProgram = true;
    }
    return s_invalidProgram;
}



ShaderProgram* ShaderProgram::GetDebugProgram()
{
    static ShaderProgram* s_debugProgram = nullptr;
    if( !s_debugProgram )
    {
        s_debugProgram = CreateOrGetFromStrings(
            DEBUG_SHADER, BUILTIN_VERTEX_SHADER, BUILTIN_FRAGMENT_SHADER, "",
            BUILTIN_SHADER_FILE, BUILTIN_SHADER_FILE,
            BUILTIN_VERTEX_SHADER_LINE, BUILTIN_FRAGMENT_SHADER_LINE,
            DEBUG_GENERATE_PATH );
        s_debugProgram->m_isOverrideProgram = true;
    }
    return s_debugProgram;
}

ShaderProgram* ShaderProgram::GetLitProgram()
{
    static ShaderProgram* s_litProgram = nullptr;
    if( !s_litProgram )
    {
        s_litProgram = CreateOrGetFromFiles(
            "Data/Shaders/lit" );
    }
    return s_litProgram;
}

ShaderProgram* ShaderProgram::GetDepthOnlyProgram()
{
    static ShaderProgram* s_program = nullptr;
    if( !s_program )
    {
        s_program = CreateOrGetFromFiles(
            "Data/Shaders/DepthOnly" );
        s_program->m_isOverrideProgram = true;
    }
    return s_program;
}

void ShaderProgram::Bind()
{
    glUseProgram( m_programHandle );
}

ShaderProgram* ShaderProgram::CreateOrGetFromFiles(
    const String& rootpath,
    const String& defines /*= "" */ )
{
    String vsFile = rootpath + ".vs";
    String fsFile = rootpath + ".fs";
    String vsSource = IOUtils::ReadFileToString( vsFile.c_str() );
    String fsSource = IOUtils::ReadFileToString( fsFile.c_str() );
    ShaderProgram* shader = CreateOrGetFromStrings(
        rootpath, vsSource, fsSource, defines, vsFile, fsFile );
    if( shader )
        shader->m_fromFile = true;
    return shader;
}

ShaderProgram* ShaderProgram::CreateOrGetFromStrings(
    const String& name,
    const String& vertShader,
    const String& fragShader,
    const String& defines, /*= ""*/
    const String& vsFilepath, /*= "From string" */
    const String& fsFilepath, /*= "From string" */
    int vsFileLineOffset,
    int fsFileLineOffset,
    const String& generatedFilepath )
{
    // Try getting first
    ShaderProgram* shaderProgram = GetProgram( name, true );
    if( shaderProgram )
        return shaderProgram;

    shaderProgram = new ShaderProgram();
    shaderProgram->m_vsFilepath = vsFilepath;
    shaderProgram->m_fsFilepath = fsFilepath;
    shaderProgram->m_vsFileLineOffset = vsFileLineOffset;
    shaderProgram->m_fsFileLineOffset = fsFileLineOffset;
    shaderProgram->m_generatedFilepath = generatedFilepath;
    bool success = shaderProgram->CreateOrUpdate(
        vertShader, fragShader, true, defines );
    if( !success )
    {
        delete shaderProgram;
        shaderProgram = nullptr;
    }
    else
    {
        s_loadedShaders[name] = shaderProgram;
    }
    return shaderProgram;
}

void ShaderProgram::UpdateAllShadersFromDisk()
{
    for( auto& shader : s_loadedShaders )
    {
        if( shader.second->m_fromFile )
            shader.second->UpdateShaderFromFile();
    }
}

ShaderProgram* ShaderProgram::GetProgram( const String& name, bool disableWarning )
{
    auto iter = s_loadedShaders.find( name );
    if( s_loadedShaders.end() == iter )
    {
        if( !disableWarning )
            LOG_WARNING( "could not get shader, please load it first: " + name );
        return nullptr;
    }
    return iter->second;
}

void ShaderProgram::UpdateShaderFromFile(
    bool updateDefines /*= false*/,
    const String& defines /*= "" */ )
{
    String vsSource = IOUtils::ReadFileToString( m_vsFilepath.c_str() );
    String fsSource = IOUtils::ReadFileToString( m_fsFilepath.c_str() );
    CreateOrUpdate( vsSource, fsSource, updateDefines, defines );
}

void ShaderProgram::UpdateShaderFromString(
    const String& vertShader,
    const String& fragShader,
    bool updateDefines /*= false*/,
    const String& defines /*= "" */ )
{
    CreateOrUpdate( vertShader, fragShader, updateDefines, defines );
}

//--------------------------------------------------------------------------------------
// End of unnamed namespace

ShaderProgram::ShaderProgram()
{

}

ShaderProgram::~ShaderProgram()
{
    if( m_programHandle != NULL )
        glDeleteProgram( m_programHandle );
}

bool ShaderProgram::CreateOrUpdate(
    const String& vertShader,
    const String& fragShader,
    bool updateDefines,
    const String& defines
)
{
    if( m_programHandle != NULL )
        glDeleteProgram( m_programHandle );

    if( updateDefines )
        m_defines = defines;

    uint vert_shader = CompileShaderFromString( vertShader, GL_VERTEX_SHADER,
                                                defines );
    uint frag_shader = CompileShaderFromString( fragShader, GL_FRAGMENT_SHADER,
                                                defines );

    if( vert_shader == NULL || frag_shader == NULL )
        m_programHandle = NULL;
    else
        m_programHandle = CreateAndLinkProgram( vert_shader, frag_shader );
    glDeleteShader( vert_shader );
    glDeleteShader( frag_shader );

    return ( m_programHandle != NULL );
}



