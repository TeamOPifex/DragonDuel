#include "Server.h"

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"


unsigned char GetPacketIdentifier(RakNet::Packet *p);
void HandleServerMessage(GameMessage message, ui8* data, ui32 length, RakNet::RakNetGUID guid);

i8 GameServer::Start(ui16 port) {
	MessageHandler = HandleServerMessage;
	gameMatchIndex = 0;

	clientID = RakNet::UNASSIGNED_SYSTEM_ADDRESS;

	server = RakNet::RakPeerInterface::GetInstance();
	server->SetIncomingPassword("Rumpelstiltskin", (int)strlen("Rumpelstiltskin"));
	server->SetTimeoutTime(30000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);

	RakNet::SocketDescriptor socketDescriptors[2];
	socketDescriptors[0].port = port;
	socketDescriptors[0].socketFamily = AF_INET; // Test out IPV4
	socketDescriptors[1].port = port;
	socketDescriptors[1].socketFamily = AF_INET6; // Test out IPV6


	bool b = server->Startup(4, socketDescriptors, 2) == RakNet::RAKNET_STARTED;
	server->SetMaximumIncomingConnections(4);

	if (!b)
	{
		OPlogErr("Failed to start dual IPV4 and IPV6 ports. Trying IPV4 only.\n");

		// Try again, but leave out IPV6
		b = server->Startup(4, socketDescriptors, 1) == RakNet::RAKNET_STARTED;
		if (!b)
		{
			OPlogErr("Server failed to start.");

			return -1;
		}
	}

	OPlogInfo("Server started.");

	server->SetUnreliableTimeout(1000);

	DataStructures::List< RakNet::RakNetSocket2* > sockets;
	server->GetSockets(sockets);

	OPlogInfo("Socket addresses used by RakNet:");
	for (unsigned int i = 0; i < sockets.Size(); i++)
	{
		OPlogInfo("%i. %s", i + 1, sockets[i]->GetBoundAddress().ToString(true));
	}

	OPlogInfo("My IP addresses:");
	for (unsigned int i = 0; i < server->GetNumberOfAddresses(); i++)
	{
		RakNet::SystemAddress sa = server->GetInternalID(RakNet::UNASSIGNED_SYSTEM_ADDRESS, i);
		OPlogInfo("%i. %s (LAN=%i)", i + 1, sa.ToString(false), sa.IsLANAddress());
	}

	return 0;
}

// Currently set to 1MB
#define WORKING_PACKET_SIZE 1024
i8 SERVERPACKETDATA[WORKING_PACKET_SIZE];

void HandleServerMessage(GameMessage message, ui8* data, ui32 length, RakNet::RakNetGUID guid) {

	switch (message) {
		case ClientConnected: {
			OPlog("ClientConnected");
			break;
		}
		case BackgroundColor: {
			OPlog("BackgroundColor");
			// Send the data right back to the client
			SERVER.Send(BackgroundColor, data, length, guid);
			break;
		}
		case CreateMatch: {
			OPlog("CreateMatch");
			// Someone wants to create a new match
			SERVER.gameMatch[SERVER.gameMatchIndex].Init();
			CreateMatchResult result = {
				SERVER.gameMatchIndex
			};
			SERVER.Send(CreateMatch, (ui8*)&result, sizeof(CreateMatchResult));
			SERVER.gameMatchIndex++;
			break;
		}
		case JoinMatch: {
			OPlog("JoinMatch");
			// Someone wants to join a match
			JoinMatchMessage* join = (JoinMatchMessage*)data;
			ui32 playerIndex = SERVER.gameMatch[join->matchIndex].AddPlayer(guid);
			OPlog("Player %d has joined the match", playerIndex);

			JoinMatchResult result = {
				guid,
				SERVER.gameMatch[join->matchIndex].playerIndex - 1
			};
			for (ui32 i = 0; i < result.otherPlayers; i++) {
				result.players[i] = 
					SERVER.gameMatch[join->matchIndex].players[i];
			}

			// Tell the client that they joined successfully
			SERVER.Send(JoinMatch, (ui8*)&result, sizeof(JoinMatchResult), guid);

			// Tell everyone else that someone joined
			SomeoneJoinedResult result2 = {
				guid
			};
			SERVER.Send(SomeoneJoined, (ui8*)&result2, sizeof(SomeoneJoinedResult), guid, true);
			break;
		}
		case Ready: {
			OPlog("Ready");
			// Player has marked that they are ready to start the match
			break;
		}
		case PositionUpdate: {
			OPlog("PositionUpdate");
			Player* player = (Player*)data;
			player->guid = guid;
			for (ui32 i = 0; i < SERVER.gameMatchIndex; i++) {
				for (ui32 j = 0; j < SERVER.gameMatch[i].playerIndex; j++) {
					if (SERVER.gameMatch[i].players[j].guid == player->guid) {
						SERVER.gameMatch[i].players[j].Position = player->Position;
						break;
					}
				}
			}
			SERVER.Send(PositionUpdate, data, length, guid, true);
			break;
		}
		default: {
			OPlog("An Unknown Message");
		}
	}
}

void GameServer::HandleDisconnect(RakNet::RakNetGUID guid) {
	for (ui32 i = 0; i < gameMatchIndex; i++) {
		if (gameMatch[i].RemovePlayer(guid)) {
			SomeoneLeftMessage result = {
				i,
				guid
			};
			OPlog("Player %d has left the match", i);
			SERVER.Send(SomeoneLeft, (ui8*)&result, sizeof(SomeoneLeftMessage));
			return;
		}
	}
}

void GameServer::Send(GameMessage message, ui8* data, ui32 length) {
	ASSERT(length < WORKING_PACKET_SIZE - 1, "Packet was too large. Increase WORKING_PACKET_SIZE from %d to more than %d", WORKING_PACKET_SIZE, length);

	SERVERPACKETDATA[0] = message;
	OPmemcpy(&SERVERPACKETDATA[1], data, length);
	server->Send(SERVERPACKETDATA, length + sizeof(i8), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void GameServer::Send(GameMessage message, ui8* data, ui32 length, RakNet::RakNetGUID guid) {
	ASSERT(length < WORKING_PACKET_SIZE - 1, "Packet was too large. Increase WORKING_PACKET_SIZE from %d to more than %d", WORKING_PACKET_SIZE, length);

	SERVERPACKETDATA[0] = message;
	OPmemcpy(&SERVERPACKETDATA[1], data, length);
	server->Send(SERVERPACKETDATA, length + sizeof(i8), HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);
}

void GameServer::Send(GameMessage message, ui8* data, ui32 length, RakNet::RakNetGUID guid, i8 exclude) {
	ASSERT(length < WORKING_PACKET_SIZE - 1, "Packet was too large. Increase WORKING_PACKET_SIZE from %d to more than %d", WORKING_PACKET_SIZE, length);

	SERVERPACKETDATA[0] = message;
	OPmemcpy(&SERVERPACKETDATA[1], data, length);
	server->Send(SERVERPACKETDATA, length + sizeof(i8), HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, exclude);
}

void GameServer::HandleMessage(ui8* data, ui32 length, RakNet::RakNetGUID guid) {
	if (data == NULL) {
		return;
	}
	
	ASSERT(length < WORKING_PACKET_SIZE - 1, "Received packet was too large. Increase WORKING_PACKET_SIZE from %d to more than %d", WORKING_PACKET_SIZE, length);
	
	if (MessageHandler) {
		MessageHandler((GameMessage)data[0], &data[1], length - 1, guid);
	}
}

void GameServer::Update(OPtimer* timer) {
	i8 message[1024];
	const i8* msg = "Totes connected";

	for (p = server->Receive(); p; server->DeallocatePacket(p), p = server->Receive())
	{
		// We got a packet, get the identifier with our handy function
		packetIdentifier = GetPacketIdentifier(p);

		// Check if this is a network message packet
		switch (packetIdentifier)
		{
		case ID_DISCONNECTION_NOTIFICATION:
			// Connection lost normally
			HandleDisconnect(p->guid);
			printf("ID_DISCONNECTION_NOTIFICATION from %s\n", p->systemAddress.ToString(true));;
			break;


		case ID_NEW_INCOMING_CONNECTION:
			// Somebody connected.  We have their IP now
			printf("ID_NEW_INCOMING_CONNECTION from %s with GUID %s\n", p->systemAddress.ToString(true), p->guid.ToString());
			clientID = p->systemAddress; // Record the player ID of the client

			printf("Remote internal IDs:\n");
			for (int index = 0; index < MAXIMUM_NUMBER_OF_INTERNAL_IDS; index++)
			{
				RakNet::SystemAddress internalId = server->GetInternalID(p->systemAddress, index);
				if (internalId != RakNet::UNASSIGNED_SYSTEM_ADDRESS)
				{
					printf("%i. %s\n", index + 1, internalId.ToString(true));
				}
			}

			server->Send(msg, strlen(msg) + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

			break;

		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
			break;

		case ID_CONNECTED_PING:
		case ID_UNCONNECTED_PING:
			printf("Ping from %s\n", p->systemAddress.ToString(true));
			break;

		case ID_CONNECTION_LOST:
			// Couldn't deliver a reliable packet - i.e. the other system was abnormally
			// terminated
			HandleDisconnect(p->guid);
			printf("ID_CONNECTION_LOST from %s\n", p->systemAddress.ToString(true));;
			break;

		default:
			// The server knows the static data of all clients, so we can prefix the message
			// With the name data
			OPlog("MESSAGE FROM CLIENT");

			HandleMessage(p->data, p->length, p->guid);

			break;
		}

	}
}

unsigned char GetPacketIdentifier(RakNet::Packet *p)
{
	if (p == 0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)p->data[0];
}

GameServer SERVER;