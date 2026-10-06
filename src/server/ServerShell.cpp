// ----------------------------------------------------------------------- //
//
// MODULE  : ServerShell.cpp
//
// PURPOSE : Bare-minimum AvP2 server shell (object.lto).
//
// ----------------------------------------------------------------------- //

#include <windows.h>
#include <new>	// placement new, used by LT_CLASS

#include "lt_server.h"

// Exports GetServerShellFunctions, GetServerShellVersion and SetInstanceHandle.
SETUP_SERVERSHELL();


// The object each client controls once it enters a world.
class StubPlayer : public BaseClass
{
public:
	StubPlayer() : BaseClass(OT_NORMAL) {}
};

LT_CLASS(StubPlayer, BaseClass)


class CStubServerShell : public IServerShell
{
public:

	// The only method the engine requires: return an object for the client.
	virtual LPBASECLASS OnClientEnterWorld(HCLIENT hClient, void *pClientData, uint32 clientDataLen)
	{
		HCLASS hClass = g_pLTServer->GetClass("StubPlayer");
		if (!hClass)
			return LTNULL;

		ObjectCreateStruct theStruct;	// Origin, no rotation.
		theStruct.m_Flags = FLAG_GOTHRUWORLD;

		BaseClass *pPlayer = g_pLTServer->CreateObject(hClass, &theStruct);
		if (pPlayer)
			g_pLTServer->SetClientUserData(hClient, pPlayer);

		return pPlayer;
	}
};


// ----------------------------------------------------------------------- //
// Engine entry points (declared in lt_server.h).
// ----------------------------------------------------------------------- //

IServerShell* CreateServerShell(ILTServer *pServer)
{
	// g_pLTServer is already set by ObjectDLLSetup, but set it again to be safe.
	g_pLTServer = pServer;
	return new CStubServerShell;
}

void DeleteServerShell(IServerShell *pInputShell)
{
	delete (CStubServerShell*)pInputShell;
}
