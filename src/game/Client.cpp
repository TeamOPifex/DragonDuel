#include "Client.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"

unsigned char GetPacketIdentifierClient(RakNet::Packet *p);
i8 PACKETDATA[4096];

i8 GameClient::Start(ui16 port, ui16 serverPort, const i8* serverIP) {
	MessageHandler = NULL;
	client = RakNet::RakPeerInterface::GetInstance();
	Connected = 0;
	
	RakNet::SocketDescriptor socketDescriptor(port, 0);
	socketDescriptor.socketFamily = AF_INET;
	client->Startup(8, &socketDescriptor, 1);
	client->SetOccasionalPing(true);

	ui8 attempt = 1;
	RakNet::ConnectionAttemptResult car = client->Connect(serverIP, serverPort, "Rumpelstiltskin", (int)strlen("Rumpelstiltskin"));
	if (car == RakNet::INVALID_PARAMETER) {
		client->Shutdown(0);
		return 0;
	}

	RakAssert(car == RakNet::CONNECTION_ATTEMPT_STARTED);

	OPlogInfo("Client Started");

	OPlogInfo("My IP addresses:");
	unsigned int i;
	for (i = 0; i < client->GetNumberOfAddresses(); i++)
	{
		OPlogInfo("%i. %s", i + 1, client->GetLocalIP(i));
	}

	OPlogInfo("My GUID is %s", client->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());

	return 1;
}

void GameClient::Send(GameMessage message, ui8* data, ui32 length) {
	PACKETDATA[0] = message;
	OPmemcpy(&PACKETDATA[1], data, length);
	client->Send(PACKETDATA, length + sizeof(i8), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void GameClient::Send(GameMessage message) {
	PACKETDATA[0] = message;
	client->Send(PACKETDATA, sizeof(i8), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void GameClient::HandleMessage(ui8* data, ui32 length) {
	if (data == NULL) {
		return;
	}

	if (MessageHandler) {
		MessageHandler((GameMessage)data[0], &data[1], length - 1);
	}
}

void GameClient::Update(OPtimer* timer) {

	for (cp = client->Receive(); cp; client->DeallocatePacket(cp), cp = client->Receive())
	{
		// We got a packet, get the identifier with our handy function
		clientPacketIdentifier = GetPacketIdentifierClient(cp);

		// Check if this is a network message packet
		switch (clientPacketIdentifier)
		{
		case ID_DISCONNECTION_NOTIFICATION:
			// Connection lost normally
			printf("ID_DISCONNECTION_NOTIFICATION\n");
			break;
		case ID_ALREADY_CONNECTED:
			// Connection lost normally
			printf("ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", cp->guid);
			break;
		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n");
			break;
		case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_CONNECTION_LOST\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
			break;
		case ID_CONNECTION_BANNED: // Banned from this server
			printf("We are banned from this server.\n");
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			Connected = -1;
			printf("Connection attempt failed\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			// Sorry, the server is full.  I don't do anything here but
			// A real app should tell the user
			printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
			break;

		case ID_INVALID_PASSWORD:
			printf("ID_INVALID_PASSWORD\n");
			break;

		case ID_CONNECTION_LOST:
			// Couldn't deliver a reliable packet - i.e. the other system was abnormally
			// terminated
			printf("ID_CONNECTION_LOST\n");
			break;

		case ID_CONNECTION_REQUEST_ACCEPTED:
			// This tells the client they have connected
			printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", cp->systemAddress.ToString(true), cp->guid.ToString());
			printf("My external address is %s\n", client->GetExternalID(cp->systemAddress).ToString(true));
			Connected = 1;
			break;
		case ID_CONNECTED_PING:
		case ID_UNCONNECTED_PING:
			printf("Ping from %s\n", cp->systemAddress.ToString(true));
			break;
		default:
			// It's a client, so just show the message
			OPlog("MESSAGE FROM SERVER");

			HandleMessage(cp->data, cp->length);
			break;
		}
	}
}

unsigned char GetPacketIdentifierClient(RakNet::Packet *p)
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

GameClient CLIENT;