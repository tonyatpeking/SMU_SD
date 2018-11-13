#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/NetDefines.hpp"


class NetMessageDefinition
{
public:
    NetMessageDefinition(
        const string& name,
        NetMessageCB cb,
        eNetMessageFlag flags,
        uint8 messageId,
        uint8 channelIdx );
    ~NetMessageDefinition() {};

    // called during construction to compute header size based on flags
    void CalculateHeaderSize();

    const string& GetName() const { return m_name; }
    MessageID GetMessageID() const { return m_id; }
    uint GetHeaderSize() const { return m_headerSize; }
    uint GetChannelIdx() const { return m_channelIdx; }
    bool HasCallback() const { return m_callback != nullptr; }

    // Flag checks
    bool RequiresConnection() const;
    bool IsClientOnly() const;
    bool IsHostOnly() const;
    bool IsReliable() const;
    bool IsInOrder() const;

public:

    string m_name;
    NetMessageCB m_callback = nullptr;
    MessageID m_id = NET_MESSAGE_ID_INVALID;
    uint8 m_channelIdx;
    eNetMessageFlag m_flags;
    uint m_headerSize; // in bytes
};
