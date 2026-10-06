// ----------------------------------------------------------------------- //
//
// MODULE  : lt_types.h
//
// PURPOSE : Basic types, constants and structs shared with the LithTech
//           Talon engine (lithtech.exe from AvP2 1.0.9.6).  Written for
//           AVP2StubBins; only what the stub binaries use is declared.
//
//           The engine and these DLLs exchange these structs by pointer,
//           so every size and field offset is fixed by the engine.  The
//           static_asserts at the bottom catch any accidental change.
//
// ----------------------------------------------------------------------- //

#ifndef LT_TYPES_H
#define LT_TYPES_H

#include <stddef.h>	// offsetof

// Plain integer types, all as 32-bit MSVC sees them.
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;
typedef unsigned int   LTBOOL;
typedef uint32         LTRESULT;

#define LTNULL  0
#define LTFALSE 0
#define LTTRUE  1

// Results.
#define LT_OK    0
#define LT_ERROR 1

// Opaque engine handles.  We only ever pass these back to the engine.
struct LTObject;
struct LTSurface;
struct LTFont;
struct LTString;
struct LTClassHandle;
struct LTClientHandle;
class  ILTMessage;

typedef LTObject*       HOBJECT;
typedef LTObject*       HLOCALOBJ;
typedef LTSurface*      HSURFACE;
typedef LTFont*         HLTFONT;
typedef LTString*       HSTRING;
typedef LTClassHandle*  HCLASS;
typedef LTClientHandle* HCLIENT;
typedef ILTMessage*     HMESSAGEREAD;

// Colours are 0x00RRGGBB.  The top bit marks the colour as the transparent one.
typedef uint32 HLTCOLOR;
#define SETRGB(r, g, b)   ((HLTCOLOR)(((uint32)(r) << 16) | ((uint32)(g) << 8) | (uint32)(b)))
#define SETRGB_T(r, g, b) (SETRGB(r, g, b) | 0x80000000UL)

// ClearScreen / FlipScreen flags.
#define CLEARSCREEN_SCREEN        1
#define CLEARSCREEN_RENDER        2
#define FLIPSCREEN_CANDRAWCONSOLE 1

// Object types and flags.
#define OT_NORMAL        0
#define FLAG_GOTHRUWORLD (1 << 21)


struct LTGUID
{
	uint32 a;
	uint16 b;
	uint16 c;
	uint8  d[8];
};

struct LTVector
{
	float x, y, z;

	void Init(
		float fx = 0.0f, 
		float fy = 0.0f, 
		float fz = 0.0f) 
	{ 
		x = fx; 
		y = fy; 
		z = fz;
	}
};

// A quaternion, stored x, y, z, w.
struct LTRotation
{
	float m_Quat[4];

	void Init() 
	{ 
		m_Quat[0] = m_Quat[1] = m_Quat[2] = 0.0f; 
		m_Quat[3] = 1.0f; 
	}
};

struct LTRect
{
	int left, top, right, bottom;

	LTRect() : left(0), top(0), right(0), bottom(0) {}
	LTRect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
	void Init(int l, int t, int r, int b) { left = l; top = t; right = r; bottom = b; }
};

struct LTIntPt
{
	int x, y;
};

// One display mode. The engine passes a list of these to OnEngineInitialized.
struct RMode
{
	LTBOOL m_bHardware;
	char   m_RenderDLL[256];
	char   m_InternalName[128];
	char   m_Description[128];
	uint32 m_Width;
	uint32 m_Height;
	uint32 m_BitDepth;
	RMode* m_pNext;
};

// Filled in by the server shell to create an object.
struct ObjectCreateStruct
{
	ObjectCreateStruct() { Clear(); }

	void Clear()
	{
		m_ObjectType    = OT_NORMAL;
		m_ContainerCode = 0;
		m_CreateFlags   = 1;	// Load the object's files automatically.
		m_Flags         = 0;
		m_Flags2        = 0;
		m_UserFlags     = 0;
		m_Pos.Init();
		m_Scale.Init(1.0f, 1.0f, 1.0f);
		m_Rotation.Init();
		m_UserData      = 0;
		m_ClassName[0]  = 0;
		m_Filename[0]   = 0;
		for (int i = 0; i < 4; i++)
			m_SkinNames[i][0] = 0;
		m_Name[0]            = 0;
		m_NextUpdate         = 0.0f;
		m_fDeactivationTime  = 0.0f;
		m_bResetAnimations   = LTTRUE;
	}

	uint16     m_ObjectType;
	uint16     m_ContainerCode;
	uint32     m_CreateFlags;
	uint32     m_Flags;
	uint32     m_Flags2;
	uint32     m_UserFlags;
	LTVector   m_Pos;
	LTVector   m_Scale;
	LTRotation m_Rotation;
	uint32     m_UserData;
	char       m_ClassName[64];
	char       m_Filename[128];
	char       m_SkinNames[4][128];
	char       m_Name[128];
	float      m_NextUpdate;
	float      m_fDeactivationTime;
	LTBOOL     m_bResetAnimations;
};


// Layout checks (32-bit only; CMakeLists.txt refuses 64-bit builds).
static_assert(sizeof(LTGUID)     == 16, "LTGUID size");
static_assert(sizeof(LTVector)   == 12, "LTVector size");
static_assert(sizeof(LTRotation) == 16, "LTRotation size");
static_assert(sizeof(LTRect)     == 16, "LTRect size");
static_assert(sizeof(LTIntPt)    == 8,  "LTIntPt size");

static_assert(sizeof(RMode)                   == 0x214, "RMode size");
static_assert(offsetof(RMode, m_Width)        == 516,   "RMode::m_Width");
static_assert(offsetof(RMode, m_pNext)        == 528,   "RMode::m_pNext");

static_assert(sizeof(ObjectCreateStruct)                       == 0x38C, "ObjectCreateStruct size");
static_assert(offsetof(ObjectCreateStruct, m_Flags)            == 8,     "ObjectCreateStruct::m_Flags");
static_assert(offsetof(ObjectCreateStruct, m_Pos)              == 20,    "ObjectCreateStruct::m_Pos");
static_assert(offsetof(ObjectCreateStruct, m_Rotation)         == 44,    "ObjectCreateStruct::m_Rotation");
static_assert(offsetof(ObjectCreateStruct, m_ClassName)        == 64,    "ObjectCreateStruct::m_ClassName");
static_assert(offsetof(ObjectCreateStruct, m_Name)             == 768,   "ObjectCreateStruct::m_Name");
static_assert(offsetof(ObjectCreateStruct, m_bResetAnimations) == 904,   "ObjectCreateStruct::m_bResetAnimations");

#endif // LT_TYPES_H
