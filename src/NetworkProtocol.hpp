#ifndef __NETWORKPROTOCOL_H_
#define __NETWORKPROTOCOL_H_

const unsigned short ServerPort = 51781;

namespace Server
{
  // Packets originated in the server
  enum PacketType
  {
    BroadcastMessage, // format: [Int32:packetType] [string:message]
    GameStarted,
    InitialState,
    PlayerEvent,
    PlayerRealtimeChange,
    PlayerConnect,
    PlayerDisconnect,
    //AcceptCoopPartner,
    PositionUpdate, // position x, position y
    TurnUpdate,
    WinnerUpdate,
    UpdateClientState,
    PlayerData
  };
}

namespace Client
{
    // Packets originated in the client
    enum PacketType
    {
      //PlayerEvent,
      //RequestCoopPartner,
      PositionUpdate, // turn, position.x, position.y
      GameEvent,
      Quit
    };
}


#endif // __NETWORKPROTOCOL_H_
