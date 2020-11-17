#ifndef __NETWORKPROTOCOL_H_
#define __NETWORKPROTOCOL_H_

const unsigned short ServerPort = 52312;

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
    UpdateClientState,
  };
}

namespace Client
{
	// Packets originated in the client
	enum PacketType
	{
		PlayerEvent,
		//RequestCoopPartner,
		PositionUpdate,
		GameEvent,
		Quit
	};
}


#endif // __NETWORKPROTOCOL_H_
