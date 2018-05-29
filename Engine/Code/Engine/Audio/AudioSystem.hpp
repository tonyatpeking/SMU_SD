#pragma once



#include "ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>
#include "Engine/Core/EngineCommon.hpp"


typedef size_t SoundID;
typedef size_t PlaybackID;
// for bad SoundIDs and SoundPlaybackIDs
constexpr size_t MISSING_SOUND_ID = (size_t) ( -1 );



class AudioSystem;


class AudioSystem
{
public:
    AudioSystem();
    virtual ~AudioSystem();

public:
    void BeginFrame();
    void EndFrame();

    SoundID CreateSound( const String& soundFilePath );
    SoundID GetSound( const String& soundFilePath );
    PlaybackID PlaySound(
        SoundID soundID, bool isLooped=false, float volume=1.f, float balance=0.0f,
        float speed=1.0f, bool isPaused=false );
    PlaybackID PlaySoundFromPath(
        const String& soundFilePath, bool isLooped=false, float volume=1.f,
        float balance=0.0f, float speed=1.0f, bool isPaused=false );
    void StopSound( PlaybackID soundPlaybackID );
    void SetPauseSound( PlaybackID soundPlaybackID, bool paused );

    // volume is in [0,1]
    void SetSoundPlaybackVolume(
        PlaybackID soundPlaybackID, float volume );
    // balance is in [-1,1], where 0 is L/R centered
    void SetSoundPlaybackBalance(
        PlaybackID soundPlaybackID, float balance );
    // speed is frequency multiplier (1.0 == normal)
    void SetSoundPlaybackSpeed(
        PlaybackID soundPlaybackID, float speed );

    void ValidateResult( FMOD_RESULT result );

protected:

    bool MissingPlaybackID( PlaybackID playbackID );

    FMOD::System* m_fmodSystem;
    std::map< String, SoundID >	m_registeredSoundIDs;
    std::vector< FMOD::Sound* > m_registeredSounds;

};

