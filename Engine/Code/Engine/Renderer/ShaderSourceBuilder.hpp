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

typedef vector<LineEntry> LineEntries;

class ShaderSourceBuilder
{
public:
    ShaderSourceBuilder() {};
    ~ShaderSourceBuilder() {};


    string Finalize(
        string& source,
        const string& filePath,
        const string& defines = "",
        // this is where the generated files will go, if blank will go to filePath
        const string& generatedFilepath = "" );

    int GetRealFileLine( int lineStitched, string& out_filePath )  const;

    bool m_hasIncludes = false;

private:
    // filePath should be the file the string was read from
    // if string was not from a file, filePath should indicate where the string came from
    // e.g. "Injected defines" to indicate the string was an injected define

    void InitShaderLoader( string& source, const string& filePath );

    bool InsertDefinesToShaderSource( const string defines );
    bool InsertIncludesToShaderSource();
    string GetFilePathFromInclude( const string& includeLine );

    // use the NoComment versions of sources
    // this checks for '*' since all comments should have been replaced with *
    bool ErrorIfLineEndsWithComment( const string& line );


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

    bool FileHasBeenAdded( const string& filePath );
    // will modify source so do not use after
    bool ReadSourceToLines( string& source, Strings& out_sourceLines,
                            Strings& out_sourceLineNoComment );
    string GetSourceFilepath();
    string GetCombinedSource();
    string GetCombinedLineMeta();

    // #defines and #includes will be appended to this
    Strings m_mainSource;
    // not really no comments but comments are replaced with /******/ //*****
    Strings m_mainSourceNoComment;

    // files that have already been added
    Strings m_includedFilePaths;

    LineEntries m_lineEntries;

};
