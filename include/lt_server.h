// ----------------------------------------------------------------------- //
//
// MODULE  : lt_server.h
//
// PURPOSE : The server side of the engine interface: ILTServer (engine
//           services), IServerShell (our callbacks), BaseClass and the
//           class registration the engine reads from object.lto.
//           Written for AVP2StubBins.
//
//           ILTServer has the same shape as ILTClient (see lt_client.h):
//           a vtable, 28 bytes of engine data, then 137 plain function
//           pointers.  We call none of its virtuals.
//
// ----------------------------------------------------------------------- //

#ifndef LT_SERVER_H
#define LT_SERVER_H

#include <stdlib.h>	// malloc / free for the class list
#include "lt_types.h"

class BaseClass;
typedef BaseClass* LPBASECLASS;


// ----------------------------------------------------------------------- //
// ILTServer: engine services for the server shell and objects.
// ----------------------------------------------------------------------- //

class ILTServer
{
public:
	virtual void _unused_slot_0() = 0;	// Only here to give the class its vtable pointer.

	void*       m_EngineData[7];                                                 // 0x04

	// Function pointers; the number is the index from 0x20.
	void*       _unused_fn_0[4];
	HCLASS      (*GetClass)(const char *pName);                                  // 4
	void*       _unused_fn_5[3];
	LPBASECLASS (*CreateObject)(HCLASS hClass, ObjectCreateStruct *pStruct);     // 8
	void*       _unused_fn_9[43];
	void        (*SetClientUserData)(HCLIENT hClient, void *pData);              // 52
	void*       _unused_fn_53[84];
};

static_assert(offsetof(ILTServer, GetClass)          == 0x30, "ILTServer::GetClass");
static_assert(offsetof(ILTServer, CreateObject)      == 0x40, "ILTServer::CreateObject");
static_assert(offsetof(ILTServer, SetClientUserData) == 0xF0, "ILTServer::SetClientUserData");
static_assert(sizeof(ILTServer)                      == 580,  "ILTServer size");

extern ILTServer *g_pLTServer;


// ----------------------------------------------------------------------- //
// IServerShell: the engine calls these on our server shell object.
// As with IClientShell, every signature must match exactly.
// ----------------------------------------------------------------------- //

class IServerShell
{
public:
	virtual             ~IServerShell() {}                                                                       // 0
	virtual LTRESULT    ServerAppMessageFn(char *pMsg, int nLen) { return LT_OK; }                               // 1
	virtual void        OnAddClient(HCLIENT hClient) {}                                                          // 2
	virtual void        OnRemoveClient(HCLIENT hClient) {}                                                       // 3
	virtual void        VerifyClient(HCLIENT hClient, void *pClientData, uint32 &nVerifyCode) {}                 // 4
	virtual LPBASECLASS OnClientEnterWorld(HCLIENT hClient, void *pClientData, uint32 clientDataLen) = 0;        // 5
	virtual void        OnClientExitWorld(HCLIENT hClient) {}                                                    // 6
	virtual void        PreStartWorld(LTBOOL bSwitchingWorlds) {}                                                // 7
	virtual void        PostStartWorld() {}                                                                      // 8
	virtual void*       GetAuthContext() { return LTNULL; }                                                      // 9
	virtual void        OnPeerToPeerAuthPacket(HCLIENT hSender, HMESSAGEREAD hMessage) {}                        // 10
	virtual void        OnMessage(HCLIENT hSender, uint8 messageID, HMESSAGEREAD hMessage) {}                    // 11
	virtual void        OnObjectMessage(LPBASECLASS pSender, uint32 messageID, HMESSAGEREAD hMessage) {}         // 12
	virtual void        OnCommandOn(HCLIENT hClient, int command) {}                                             // 13
	virtual void        OnCommandOff(HCLIENT hClient, int command) {}                                            // 14
	virtual void        Update(float timeElapsed) {}                                                             // 15
	virtual void        OnPlaybackFinish() {}                                                                    // 16
	virtual void        CacheFiles() {}                                                                          // 17
	virtual void        SRand(unsigned int uiRand) {}                                                            // 18
	virtual LTRESULT    FileLoadNotify(const char *pFilename, LTRESULT status) { return LT_OK; }                 // 19
	virtual LTRESULT    ProcessPacket(char *pData, uint32 dataLen, uint8 senderAddr[4], uint16 senderPort) { return LT_OK; } // 20
};


// ----------------------------------------------------------------------- //
// object.lto server shell exports.  Put SETUP_SERVERSHELL() in one .cpp
// file and define
//   IServerShell* CreateServerShell(ILTServer *pServerLT);
//   void DeleteServerShell(IServerShell *pShell);
// The engine requires server shell version 2.
// ----------------------------------------------------------------------- //

typedef IServerShell* (*CreateServerShellFn)(ILTServer *pServerLT);
typedef void          (*DeleteServerShellFn)(IServerShell *pShell);

IServerShell* CreateServerShell(ILTServer *pServerLT);
void          DeleteServerShell(IServerShell *pShell);

#define SETUP_SERVERSHELL() \
	void *g_hLTDLLInstance = LTNULL; \
	extern "C" __declspec(dllexport) void __cdecl GetServerShellFunctions(CreateServerShellFn *pCreate, DeleteServerShellFn *pDelete) \
	{ \
		*pCreate = CreateServerShell; \
		*pDelete = DeleteServerShell; \
	} \
	extern "C" __declspec(dllexport) int __cdecl GetServerShellVersion() { return 2; } \
	extern "C" __declspec(dllexport) void __cdecl SetInstanceHandle(void *hInstance) { g_hLTDLLInstance = hInstance; }


// ----------------------------------------------------------------------- //
// BaseClass: the root of every server object.  The engine constructs
// objects itself (through ClassDef below), sets m_hObject, and calls the
// two message functions through the vtable.
// ----------------------------------------------------------------------- //

#define MID_PRECREATE 0	// EngineMessageFn: pData is the ObjectCreateStruct.

class BaseClass
{
public:
	BaseClass(uint8 nType = OT_NORMAL)
		: m_pFirstAggregate(LTNULL), m_hObject(LTNULL), m_nType(nType), m_bTrueBaseClass(LTTRUE)
	{
	}

	virtual ~BaseClass() {}

	virtual uint32 EngineMessageFn(uint32 messageID, void *pData, float fData)
	{
		// Objects pick their type in the constructor; tell the engine.
		if (messageID == MID_PRECREATE && pData)
		{
			ObjectCreateStruct *pStruct = (ObjectCreateStruct*)pData;
			if (pStruct->m_ObjectType == OT_NORMAL)
				pStruct->m_ObjectType = m_nType;
		}
		return 1;
	}

	virtual uint32 ObjectMessageFn(HOBJECT hSender, uint32 messageID, HMESSAGEREAD hRead)
	{
		return 1;
	}

	void*   m_pFirstAggregate;	// Not used by the stubs, but the engine expects it here.
	HOBJECT m_hObject;
	uint8   m_nType;
	LTBOOL  m_bTrueBaseClass;
};

static_assert(offsetof(BaseClass, m_pFirstAggregate) == 4,  "BaseClass::m_pFirstAggregate");
static_assert(offsetof(BaseClass, m_hObject)         == 8,  "BaseClass::m_hObject");
static_assert(offsetof(BaseClass, m_nType)           == 12, "BaseClass::m_nType");
static_assert(sizeof(BaseClass)                      == 20, "BaseClass size");


// ----------------------------------------------------------------------- //
// Class registration.  object.lto hands the engine an array of ClassDef
// pointers from ObjectDLLSetup().  LT_CLASS() describes one class;
// LT_DEFINE_CLASSES() (once per DLL) provides ObjectDLLSetup itself.
// ----------------------------------------------------------------------- //

// An editable property of a class.  The stubs don't have any, but a
// ClassDef must point at a valid (empty) list.
struct PropDef
{
	char*         m_PropName;
	short         m_PropType;
	char*         m_PropHelp;
	LTVector      m_DefaultValueVector;
	float         m_DefaultValueFloat;
	char*         m_DefaultValueString;
	unsigned long m_PropFlags;
	void*         m_pDEditInternal;
	void*         m_pInternal;
};

struct ClassDef
{
	const char*  m_ClassName;
	ClassDef*    m_ParentClass;
	uint32       m_ClassFlags;
	void         (*m_ConstructFn)(void *pObject);	// Placement-constructs the object.
	void         (*m_DestructFn)(void *pObject);
	void*        m_PluginFn;	// DEdit only.
	short        m_nProps;
	PropDef*     m_Props;
	long         m_ClassObjectSize;
	void*        m_pInternal[2];	// Owned by the engine.
};

static_assert(sizeof(PropDef)                     == 44, "PropDef size");
static_assert(sizeof(ClassDef)                    == 44, "ClassDef size");
static_assert(offsetof(ClassDef, m_nProps)        == 24, "ClassDef::m_nProps");
static_assert(offsetof(ClassDef, m_ClassObjectSize) == 32, "ClassDef::m_ClassObjectSize");

// Every LT_CLASS adds itself to this list while the DLL loads.
struct LTClassLink
{
	LTClassLink(ClassDef *pClass) : m_pClass(pClass), m_pNext(s_pHead) { s_pHead = this; }

	ClassDef*    m_pClass;
	LTClassLink* m_pNext;

	static LTClassLink* s_pHead;
};

// LT_CLASS(Name, Parent): registers class Name (derived from Parent).
// The parent's class must also be registered with LT_CLASS or LT_ROOT_CLASS.
#define LT_CLASS_IMPL(name, parentDef, flags) \
	static void name##_Construct(void *p) { ::new(p) name; } \
	static void name##_Destruct(void *p)  { ((name*)p)->~name(); } \
	static PropDef name##_NoProps[1] = {}; \
	ClassDef name##_ClassDef = { #name, parentDef, flags, name##_Construct, name##_Destruct, \
	                             LTNULL, 0, name##_NoProps, sizeof(name), { LTNULL, LTNULL } }; \
	static LTClassLink name##_ClassLink(&name##_ClassDef);

#define LT_CLASS(name, parent) \
	extern ClassDef parent##_ClassDef; \
	LT_CLASS_IMPL(name, &parent##_ClassDef, 0)

#define LT_ROOT_CLASS(name) \
	LT_CLASS_IMPL(name, LTNULL, 0)

// Provides g_pLTServer and the ObjectDLLSetup export (object DLL version 1).
#define LT_DEFINE_CLASSES() \
	ILTServer *g_pLTServer = LTNULL; \
	LTClassLink* LTClassLink::s_pHead = LTNULL; \
	static struct LTClassList \
	{ \
		ClassDef** m_pList; \
		LTClassList() : m_pList(LTNULL) {} \
		~LTClassList() { free(m_pList); } \
	} s_ClassList; \
	extern "C" __declspec(dllexport) ClassDef** __cdecl ObjectDLLSetup(int *pnDefs, ILTServer *pServer, int *pVersion) \
	{ \
		*pVersion = 1; \
		g_pLTServer = pServer; \
		int nDefs = 0; \
		for (LTClassLink *pLink = LTClassLink::s_pHead; pLink; pLink = pLink->m_pNext) \
			nDefs++; \
		free(s_ClassList.m_pList); \
		s_ClassList.m_pList = (ClassDef**)malloc(sizeof(ClassDef*) * (nDefs ? nDefs : 1)); \
		int i = 0; \
		for (LTClassLink *pLink = LTClassLink::s_pHead; pLink; pLink = pLink->m_pNext) \
			s_ClassList.m_pList[i++] = pLink->m_pClass; \
		*pnDefs = nDefs; \
		return s_ClassList.m_pList; \
	}

#endif // LT_SERVER_H
