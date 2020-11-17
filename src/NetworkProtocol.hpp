#ifndef __NETWORKPROTOCOL_H_
#define __NETWORKPROTOCOL_H_

const unsigned short ServerPort = 5000;

namespace Server
{
  // Packets originated in the server
  enum PacketType
  {
    BroadcastMessage, // format: [Int32:packetType] [string:message]
    InitialState,
    PlayerEvent,
    PlayerRealtimeChange,
    PlayerConnect,
    PlayerDisconnect,
    AcceptCoopPartner,
    UpdateClientState,
  };
}


#endif // __NETWORKPROTOCOL_H_
