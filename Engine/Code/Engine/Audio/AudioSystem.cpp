#include "Engine/Math/Random.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/FileIO/XmlUtils.hpp"
#include "Engine/Core/ContainerUtils.hpp"

#include "ThirdParty/fmod/fmod.hpp"

/*
To disable audio entirely (and remove requirement for fmod.dll / fmod64.dll) for any
game, #define ENGINE_DISABLE_AUDIO in your game's Code/Game/EngineBuildPreferences.hpp
file.

Note that this #include is an exception to the rule "engine code doesn't know about
game code".
Purpose: Each game can now direct the engine via #defines to build differently.
Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
*/
#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_AUDIO )



// Link in the appropriate FMOD static library (32-bit or 64-bit)
//
#if defined( _WIN64 )
#pragma comment( lib, "ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "ThirdParty/fmod/fmod_vc.lib" )
#endif



// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//
AudioSystem::AudioSystem()
    : m_fmodSystem( nullptr )
{
    FMOD_RESULT result;
    result = FMOD::System_Create( &m_fmodSystem );
    ValidateResult( result );

    result = m_fmodSystem->init( 512, FMOD_INIT_NORMAL, nullptr );
    ValidateResult( result );
}



AudioSystem::~AudioSystem()
{
    FMOD_RESULT result = m_fmodSystem->release();
    ValidateResult( result );
    m_fmodSystem = nullptr;
}



void AudioSystem::BeginFrame()
{
    m_fmodSystem->update();
}



void AudioSystem::EndFrame()
{
}

SoundID AudioSystem::CreateSound( const string& soundFilePath )
{
    auto found = m_registeredSoundIDs.find( soundFilePath );
    if( found != m_registeredSoundIDs.end() )
    {
        return found->second;
    }
    else
    {
        FMOD::Sound* newSound = nullptr;
        m_fmodSystem->createSound(
            soundFilePath.c_str(), FMOD_DEFAULT, nullptr, &newSound );

        if( newSound )
        {
            SoundID newSoundID = m_registeredSounds.size();
            m_registeredSoundIDs[soundFilePath] = newSoundID;
            m_registeredSounds.push_back( newSound );
            return newSoundID;
        }
        else
        {
            LOG_ASSET_LOAD_FAILED( soundFilePath.c_str() );
            return MISSING_SOUND_ID;
        }
    }
}

SoundID AudioSystem::GetSound( const string& soundFilePath )
{
    auto found = m_registeredSoundIDs.find( soundFilePath );
    if( found != m_registeredSoundIDs.end() )
    {
        return found->second;
    }
    else
    {
        LOG_ASSET_NOT_LOADED( soundFilePath.c_str() );
        return MISSING_SOUND_ID;
    }
}

PlaybackID AudioSystem::PlaySound(
    SoundID soundID, bool isLooped, float volume, float balance, float speed,
    bool isPaused )
{
    size_t numSounds = m_registeredSounds.size();
    if( soundID < 0 || soundID >= numSounds )
        return MISSING_SOUND_ID;

    FMOD::Sound* sound = m_registeredSounds[soundID];
    if( !sound )
        return MISSING_SOUND_ID;

    FMOD::Channel* channelAssignedToSound = nullptr;
    m_fmodSystem->playSound( sound, nullptr, isPaused, &channelAssignedToSound );
    if( channelAssignedToSound )
    {
        int loopCount = isLooped ? -1 : 0;
        unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
        float frequency;
        channelAssignedToSound->setMode( playbackMode );
        channelAssignedToSound->getFrequency( &frequency );
        channelAssignedToSound->setFrequency( frequency * speed );
        channelAssignedToSound->setVolume( volume );
        channelAssignedToSound->setPan( balance );
        channelAssignedToSound->setLoopCount( loopCount );
    }

    return (PlaybackID) channelAssignedToSound;
}


PlaybackID AudioSystem::PlaySoundFromPath(
    const string& soundFilePath, bool isLooped/*=false*/, float volume/*=1.f*/,
    float balance/*=0.0f*/, float speed/*=1.0f*/, bool isPaused/*=false */ )
{
    if( soundFilePath.empty() )
        LOG_WARNING( "soundFilePath is empty string" );

    SoundID soundID = GetSound( soundFilePath );
    return PlaySound( soundID, isLooped, volume, balance, speed, isPaused );
}


void AudioSystem::StopSound( PlaybackID playbackID )
{
    if( MissingPlaybackID( playbackID ) )
        return;

    FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) playbackID;
    channelAssignedToSound->stop();
}


void AudioSystem::SetPauseSound( PlaybackID playbackID, bool paused )
{
    if( MissingPlaybackID( playbackID ) )
        return;

    FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) playbackID;
    channelAssignedToSound->setPaused( paused );
}



// Volume is in [0,1]
//
void AudioSystem::SetSoundPlaybackVolume( PlaybackID playbackID, float volume )
{
    if( MissingPlaybackID( playbackID ) )
        return;

    FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) playbackID;
    channelAssignedToSound->setVolume( volume );
}



// Balance is in [-1,1], where 0 is L/R centered
//
void AudioSystem::SetSoundPlaybackBalance( PlaybackID playbackID, float balance )
{
    if( MissingPlaybackID( playbackID ) )
        return;

    FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) playbackID;
    channelAssignedToSound->setPan( balance );
}



// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetSoundPlaybackSpeed( PlaybackID playbackID, float speed )
{
    if( MissingPlaybackID( playbackID ) )
        return;

    FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) playbackID;
    float frequency;
    FMOD::Sound* currentSound = nullptr;
    channelAssignedToSound->getCurrentSound( &currentSound );
    if( !currentSound )
        return;

    int ignored = 0;
    currentSound->getDefaults( &frequency, &ignored );
    channelAssignedToSound->setFrequency( frequency * speed );
}



void AudioSystem::ValidateResult( int result )
{
    if( result != FMOD_OK )
    {
        ERROR_RECOVERABLE( Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", (int) result ) );
    }
}


PlaybackID AudioSystem::PlayOneOffSoundFromGroup( string groupName )
{
    if( ContainerUtils::ContainsKey( m_registeredGroups, groupName ) )
    {
        AudioGroup& group = m_registeredGroups[groupName];
        int numSounds = (int)group.soundIDs.size();
        if( numSounds == 0 )
            return MISSING_SOUND_ID;
        int soundToPlay = Random::Default()->IntInRange( 0, numSounds - 1 );
        return PlaySound( group.soundIDs[soundToPlay] );
    }
    return MISSING_SOUND_ID;
}

void AudioSystem::LoadAudioGroups( string datafile )
{
    XMLDocument doc;
    doc.LoadFromFile( datafile );
    XMLElement root = doc.FirstChild();

    // loop groups
    for( XMLElement groupEl = root.FirstChild(); groupEl.Valid();
         groupEl = groupEl.NextSibling() )
    {
        AudioGroup group{};
        XMLAttribute nameAttr = groupEl.FirstAttribute();
        group.name = nameAttr.Value();

        // Loop clips
        for( XMLElement clipEl = groupEl.FirstChild(); clipEl.Valid();
             clipEl = clipEl.NextSibling() )
        {
            string soundPath = clipEl.FirstAttribute().Value();
            SoundID soundID = CreateSound( soundPath );
            group.soundIDs.push_back( soundID );
        }
        m_registeredGroups[group.name] = group;
    }

}

bool AudioSystem::MissingPlaybackID( PlaybackID playbackID )
{
    if( playbackID == MISSING_SOUND_ID )
    {
        ERROR_RECOVERABLE( "WARNING: attempt use missing sound playback ID!" );
        return true;
    }
    return false;
}

#endif // !defined( ENGINE_DISABLE_AUDIO )
