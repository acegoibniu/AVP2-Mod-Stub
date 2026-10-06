// ----------------------------------------------------------------------- //
//
// MODULE  : lt_client.h
//
// PURPOSE : The client side of the engine interface: ILTClient (engine
//           services), ILTCursor, IClientShell (our callbacks) and the
//           exports cshell.dll must provide.  Written for AVP2StubBins.
//
//           ILTClient is built by the engine and handed to us, so its
//           layout is fixed by lithtech.exe:
//             - a vtable; we only call a few of its slots (the rest are
//               placeholders that keep the slot numbers right),
//             - 28 bytes of engine data,
//             - 212 plain function pointers (no 'this'), of which we name
//               the ones we call and pad the rest,
//             - four interface pointers, one of them the cursor.
//           The static_asserts at the bottom pin every offset we use.
//
// ----------------------------------------------------------------------- //

#ifndef LT_CLIENT_H
#define LT_CLIENT_H

#include "lt_types.h"

// Defines a run of unused virtual slots.  Each slot needs its own name.
#define LT_VSLOT(n) virtual void _unused_slot_##n() = 0;


// ----------------------------------------------------------------------- //
// ILTCursor: the engine's mouse cursor service.
// ----------------------------------------------------------------------- //

enum CursorMode
{
	CM_None = 0,
	CM_Hardware
};

class ILTCursorInst;
typedef ILTCursorInst* HLTCURSOR;

class ILTCursor
{
public:
	virtual LTRESULT SetCursorMode(CursorMode cMode) = 0;                                  // 0
	LT_VSLOT(1) LT_VSLOT(2)
	virtual LTRESULT LoadCursorBitmapResource(const char *pName, HLTCURSOR &hCursor) = 0;  // 3
	virtual LTRESULT FreeCursor(const HLTCURSOR hCursor) = 0;                              // 4
	virtual LTRESULT SetCursor(HLTCURSOR hCursor) = 0;                                     // 5
	LT_VSLOT(6) LT_VSLOT(7)
};


// ----------------------------------------------------------------------- //
// ILTClient: engine services for the client shell.
// ----------------------------------------------------------------------- //

class ILTClient
{
public:

	// --- Virtual slots 0-50 (the vtable continues; we don't use the rest). ---

	LT_VSLOT(0)  LT_VSLOT(1)  LT_VSLOT(2)  LT_VSLOT(3)  LT_VSLOT(4)
	LT_VSLOT(5)  LT_VSLOT(6)  LT_VSLOT(7)  LT_VSLOT(8)  LT_VSLOT(9)
	LT_VSLOT(10) LT_VSLOT(11) LT_VSLOT(12) LT_VSLOT(13) LT_VSLOT(14)
	LT_VSLOT(15) LT_VSLOT(16) LT_VSLOT(17) LT_VSLOT(18) LT_VSLOT(19)
	LT_VSLOT(20) LT_VSLOT(21) LT_VSLOT(22) LT_VSLOT(23) LT_VSLOT(24)
	LT_VSLOT(25) LT_VSLOT(26) LT_VSLOT(27) LT_VSLOT(28) LT_VSLOT(29)
	LT_VSLOT(30) LT_VSLOT(31) LT_VSLOT(32) LT_VSLOT(33) LT_VSLOT(34)
	LT_VSLOT(35) LT_VSLOT(36) LT_VSLOT(37) LT_VSLOT(38)
	virtual void     CPrint(const char *pMsg, ...) = 0;              // 39
	LT_VSLOT(40) LT_VSLOT(41) LT_VSLOT(42) LT_VSLOT(43)
	virtual HSTRING  FormatString(int nStringId, ...) = 0;           // 44: string from cres.dll
	LT_VSLOT(45)
	virtual HSTRING  CreateString(const char *pString) = 0;          // 46
	virtual void     FreeString(HSTRING hString) = 0;                // 47
	LT_VSLOT(48) LT_VSLOT(49)
	virtual char*    GetStringData(HSTRING hString) = 0;             // 50

	// --- Data. ---

	void*    m_EngineData[7];                                        // 0x04

	// --- Function pointers; the number is the index from 0x20. ---

	void*    _unused_fn_0[6];
	void     (*Shutdown)();                                          // 6
	void     (*ShutdownWithMessage)(const char *pMsg, ...);          // 7
	LTRESULT (*FlipScreen)(uint32 flags);                            // 8
	LTRESULT (*ClearScreen)(LTRect *pClearRect, uint32 flags, LTVector *pColor); // 9
	LTRESULT (*Start3D)();                                           // 10
	void*    _unused_fn_11[2];
	LTRESULT (*StartOptimized2D)();                                  // 13
	LTRESULT (*EndOptimized2D)();                                    // 14
	void*    _unused_fn_15[4];
	LTRESULT (*End3D)();                                             // 19
	void*    _unused_fn_20[3];
	LTRESULT (*SetRenderMode)(RMode *pMode);                         // 23
	void*    _unused_fn_24[48];
	HLTFONT  (*CreateFont)(const char *pFontName, int width, int height,
	                       LTBOOL bItalic, LTBOOL bUnderline, LTBOOL bBold); // 72
	void     (*DeleteFont)(HLTFONT hFont);                           // 73
	void*    _unused_fn_74[7];
	LTRESULT (*OptimizeSurface)(HSURFACE hSurface, HLTCOLOR hTransparentColor); // 81
	void*    _unused_fn_82[1];
	HSURFACE (*GetScreenSurface)();                                  // 83
	void*    _unused_fn_84[1];
	HSURFACE (*CreateSurfaceFromString)(HLTFONT hFont, HSTRING hString,
	                                    HLTCOLOR hForeColor, HLTCOLOR hBackColor,
	                                    int extraPixelsX, int extraPixelsY); // 85
	HSURFACE (*CreateSurface)(uint32 width, uint32 height);          // 86
	LTRESULT (*DeleteSurface)(HSURFACE hSurface);                    // 87
	void*    _unused_fn_88[6];
	void     (*GetSurfaceDims)(HSURFACE hSurf, uint32 *pWidth, uint32 *pHeight); // 94
	void*    _unused_fn_95[4];
	LTRESULT (*DrawSurfaceToSurfaceTransparent)(HSURFACE hDest, HSURFACE hSrc,
	                                            LTRect *pSrcRect, int destX, int destY,
	                                            HLTCOLOR hTransparentColor); // 99
	void*    _unused_fn_100[8];
	LTRESULT (*FillRect)(HSURFACE hDest, LTRect *pRect, HLTCOLOR hColor); // 108
	void*    _unused_fn_109[24];
	void     (*DebugOut)(const char *pMsg, ...);                     // 133
	void*    _unused_fn_134[2];
	void     (*RunConsoleString)(const char *pString);               // 136
	void*    _unused_fn_137[63];
	LTRESULT (*GetEngineHook)(const char *pName, void **ppData);     // 200
	void*    _unused_fn_201[11];

	// --- Sub-interfaces. ---

	void*      m_pVideoMgr;                                          // 0x370
	void*      m_pTexMod;                                            // 0x374
	ILTCursor* m_pCursorLT;                                          // 0x378
	void*      m_pDirectMusicMgr;                                    // 0x37C

	ILTCursor* Cursor() { return m_pCursorLT; }
};

// windows.h renames CreateFont to CreateFontA; that doesn't move the member.
static_assert(offsetof(ILTClient, m_EngineData)          == 0x04,  "ILTClient data");
static_assert(offsetof(ILTClient, Shutdown)              == 0x38,  "ILTClient::Shutdown");
static_assert(offsetof(ILTClient, ShutdownWithMessage)   == 0x3C,  "ILTClient::ShutdownWithMessage");
static_assert(offsetof(ILTClient, FlipScreen)            == 0x40,  "ILTClient::FlipScreen");
static_assert(offsetof(ILTClient, ClearScreen)           == 0x44,  "ILTClient::ClearScreen");
static_assert(offsetof(ILTClient, Start3D)               == 0x48,  "ILTClient::Start3D");
static_assert(offsetof(ILTClient, StartOptimized2D)      == 0x54,  "ILTClient::StartOptimized2D");
static_assert(offsetof(ILTClient, EndOptimized2D)        == 0x58,  "ILTClient::EndOptimized2D");
static_assert(offsetof(ILTClient, End3D)                 == 0x6C,  "ILTClient::End3D");
static_assert(offsetof(ILTClient, SetRenderMode)         == 0x7C,  "ILTClient::SetRenderMode");
static_assert(offsetof(ILTClient, DeleteFont)            == 0x144, "ILTClient::DeleteFont");
static_assert(offsetof(ILTClient, OptimizeSurface)       == 0x164, "ILTClient::OptimizeSurface");
static_assert(offsetof(ILTClient, GetScreenSurface)      == 0x16C, "ILTClient::GetScreenSurface");
static_assert(offsetof(ILTClient, CreateSurfaceFromString) == 0x174, "ILTClient::CreateSurfaceFromString");
static_assert(offsetof(ILTClient, CreateSurface)         == 0x178, "ILTClient::CreateSurface");
static_assert(offsetof(ILTClient, DeleteSurface)         == 0x17C, "ILTClient::DeleteSurface");
static_assert(offsetof(ILTClient, GetSurfaceDims)        == 0x198, "ILTClient::GetSurfaceDims");
static_assert(offsetof(ILTClient, DrawSurfaceToSurfaceTransparent) == 0x1AC, "ILTClient::DrawSurfaceToSurfaceTransparent");
static_assert(offsetof(ILTClient, FillRect)              == 0x1D0, "ILTClient::FillRect");
static_assert(offsetof(ILTClient, DebugOut)              == 0x234, "ILTClient::DebugOut");
static_assert(offsetof(ILTClient, RunConsoleString)      == 0x240, "ILTClient::RunConsoleString");
static_assert(offsetof(ILTClient, GetEngineHook)         == 0x340, "ILTClient::GetEngineHook");
static_assert(offsetof(ILTClient, m_pCursorLT)           == 0x378, "ILTClient::m_pCursorLT");
static_assert(sizeof(ILTClient)                          == 0x380, "ILTClient size");


// ----------------------------------------------------------------------- //
// IClientShell: the engine calls these on our client shell object.
// The engine calls every slot, so each signature must match exactly
// (arguments are popped by the callee). Override what you need.
// ----------------------------------------------------------------------- //

struct ArgList;
struct CollisionInfo;
struct PlaySoundInfo;
struct StateChange;
class  ILTStream;

class IClientShell
{
public:
	virtual          ~IClientShell() {}                                                          // 0
	virtual void*    GetAuthContext() { return LTNULL; }                                         // 1
	virtual void     OnCommandOn(int command) {}                                                 // 2
	virtual void     OnCommandOff(int command) {}                                                // 3
	virtual void     OnKeyDown(int key, int rep) {}                                              // 4
	virtual void     OnKeyUp(int key) {}                                                         // 5
	virtual void     OnPeerToPeerAuthPacket(HMESSAGEREAD hMessage) {}                            // 6
	virtual void     OnMessage(uint8 messageID, HMESSAGEREAD hMessage) {}                        // 7
	virtual void     OnModelKey(HLOCALOBJ hObj, ArgList *pArgs, int nTracker) {}                 // 8
	virtual void     PreLoadWorld(char *pWorldName) {}                                           // 9
	virtual void     OnEnterWorld() {}                                                           // 10
	virtual void     OnExitWorld() {}                                                            // 11
	virtual void     SpecialEffectNotify(HLOCALOBJ hObj, HMESSAGEREAD hMessage) {}               // 12
	virtual void     OnObjectRemove(HLOCALOBJ hObj) {}                                           // 13
	virtual void     PreUpdate() {}                                                              // 14
	virtual void     Update() {}                                                                 // 15
	virtual void     PostUpdate() {}                                                             // 16
	virtual LTRESULT OnObjectMove(HLOCALOBJ hObj, LTBOOL bTeleport, LTVector *pNewPos) { return LT_OK; }       // 17
	virtual LTRESULT OnObjectRotate(HLOCALOBJ hObj, LTBOOL bTeleport, LTRotation *pNewRot) { return LT_OK; }  // 18
	virtual LTRESULT OnEngineInitialized(RMode *pMode, LTGUID *pAppGuid) { return LT_ERROR; }  // 19: must override
	virtual void     OnEngineTerm() {}                                                           // 20
	virtual void     OnEvent(uint32 dwEventID, uint32 dwParam) {}                                // 21
	virtual LTRESULT OnTouchNotify(HOBJECT hMain, CollisionInfo *pInfo, float forceMag) { return LT_OK; } // 22
	virtual void     SRand() {}                                                                  // 23
	virtual void     DemoSerialize(ILTStream *pStream, LTBOOL bLoad) {}                          // 24
	virtual void     OnPlaySound(PlaySoundInfo *pPlaySoundInfo) {}                               // 25
	virtual void     SetDisconnectCode(uint32 nCode, const char *pMsg) {}                        // 26
	virtual void     OnTextureLoad(StateChange **ppStateChange, const char *pCommandLine) {}     // 27
};


// ----------------------------------------------------------------------- //
// cshell.dll exports.  Put SETUP_CLIENTSHELL() in one .cpp file and define
//   IClientShell* CreateClientShell(ILTClient *pClientLT);
//   void DeleteClientShell(IClientShell *pShell);
// The engine requires client shell version 2.
// ----------------------------------------------------------------------- //

typedef IClientShell* (*CreateClientShellFn)(ILTClient *pClientLT);
typedef void          (*DeleteClientShellFn)(IClientShell *pShell);

IClientShell* CreateClientShell(ILTClient *pClientLT);
void          DeleteClientShell(IClientShell *pShell);

extern ILTClient *g_pLTClient;

#define SETUP_CLIENTSHELL() \
	ILTClient *g_pLTClient = LTNULL; \
	void *g_hLTDLLInstance = LTNULL; \
	extern "C" __declspec(dllexport) void __cdecl GetClientShellFunctions(CreateClientShellFn *pCreate, DeleteClientShellFn *pDelete) \
	{ \
		*pCreate = CreateClientShell; \
		*pDelete = DeleteClientShell; \
	} \
	extern "C" __declspec(dllexport) int __cdecl GetClientShellVersion() { return 2; } \
	extern "C" __declspec(dllexport) void __cdecl SetInstanceHandle(void *hInstance) { g_hLTDLLInstance = hInstance; }

#endif // LT_CLIENT_H
