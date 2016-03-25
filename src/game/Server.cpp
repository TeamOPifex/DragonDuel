#include "Server.h"

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"

RakNet::RakPeerInterface* server;
RakNet::Packet* p;
RakNet::SystemAddress clientID;
ui8 packetIdentifier;

unsigned char GetPacketIdentifier(RakNet::Packet *p);

i8 ServerStart(ui16 port) {
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

i8 SERVERPACKETDATA[4096];
void ServerHandleMessage(ui8* data, ui32 length) {
	if (data == NULL) {
		return;
	}

	if (data[0] == ID_USER_PACKET_ENUM + 1 && data[1] == 0) {
		SERVERPACKETDATA[0] = ID_USER_PACKET_ENUM + 1;
		OPmemcpy(&SERVERPACKETDATA[1], data, length);
		server->Send(SERVERPACKETDATA, length + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	}
}

void ServerUpdate() {
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
			printf("ID_CONNECTION_LOST from %s\n", p->systemAddress.ToString(true));;
			break;

		default:
			// The server knows the static data of all clients, so we can prefix the message
			// With the name data
			printf("MESSAGE FROM CLIENT :: %s\n", p->data);

			ServerHandleMessage(p->data, p->length);

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