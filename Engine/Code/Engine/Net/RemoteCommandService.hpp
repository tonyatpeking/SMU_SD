#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/IConsoleObserver.hpp"
#include "Engine/Core/SmartEnum.hpp"

SMART_ENUM(
    RCS_State,

    INITIAL,
    CLIENT,
    HOST,
    HOST_FAILED
)

class Timer;
class TCPSocket;

class RemoteCommandService : IConsoleObserver
{
public:
    static RemoteCommandService* GetDefault();

	RemoteCommandService(){};
	~RemoteCommandService(){};

    void StartUp();
    void ShutDown();

    void Update();

    void RenderWidget();


    void ShouldHost( const String& service = "" );

    void ShouldJoin( const NetAddress& addr );

    void ShouldJoin( const String& addr );

    bool SendMsg( int idx, bool isEcho, const char* str );

    bool SendMsg( TCPSocket* sock, bool isEcho, const char* str );

    void RemoteCommandAll( const char* str );

    void RemoteCommandAllButMe( const char* str );

    void SetEchoOn( bool echoOn );

private:

    // Console will call this when RCS is observing
    void OnConsolePrint( const String& str ) override;

    void InitialState_Update();
    void HostState_Update();
    void ClientState_Update();
    void HostFailedState_Update();

    void SetState( RCS_State state );

    bool TryJoin( const NetAddress& addr );
    bool TryHost();
    void Disconnect();

    void StartDelayTimer();
    bool IsDelayOver();

    bool IsHostAddrLocal();

    NetAddress GetLocalHostAddr();

    TCPSocket* GetSocketByIndex( int idx );


    void ProcessReceives();

    bool ProcessReceive( TCPSocket* sock );

    void ProcessAccepts();

    void RemoveClosedConnections();

    bool m_echoOn = true;
    bool m_shouldDisconnect = false;
    bool m_shouldHost = false;
    const String m_defautService = "29283";
    String m_service = "29283";
    float m_failDelay = 1.f;

    RCS_State m_rcs_state = RCS_State::INITIAL;
    NetAddress m_hostAddress;

    TCPSocket* m_hostListenSocket = nullptr;

    std::vector<TCPSocket*> m_connectedSockets;

    TCPSocket* m_echoToSocket = nullptr;

    Timer* m_delayTimer = nullptr;

};
