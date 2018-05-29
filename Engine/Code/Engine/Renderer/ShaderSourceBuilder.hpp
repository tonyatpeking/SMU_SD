#pragma once
#include "Engine/Core/Types.hpp"


class ShaderProgram;

// has meta info for each line
struct LineEntry
{
    // this is an index of s_includedFilePaths
    int originFile = 0;
    // the line number in the original file
    int localLineNum = 0;
    LineEntry( int file, int line )
        : originFile( file )
        , localLineNum( line ) {}
};

typedef std::vector<LineEntry> LineEntries;

class ShaderSourceBuilder
{
public:
    ShaderSourceBuilder() {};
    ~ShaderSourceBuilder() {};


    String Finalize(
        String& source,
        const String& filePath,
        const String& defines = "",
        // this is where the generated files will go, if blank will go to filePath
        const String& generatedFilepath = "" );

    int GetRealFileLine( int lineStitched, String& out_filePath )  const;

    bool m_hasIncludes = false;

private:
    // filePath should be the file the string was read from
    // if string was not from a file, filePath should indicate where the string came from
    // e.g. "Injected defines" to indicate the string was an injected define

    void InitShaderLoader( String& source, const String& filePath );

    bool InsertDefinesToShaderSource( const String defines );
    bool InsertIncludesToShaderSource();
    String GetFilePathFromInclude( const String& includeLine );

    // use the NoComment versions of sources
    // this checks for '*' since all comments should have been replaced with *
    bool ErrorIfLineEndsWithComment( const String& line );


    // auto updates s_lineEntries
    // keeps g_mainSource and g_mainSourceNoComment in sync
    // strings are expected to have \n endings
    void MoveToSource( int lineNum, int fileIdx, Strings&& toMove,
                       Strings&& toMoveNoComment );
    void MoveToStrings( int lineNum, Strings& toReceive, Strings&& toMove );
    void RemoveLineFromSource( int lineNum );
    void CommentLineFromSource( int lineNum );

    LineEntries MakeFileLineEntries( int fileIdx, int lineCount );
    void InsertLineEntries( int replacePos, int fileIdx, int lineCount );

    bool FileHasBeenAdded( const String& filePath );
    // will modify source so do not use after
    bool ReadSourceToLines( String& source, Strings& out_sourceLines,
                            Strings& out_sourceLineNoComment );
    String GetSourceFilepath();
    String GetCombinedSource();
    String GetCombinedLineMeta();

    // #defines and #includes will be appended to this
    Strings m_mainSource;
    // not really no comments but comments are replaced with /******/ //*****
    Strings m_mainSourceNoComment;

    // files that have already been added
    Strings m_includedFilePaths;

    LineEntries m_lineEntries;

};
