﻿
#include "Engine/Core/CommandSystem.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/FileIO/IOUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Profiler/ProfilerReport.hpp"
#include "Engine/Profiler/ProfilerReportEntry.hpp"

#include "Game/GameCommands.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Net/UDPTest.hpp"


void GameCommands::RegisterAllCommands()
{
    CommandSystem* commandSys = CommandSystem::DefaultCommandSystem();

    commandSys->AddCommand( "udpTestStart", []( String str )
    {
        CommandParameterParser parser( str );

        String portStr = "";
        if( parser.NumOfParams() > 0 )
        {
            parser.GetNext( portStr );
        }

        UNUSED( str );
        UDPTest::GetInstance()->Start( portStr );
    } );

    commandSys->AddCommand( "udpTestStop", []( String str )
    {
        UNUSED( str );
        UDPTest::GetInstance()->Stop();
    } );


    commandSys->AddCommand( "udpTestSend", []( String str )
    {
        CommandParameterParser parser( str );

        String addrStr;

        parser.GetNext( addrStr );

        NetAddress addr(addrStr);

        String msg;

        parser.GetNext( msg );

        Printf( "Sending Message..." );

        UDPTest::GetInstance()->SendTo(addr,msg.c_str(),msg.size());
    } );


    commandSys->AddCommand( "echo", []( String str )
    {
        CommandParameterParser parser( str );
        String msg;
        parser.GetNext( msg );
        Rgba color = Rgba::GREEN;
        if( parser.NumOfParams() > 1 )
            parser.GetNext( color );

        if( parser.AllParseSuccess() )
            Print( msg, color );
    } );


    commandSys->AddCommand( "profilerReport", []( String str )
    {
        CommandParameterParser parser( str );
        size_t numParams = parser.NumOfParams();
        bool useTreeMode = true;
        bool sortByTotalTime = true;
        if( numParams > 0 )
        {
            String param;
            parser.GetNext( param );
            if( "flat" == param || "Flat" == param )
                useTreeMode = false;
            if( numParams > 1 )
            {
                parser.GetNext( param );
                if( "self" == param || "Self" == param )
                    sortByTotalTime = false;
            }
        }

        Profiler::Measurement* lastFrame = Profiler::GetPreviousFrame();
        ProfilerReport report;
        if( useTreeMode )
        {
            report.GenerateReportTreeFromFrame( lastFrame );
        }
        else
        {
            report.GenerateReportFlatFromFrame( lastFrame );
        }

        if( sortByTotalTime )
        {
            report.SortByTotalTime();
        }
        else
        {
            report.SortBySelfTime();
        }


        String reportStr = report.ToString();
        Print( reportStr );

    } );


    commandSys->AddCommand( "add", []( String str )
    {
        CommandParameterParser parser( str );

        size_t numParams = parser.NumOfParams();
        if( numParams < 2 )
        {
            LOG_WARNING( "add needs at least 2 params" );
            return;
        }

        float sum = 0;
        for( size_t paramIdx = 0; paramIdx < parser.NumOfParams(); ++paramIdx )
        {
            float param;
            parser.GetNext( param );
            sum += param;
        }

        if( parser.AllParseSuccess() )
            Print( ToString( sum ) );
    } );



    commandSys->AddCommand( "quit", []( String str )
    {
        g_app->OnQuitRequested();
    } );

    commandSys->AddCommand( "quit()", []( String str )
    {
        g_app->OnQuitRequested();
    } );

    commandSys->AddCommand( "cls", []( String str )
    {
        g_console->Clear();
    } );



    commandSys->AddCommand( "dump", []( String str )
    {
        CommandParameterParser parser( str );
        String path;
        parser.GetNext( path );

        if( !parser.AllParseSuccess() )
            return;

        g_console->SetOutputFilePath( path );
        bool dumpSuccess = g_console->OutputToFile();
        if( dumpSuccess )
            Print( "log dumped to file: " + path, Rgba::CYAN );
        else
            Print( "failed to dump to file: " + path, Rgba::YELLOW );
    } );

    commandSys->AddCommand( "health", []( String str )
    {
        CommandParameterParser parser( str );
        String mode;
        parser.GetNext( mode );

        if( !parser.AllParseSuccess() )
            return;

        if( mode == "always" )
            g_config->healthBarRenderMode = RenderMode::ALWAYS;
        else if( mode == "never" )
            g_config->healthBarRenderMode = RenderMode::NEVER;
        else if( mode == "normal" )
            g_config->healthBarRenderMode = RenderMode::NORMAL;
        else
            LOG_WARNING( "wrong format for command, supported modes are always, normal, never e.g \"health always\"" );
    } );

    commandSys->AddCommand( "echoFile", []( String str )
    {
        CommandParameterParser parser( str );
        String filePath;
        parser.GetNext( filePath );

        if( !parser.AllParseSuccess() )
            return;

        bool canOpenFile = IOUtils::CanOpenFile( filePath.c_str() );
        if( !canOpenFile )
        {
            Print( "Could not open file: " + filePath );
            return;
        }

        String fileContents = IOUtils::ReadFileToString( filePath.c_str() );
        Print( fileContents );
    } );

    commandSys->AddCommand( "get", []( String str )
    {
        CommandParameterParser parser( str );
        String flag;
        parser.GetNext( flag );
        if( !parser.AllParseSuccess() )
            return;
        Print( ToString( GetFlag( flag ) ) );
    } );

    commandSys->AddCommand( "set", []( String str )
    {
        CommandParameterParser parser( str );
        String flag;
        parser.GetNext( flag );
        if( !parser.AllParseSuccess() )
            return;
        SetFlag( flag );
        Print( "set flag [" + flag + "] to true" );
    } );

    commandSys->AddCommand( "clear", []( String str )
    {
        CommandParameterParser parser( str );
        String flag;
        parser.GetNext( flag );
        if( !parser.AllParseSuccess() )
            return;
        SetFlag( flag, false );
        Print( "cleared flag [" + flag + "] to false" );
    } );

    commandSys->AddCommand( "toggle", []( String str )
    {
        CommandParameterParser parser( str );
        String flag;
        parser.GetNext( flag );
        if( !parser.AllParseSuccess() )
            return;
        bool before = GetFlag( flag );
        ToggleFlag( flag );
        Print( "flag [" + flag + "] was " + ToString( before ) );
        Print( "set flag [" + flag + "] to " + ToString( !before ) );
    } );

    commandSys->AddCommand( "flags", []( String str )
    {
        auto allFlags = GameFlag::AllValues();
        for( auto& flag : allFlags )
        {
            Print( flag.ToString() + " " + ToString( GetFlag( flag ) ) );
        }
    } );

    commandSys->AddCommand( "debugClear", []( String str )
    {
        DebugRender::Clear();
    } );

    commandSys->AddCommand( "debugHide", []( String str )
    {
        DebugRender::SetHidden( true );
    } );

    commandSys->AddCommand( "debugShow", []( String str )
    {
        DebugRender::SetHidden( false );
    } );

    // Console printf
    //Printf( Rgba::RED, "this is an int: %d, this is a float: %f  Lets see what happens when I type a really loooooooooooooooooooooooooooooooooooooooooooong line. Oh look it wraps around", 12, 123.3 );

    //commandSys->RunCommand( "echo Hello" );

    // All of these formats will work
//     g_console->InputWithEnter( "add(3,4)" );
//     g_console->InputWithEnter( "add 3,4" );
//     g_console->InputWithEnter( "add(3 4)" );
//     g_console->InputWithEnter( "add 3 4" );
//     g_console->InputWithEnter( "add    3  ,  4   " );
//     g_console->InputWithEnter( "add 1 2 3 4 5 6 7 8 9 10" );

    // These will fail with error output to console
//     g_console->InputWithEnter( "add" );
//     g_console->InputWithEnter( "add()" );
//     g_console->InputWithEnter( "add ,," );
//     g_console->InputWithEnter( "add d a" );

//     g_console->InputWithEnter( "echo Hello" );
//     g_console->InputWithEnter( "echo Hello (255,111,111,255)" );
    // this does not parse the color correctly, anything inside brackets need ','
    //g_console->InputWithEnter( "echo Hello (255 111 111 255)" );

    //g_console->InputWithEnter( "clear" );
//     g_console->InputWithEnter( "help" );
//
//     g_console->InputWithEnter( "dump log.txt" );



}

