#include "Engine/Renderer/ShaderSourceBuilder.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/FileIO/IOUtils.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"


int ShaderSourceBuilder::GetRealFileLine( int lineStitched, String& out_filePath ) const
{
    if( lineStitched > 0 )
        --lineStitched;
    LineEntry entry = m_lineEntries[lineStitched];
    if( -1 == entry.originFile )
        out_filePath = m_includedFilePaths[0] + "[injected #define]";
    else
        out_filePath = m_includedFilePaths[entry.originFile];
    return entry.localLineNum + 1;
}


String ShaderSourceBuilder::Finalize( String& source,
                                   const String& filePath,
                                   const String& defines /*= "" */,
                                   const String& generatedFilepath )
{
    InitShaderLoader( source, filePath );
    bool insertSuccess = InsertDefinesToShaderSource( defines );
    bool includeSuccess = InsertIncludesToShaderSource();
    if( m_hasIncludes )
    {
        String writeToFilepath;
        if( generatedFilepath.empty() )
            writeToFilepath = filePath;
        else
            writeToFilepath = generatedFilepath;
        IOUtils::WriteToFile( writeToFilepath + ".combined.txt",
                               GetCombinedSource() );
        IOUtils::WriteToFile( writeToFilepath + ".meta.txt",
                               GetCombinedLineMeta() );
    }
    if( !includeSuccess || !insertSuccess )
        return "";

    return GetCombinedSource();
}

void ShaderSourceBuilder::InitShaderLoader( String& source,
                                            const String& filePath )
{
    m_includedFilePaths.push_back( filePath );
    ReadSourceToLines( source, m_mainSource, m_mainSourceNoComment );
    m_lineEntries = MakeFileLineEntries( 0, (int) m_mainSource.size() );
}

bool ShaderSourceBuilder::InsertDefinesToShaderSource( const String defines )
{
    // find line with version,
    size_t versionPos = 0;
    size_t versionLine = 0;
    bool found = StringUtils::FindInStrings( m_mainSourceNoComment, "#version",
                                             versionLine, versionPos );
    if( !found )
    {
        LOG_WARNING( "version number not found in shader: " + m_includedFilePaths[0] );
        return false;
    }
    //check if the version line ends with a comment
    if( ErrorIfLineEndsWithComment( m_mainSourceNoComment[versionLine] ) )
        return false;

    // parse defines and add formatting
    Strings tokens;
    StringUtils::ParseToTokens( defines, tokens, ";", false, false );
    for( int tokenIdx = (int) tokens.size() - 1; tokenIdx >= 0; --tokenIdx )
    {
        String& token = tokens[tokenIdx];
        if( tokens[tokenIdx].empty() || StringUtils::IsAllWhitespace( tokens[tokenIdx] ) )
        {
            ContainerUtils::EraseAtIndexFast( tokens, tokenIdx );
            continue;
        }
        StringUtils::ReplaceAll( token, '=', ' ' );
        token = "#define " + token + "\n";
    }
    // copy is needed because the tokens are moved
    Strings tokensCpy = tokens;
    MoveToSource( (int) versionLine + 1, -1, std::move( tokens ), std::move( tokensCpy ) );
    return true;
}

bool ShaderSourceBuilder::InsertIncludesToShaderSource()
{
    while( true )
    {
        size_t includePos = 0;
        size_t includeLine = 0;
        bool found = StringUtils::FindInStrings( m_mainSourceNoComment, "#include",
                                                 includeLine, includePos );
        if( !found )
            return true;
        else
            m_hasIncludes = true;

        String includeLineStr = m_mainSourceNoComment[includeLine];
        //check if the include line ends with a comment
        if( ErrorIfLineEndsWithComment( includeLineStr ) )
            return false;
        String includeFilePath = GetFilePathFromInclude( includeLineStr );
        StringUtils::RemoveOuterWhitespace( includeFilePath );
        CommentLineFromSource( (int) includeLine );
        if( FileHasBeenAdded( includeFilePath ) )
            continue;

        String source = IOUtils::ReadFileToString( includeFilePath.c_str() );
        if( source.empty() )
        {

            String errorMsg = "Could not include file [" + includeFilePath + "]";
            String realFile;
            int realLine = GetRealFileLine( (int) includeLine + 1, realFile );
            Log( ( IOUtils::GetCurrentDir() + "/" + realFile ),
                 "", realLine, LOG_LEVEL_WARNING, "", errorMsg );
            return false;
        }

        m_includedFilePaths.push_back( includeFilePath );
        Strings includeSource;
        Strings includeSourceNoComment;
        ReadSourceToLines( source, includeSource, includeSourceNoComment );
        MoveToSource( (int) includeLine, (int) m_includedFilePaths.size() - 1,
                      std::move( includeSource ),
                      std::move( includeSourceNoComment ) );
    }
    return true;

}

//extracts "fullPath/abcd" from "#include \"abcd\""
String ShaderSourceBuilder::GetFilePathFromInclude( const String& includeLine )
{
    size_t start = includeLine.find( '\"' );
    size_t end = 0;
    if( String::npos != start )
        end = includeLine.find( '\"', start + 1 );

    if( String::npos == start || String::npos == end )
    {
        LOG_WARNING( "Bad include: " + includeLine );
        return includeLine;
    }

    return includeLine.substr( start + 1, end - start - 1 );
}

bool ShaderSourceBuilder::ErrorIfLineEndsWithComment( const String& line )
{
    if( line.size() < 2 )
        return false;
    if( '*' == line[line.size() - 2] )
    {
        LOG_WARNING( "ERROR '/*' on same line as #include / #version in shader" );
        return true;
    }
    return false;
}

void ShaderSourceBuilder::MoveToSource( int lineNum, int fileIdx, Strings&& toMove,
                                        Strings&& toMoveNoComment )
{
    if( toMove.empty() )
        return;
    MoveToStrings( lineNum, m_mainSource, std::move( toMove ) );
    MoveToStrings( lineNum, m_mainSourceNoComment, std::move( toMoveNoComment ) );
    InsertLineEntries( lineNum, fileIdx, (int) toMove.size() );
}

void ShaderSourceBuilder::MoveToStrings( int lineNum, Strings& toReceive,
                                         Strings&& toMove )
{

    toReceive.insert( toReceive.begin() + lineNum,
                      make_move_iterator( toMove.begin() ),
                      make_move_iterator( toMove.end() ) );
}

void ShaderSourceBuilder::RemoveLineFromSource( int lineNum )
{
    m_lineEntries.erase( m_lineEntries.begin() + lineNum );
    m_mainSource.erase( m_mainSource.begin() + lineNum );
    m_mainSourceNoComment.erase( m_mainSourceNoComment.begin() + lineNum );
}

void ShaderSourceBuilder::CommentLineFromSource( int lineNum )
{
    m_mainSource[lineNum] = "//" + m_mainSource[lineNum];
    m_mainSourceNoComment[lineNum] = "\n";
}

LineEntries ShaderSourceBuilder::MakeFileLineEntries( int fileIdx,
                                                      int lineCount )
{
    LineEntries lineEntries;
    lineEntries.reserve( lineCount );
    for( int lineNum = 0; lineNum < lineCount; ++lineNum )
        lineEntries.push_back( LineEntry( fileIdx, lineNum ) );
    return lineEntries;
}

void ShaderSourceBuilder::InsertLineEntries( int replacePos, int fileIdx,
                                             int lineCount )
{
    LineEntries entiresToInsert = MakeFileLineEntries( fileIdx, lineCount );
    m_lineEntries.reserve( m_lineEntries.size() + lineCount );
    m_lineEntries.insert( m_lineEntries.begin() + replacePos,
                          entiresToInsert.begin(), entiresToInsert.end() );
}

bool ShaderSourceBuilder::FileHasBeenAdded( const String& filePath )
{
    return ContainerUtils::Contains( m_includedFilePaths, filePath );
}

bool ShaderSourceBuilder::ReadSourceToLines( String& source,
                                             Strings& out_sourceLines,
                                             Strings& out_sourceLineNoComment )
{
    StringUtils::ParseToLines( source, out_sourceLines, false, true );
    StringUtils::ReplaceComments( source );
    StringUtils::ParseToLines( source, out_sourceLineNoComment, false, true );
    return true;
}

String ShaderSourceBuilder::GetSourceFilepath()
{
    return m_includedFilePaths[0];
}

String ShaderSourceBuilder::GetCombinedSource()
{
    return StringUtils::ConcatStrings( m_mainSource );
}

String ShaderSourceBuilder::GetCombinedLineMeta()
{
    Strings lineMeta;
    for( auto& entry : m_lineEntries )
    {
        lineMeta.push_back( Stringf( "file:%d, line:%d\n", entry.originFile, entry.localLineNum ) );
    }
    return StringUtils::ConcatStrings( lineMeta );
}
