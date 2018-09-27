#pragma once


#include <string>
#include <vector>
#include <map>
#include "Engine/Core/EngineCommonH.hpp"

typedef size_t GroupID;
typedef size_t SoundID;
typedef size_t PlaybackID;
// for bad SoundIDs and SoundPlaybackIDs
constexpr size_t MISSING_SOUND_ID = (size_t) ( -1 );

struct AudioGroup
{
    vector<SoundID>soundIDs;
    string name;
};

class AudioSystem;

namespace FMOD
{
class Sound;
class System;
}


class AudioSystem
{
public:
    AudioSystem();
    virtual ~AudioSystem();

public:
    void BeginFrame();
    void EndFrame();

    SoundID CreateSound( const string& soundFilePath );
    SoundID GetSound( const string& soundFilePath );
    PlaybackID PlaySound(
        SoundID soundID, bool isLooped=false, float volume=1.f, float balance=0.0f,
        float speed=1.0f, bool isPaused=false );
    PlaybackID PlaySoundFromPath(
        const string& soundFilePath, bool isLooped=false, float volume=1.f,
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

    void ValidateResult( int result );

    // Audio groups
    PlaybackID PlayOneOffSoundFromGroup( string groupName );
    void LoadAudioGroups( string datafile );
protected:

    bool MissingPlaybackID( PlaybackID playbackID );

    FMOD::System* m_fmodSystem;
    map< string, SoundID >	m_registeredSoundIDs;
    vector< FMOD::Sound* > m_registeredSounds;
    map<string, AudioGroup> m_registeredGroups;
};

