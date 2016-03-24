#include "./OPengine.h"

#include "MessageIdentifiers.h"

enum GameMessage {
	GENERIC = ID_USER_PACKET_ENUM,
	BackgroundColor
};

extern void(*MessageHandler)(GameMessage, ui8*);

void ClientStart(const i8* port, const i8* serverPort, const i8* serverIP);
void ClientSend(GameMessage message, i8* data, ui32 length);
void ClientUpdate();