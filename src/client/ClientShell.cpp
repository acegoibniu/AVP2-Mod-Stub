// ----------------------------------------------------------------------- //
//
// MODULE  : ClientShell.cpp
//
// PURPOSE : Bare-minimum AvP2 client shell (cshell.dll).  Sets the render
//           mode, prints a message to the console, and draws the message.
//
// ----------------------------------------------------------------------- //

#include <windows.h>
#include <stdio.h>

#include "lt_client.h"
#include "cres_ids.h"

// Exports GetClientShellFunctions, GetClientShellVersion and SetInstanceHandle,
// and defines g_pLTClient.
SETUP_CLIENTSHELL();

namespace
{
	// AvP2's application GUID, this is used for LithTech network connections.
	const LTGUID kAppGuid = { 0x1dfb2bc1, 0xeb40, 0x11d2, { 0xb7, 0xd2, 0x00, 0x60, 0x97, 0x17, 0x66, 0xc1 } };

	// Black is the transparent color for all our surfaces, like AvP2's interface.
	const HLTCOLOR kTransparent  = SETRGB_T(0, 0, 0);
	const HLTCOLOR kBackground   = SETRGB(0, 0, 0);
	const HLTCOLOR kHelloColor   = SETRGB(120, 255, 120);
	const HLTCOLOR kLabelColor   = SETRGB(255, 255, 255);
	const HLTCOLOR kButtonColor  = SETRGB(110, 20, 20);
	const HLTCOLOR kButtonHover  = SETRGB(200, 40, 40);
	const HLTCOLOR kNoticeColor  = SETRGB(150, 150, 150);

	const int kButtonPadX = 40;
	const int kButtonPadY = 12;
	const int kButtonGap  = 40;	// Space between the hello text and the button.
	const int kNoticeGap  = 40;	// Space between the button and the notice lines.
	const int kNoticeLineGap = 4;

	// Small print under the button: credit and the Monolith disclaimer.
	struct NoticeLine { int nStringId; const char *pFallback; };
	const NoticeLine kNoticeLines[] =
	{
		{ IDS_CREDIT,      "AVP2 Mod Stub by Ace O'Doom" },
		{ IDS_DISCLAIMER1, "This add-on is not made by or supported by Monolith Productions," },
		{ IDS_DISCLAIMER2, "or any of its affiliates and subsidiaries." },
	};
	const int kNumNoticeLines = sizeof(kNoticeLines) / sizeof(kNoticeLines[0]);

	HWND    g_hMainWnd       = NULL;
	WNDPROC g_pfnMainWndProc = NULL;
}


class CStubClientShell : public IClientShell
{
public:

	CStubClientShell()
		: m_hFont(LTNULL), m_hSmallFont(LTNULL), m_hHelloSurf(LTNULL), m_hButtonSurf(LTNULL), m_hButtonHoverSurf(LTNULL),
		  m_hCursor(LTNULL), m_bHover(false)
	{
		m_ButtonRect.Init(0, 0, 0, 0);
		for (int i = 0; i < kNumNoticeLines; i++)
		{
			m_hNoticeSurf[i] = LTNULL;
			m_NoticePos[i].x = m_NoticePos[i].y = 0;
		}
	}

	virtual LTRESULT OnEngineInitialized(RMode *pMode, LTGUID *pAppGuid);
	virtual void     OnEngineTerm();
	virtual void     Update();
	virtual void     OnKeyDown(int key, int rep);

	void OnMouseMove(int x, int y)   { m_bHover = HitButton(x, y); }
	void OnLButtonDown(int x, int y) { if (HitButton(x, y)) g_pLTClient->Shutdown(); }

private:

	bool     SetRenderMode(RMode *pMode);
	HSTRING  LoadHString(int nStringId, const char *pFallback);
	HSURFACE CreateTextSurface(HLTFONT hFont, int nStringId, const char *pFallback, HLTCOLOR hColor);
	HSURFACE CreateButtonSurface(HSURFACE hLabel, HLTCOLOR hFill);
	void     CreateInterface();
	void     LayoutInterface(uint32 nScreenWidth, uint32 nScreenHeight);
	bool     HitButton(int x, int y);
	void     HookWindow();
	void     UnhookWindow();

	HLTFONT   m_hFont;
	HLTFONT   m_hSmallFont;
	HSURFACE  m_hHelloSurf;
	HSURFACE  m_hButtonSurf;
	HSURFACE  m_hButtonHoverSurf;
	HSURFACE  m_hNoticeSurf[kNumNoticeLines];
	HLTCURSOR m_hCursor;

	LTRect    m_ButtonRect;	// In screen-surface pixels.
	LTIntPt   m_HelloPos;
	LTIntPt   m_NoticePos[kNumNoticeLines];
	bool      m_bHover;
};

static CStubClientShell *g_pStubShell = NULL;


// ----------------------------------------------------------------------- //
// Engine entry points (declared in lt_client.h).
// ----------------------------------------------------------------------- //

IClientShell* CreateClientShell(ILTClient *pClientDE)
{
	g_pLTClient = pClientDE;
	g_pStubShell = new CStubClientShell;
	return g_pStubShell;
}

void DeleteClientShell(IClientShell *pInputShell)
{
	delete (CStubClientShell*)pInputShell;
	g_pStubShell = NULL;
}


// ----------------------------------------------------------------------- //
// Window hook: the engine doesn't pass mouse clicks to the client shell,
// so subclass its window like AvP2's HookWindow() does.
// ----------------------------------------------------------------------- //

static LRESULT CALLBACK HookedWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (g_pStubShell)
	{
		switch (uMsg)
		{
			case WM_MOUSEMOVE:
				g_pStubShell->OnMouseMove((short)LOWORD(lParam), (short)HIWORD(lParam));
				break;

			case WM_LBUTTONDOWN:
				g_pStubShell->OnLButtonDown((short)LOWORD(lParam), (short)HIWORD(lParam));
				break;
		}
	}

	return CallWindowProc(g_pfnMainWndProc, hWnd, uMsg, wParam, lParam);
}

void CStubClientShell::HookWindow()
{
	if (g_pLTClient->GetEngineHook("HWND", (void**)&g_hMainWnd) != LT_OK || !g_hMainWnd)
	{
		g_pLTClient->CPrint("AVP2 Mod Stub: couldn't get the engine window; use Esc to exit.");
		g_hMainWnd = NULL;
		return;
	}

	g_pfnMainWndProc = (WNDPROC)GetWindowLongPtr(g_hMainWnd, GWLP_WNDPROC);
	if (!g_pfnMainWndProc || !SetWindowLongPtr(g_hMainWnd, GWLP_WNDPROC, (LONG_PTR)HookedWindowProc))
	{
		g_pLTClient->CPrint("AVP2 Mod Stub: couldn't hook the engine window; use Esc to exit.");
		g_pfnMainWndProc = NULL;
		g_hMainWnd = NULL;
	}
}

void CStubClientShell::UnhookWindow()
{
	if (g_hMainWnd && g_pfnMainWndProc)
	{
		SetWindowLongPtr(g_hMainWnd, GWLP_WNDPROC, (LONG_PTR)g_pfnMainWndProc);
	}

	g_hMainWnd = NULL;
	g_pfnMainWndProc = NULL;
}


// ----------------------------------------------------------------------- //
// Startup / shutdown.
// ----------------------------------------------------------------------- //

LTRESULT CStubClientShell::OnEngineInitialized(RMode *pMode, LTGUID *pAppGuid)
{
	*pAppGuid = kAppGuid;

	if (!SetRenderMode(pMode))
	{
		g_pLTClient->ShutdownWithMessage("AVP2 Mod Stub: couldn't set a render mode.");
		return LT_ERROR;
	}

	HSTRING hHello = LoadHString(IDS_HELLO, "AvP2 Mod stub has been loaded!");
	g_pLTClient->CPrint("%s", g_pLTClient->GetStringData(hHello));
	g_pLTClient->CPrint("AVP2 Mod Stub: click Exit or press Esc to quit.");
	g_pLTClient->FreeString(hHello);

	CreateInterface();
	HookWindow();

	return LT_OK;
}

// Same fallback order as AvP2's CGameClientShell::OnEngineInitialized.
bool CStubClientShell::SetRenderMode(RMode *pMode)
{
	if (g_pLTClient->SetRenderMode(pMode) == LT_OK)
		return true;

	g_pLTClient->DebugOut("AVP2 Mod Stub: couldn't set render mode, trying 640x480x16...\n");

	RMode rMode = *pMode;
	rMode.m_Width    = 640;
	rMode.m_Height   = 480;
	rMode.m_BitDepth = 16;
	rMode.m_pNext    = LTNULL;

	return g_pLTClient->SetRenderMode(&rMode) == LT_OK;
}

void CStubClientShell::OnEngineTerm()
{
	UnhookWindow();

	if (m_hHelloSurf)       g_pLTClient->DeleteSurface(m_hHelloSurf);
	if (m_hButtonSurf)      g_pLTClient->DeleteSurface(m_hButtonSurf);
	if (m_hButtonHoverSurf) g_pLTClient->DeleteSurface(m_hButtonHoverSurf);
	m_hHelloSurf = m_hButtonSurf = m_hButtonHoverSurf = LTNULL;

	for (int i = 0; i < kNumNoticeLines; i++)
	{
		if (m_hNoticeSurf[i])
			g_pLTClient->DeleteSurface(m_hNoticeSurf[i]);
		m_hNoticeSurf[i] = LTNULL;
	}

	if (m_hFont)
	{
		g_pLTClient->DeleteFont(m_hFont);
		m_hFont = LTNULL;
	}

	if (m_hSmallFont)
	{
		g_pLTClient->DeleteFont(m_hSmallFont);
		m_hSmallFont = LTNULL;
	}

	if (m_hCursor)
	{
		g_pLTClient->Cursor()->FreeCursor(m_hCursor);
		m_hCursor = LTNULL;
	}
}


// ----------------------------------------------------------------------- //
// Interface creation.
// ----------------------------------------------------------------------- //

// Strings come from cres.dll's string table; the fallback is only used if
// cres.dll is missing or doesn't have the string.
HSTRING CStubClientShell::LoadHString(int nStringId, const char *pFallback)
{
	HSTRING hString = g_pLTClient->FormatString(nStringId);
	const char *pData = hString ? g_pLTClient->GetStringData(hString) : LTNULL;
	if (pData && pData[0])
		return hString;

	if (hString)
		g_pLTClient->FreeString(hString);
	return g_pLTClient->CreateString(pFallback);
}

HSURFACE CStubClientShell::CreateTextSurface(HLTFONT hFont, int nStringId, const char *pFallback, HLTCOLOR hColor)
{
	if (!hFont)
		return LTNULL;

	HSTRING hString = LoadHString(nStringId, pFallback);
	HSURFACE hSurf = g_pLTClient->CreateSurfaceFromString(hFont, hString, hColor, kBackground, 0, 0);
	g_pLTClient->FreeString(hString);
	return hSurf;
}

// A filled rectangle with the label centered on it.
HSURFACE CStubClientShell::CreateButtonSurface(HSURFACE hLabel, HLTCOLOR hFill)
{
	uint32 nLabelWidth = 0, nLabelHeight = 0;
	g_pLTClient->GetSurfaceDims(hLabel, &nLabelWidth, &nLabelHeight);

	uint32 nWidth  = nLabelWidth + kButtonPadX * 2;
	uint32 nHeight = nLabelHeight + kButtonPadY * 2;

	HSURFACE hSurf = g_pLTClient->CreateSurface(nWidth, nHeight);
	if (!hSurf)
		return LTNULL;

	LTRect rcFill(0, 0, nWidth, nHeight);
	g_pLTClient->FillRect(hSurf, &rcFill, hFill);
	g_pLTClient->DrawSurfaceToSurfaceTransparent(hSurf, hLabel, LTNULL, kButtonPadX, kButtonPadY, kTransparent);
	g_pLTClient->OptimizeSurface(hSurf, kTransparent);
	return hSurf;
}

void CStubClientShell::CreateInterface()
{
	m_hFont = g_pLTClient->CreateFont("Arial", 0, 32, LTFALSE, LTFALSE, LTTRUE);
	if (!m_hFont)
		g_pLTClient->CPrint("AVP2 Mod Stub: couldn't create a font; nothing will be drawn. Use Esc to exit.");

	m_hHelloSurf = CreateTextSurface(m_hFont, IDS_HELLO, "AvP2 Mod stub has been loaded!", kHelloColor);
	if (m_hHelloSurf)
		g_pLTClient->OptimizeSurface(m_hHelloSurf, kTransparent);

	HSURFACE hLabel = CreateTextSurface(m_hFont, IDS_EXIT, "Exit", kLabelColor);
	if (hLabel)
	{
		m_hButtonSurf      = CreateButtonSurface(hLabel, kButtonColor);
		m_hButtonHoverSurf = CreateButtonSurface(hLabel, kButtonHover);
		g_pLTClient->DeleteSurface(hLabel);
	}

	m_hSmallFont = g_pLTClient->CreateFont("Arial", 0, 18, LTFALSE, LTFALSE, LTFALSE);
	for (int i = 0; i < kNumNoticeLines; i++)
	{
		m_hNoticeSurf[i] = CreateTextSurface(m_hSmallFont, kNoticeLines[i].nStringId, kNoticeLines[i].pFallback, kNoticeColor);
		if (m_hNoticeSurf[i])
			g_pLTClient->OptimizeSurface(m_hNoticeSurf[i], kTransparent);
	}

	// The engine re-centers the mouse every frame (for mouse-look) unless
	// CursorCenter is 0; AvP2's CInterfaceMgr::UseCursor() does the same.
	g_pLTClient->RunConsoleString("CursorCenter 0");

	// Mouse pointer from cres.dll, the same way AvP2's InterfaceMgr does it.
	ILTCursor *pCursor = g_pLTClient->Cursor();
	if (pCursor && pCursor->LoadCursorBitmapResource(MAKEINTRESOURCE(IDC_POINTER), m_hCursor) == LT_OK)
	{
		pCursor->SetCursor(m_hCursor);
		pCursor->SetCursorMode(CM_Hardware);
	}
	else
	{
		m_hCursor = LTNULL;
		g_pLTClient->CPrint("AVP2 Mod Stub: couldn't load the mouse cursor.");
	}
}

void CStubClientShell::LayoutInterface(uint32 nScreenWidth, uint32 nScreenHeight)
{
	uint32 nHelloWidth = 0, nHelloHeight = 0;
	if (m_hHelloSurf)
		g_pLTClient->GetSurfaceDims(m_hHelloSurf, &nHelloWidth, &nHelloHeight);

	uint32 nButtonWidth = 0, nButtonHeight = 0;
	if (m_hButtonSurf)
		g_pLTClient->GetSurfaceDims(m_hButtonSurf, &nButtonWidth, &nButtonHeight);

	int nTotalHeight = (int)(nHelloHeight + kButtonGap + nButtonHeight);
	int nTop = ((int)nScreenHeight - nTotalHeight) / 2;

	m_HelloPos.x = ((int)nScreenWidth - (int)nHelloWidth) / 2;
	m_HelloPos.y = nTop;

	int nButtonLeft = ((int)nScreenWidth - (int)nButtonWidth) / 2;
	int nButtonTop  = nTop + (int)nHelloHeight + kButtonGap;
	m_ButtonRect.Init(nButtonLeft, nButtonTop, nButtonLeft + (int)nButtonWidth, nButtonTop + (int)nButtonHeight);

	int nNoticeTop = m_ButtonRect.bottom + kNoticeGap;
	for (int i = 0; i < kNumNoticeLines; i++)
	{
		uint32 nWidth = 0, nHeight = 0;
		if (m_hNoticeSurf[i])
			g_pLTClient->GetSurfaceDims(m_hNoticeSurf[i], &nWidth, &nHeight);

		m_NoticePos[i].x = ((int)nScreenWidth - (int)nWidth) / 2;
		m_NoticePos[i].y = nNoticeTop;
		nNoticeTop += (int)nHeight + kNoticeLineGap;
	}
}

// x, y are window client coordinates; the screen surface can be a different
// size (e.g. a stretched borderless window), so scale into surface pixels.
bool CStubClientShell::HitButton(int x, int y)
{
	if (!m_hButtonSurf || !g_hMainWnd)
		return false;

	RECT rcClient;
	if (!GetClientRect(g_hMainWnd, &rcClient) || rcClient.right <= 0 || rcClient.bottom <= 0)
		return false;

	uint32 nScreenWidth = 0, nScreenHeight = 0;
	g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nScreenWidth, &nScreenHeight);

	int sx = MulDiv(x, (int)nScreenWidth, rcClient.right);
	int sy = MulDiv(y, (int)nScreenHeight, rcClient.bottom);

	return sx >= m_ButtonRect.left && sx < m_ButtonRect.right &&
		   sy >= m_ButtonRect.top  && sy < m_ButtonRect.bottom;
}


// ----------------------------------------------------------------------- //
// Per-frame.
// ----------------------------------------------------------------------- //

void CStubClientShell::Update()
{
	HSURFACE hScreen = g_pLTClient->GetScreenSurface();
	uint32 nScreenWidth = 0, nScreenHeight = 0;
	g_pLTClient->GetSurfaceDims(hScreen, &nScreenWidth, &nScreenHeight);

	// Recomputed every frame so a render-mode change (e.g. from the console)
	// keeps everything centered.
	LayoutInterface(nScreenWidth, nScreenHeight);

	g_pLTClient->ClearScreen(LTNULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER, LTNULL);

	g_pLTClient->Start3D();
	g_pLTClient->StartOptimized2D();

	if (m_hHelloSurf)
	{
		g_pLTClient->DrawSurfaceToSurfaceTransparent(hScreen, m_hHelloSurf, LTNULL,
			m_HelloPos.x, m_HelloPos.y, kTransparent);
	}

	HSURFACE hButton = (m_bHover && m_hButtonHoverSurf) ? m_hButtonHoverSurf : m_hButtonSurf;
	if (hButton)
	{
		g_pLTClient->DrawSurfaceToSurfaceTransparent(hScreen, hButton, LTNULL,
			m_ButtonRect.left, m_ButtonRect.top, kTransparent);
	}

	for (int i = 0; i < kNumNoticeLines; i++)
	{
		if (m_hNoticeSurf[i])
		{
			g_pLTClient->DrawSurfaceToSurfaceTransparent(hScreen, m_hNoticeSurf[i], LTNULL,
				m_NoticePos[i].x, m_NoticePos[i].y, kTransparent);
		}
	}

	g_pLTClient->EndOptimized2D();
	g_pLTClient->End3D();

	g_pLTClient->FlipScreen(FLIPSCREEN_CANDRAWCONSOLE);
}

void CStubClientShell::OnKeyDown(int key, int rep)
{
	if (key == VK_ESCAPE)
		g_pLTClient->Shutdown();
}
