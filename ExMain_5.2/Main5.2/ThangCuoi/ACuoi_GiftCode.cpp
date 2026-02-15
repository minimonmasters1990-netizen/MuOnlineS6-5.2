#include "Stdafx.h"
#include "ACuoi_GiftCode.h"
#include "WSclientinline.h"
#include "ZzzToolKit.h"
#include "WideData.h"

/// ------------------------------------------------------------------------
/// Class x? ly UI GiftCode (Season3B)
/// - Cho ph¨¦p nh?p GiftCode ?? g?i v? GS
/// - UI g?m: khung, textbox, n¨²t x¨¢c nh?n
/// - ?? lo?i b? c? c?nh b¨¢o kh?ng d¨´ng
/// ------------------------------------------------------------------------
SEASON3B::CNewUIGiftCode::CNewUIGiftCode()
{
	m_pNewUIMng = nullptr;
	m_pGiftCodeInput = nullptr; // ch? c?n 1 textbox nh?p GiftCode

	m_Pos.x = 0;
	m_Pos.y = 0;

	WINDOW_WIDTH = 380;
	WINDOW_HEIGHT = 190;
}

SEASON3B::CNewUIGiftCode::~CNewUIGiftCode()
{
	Release(); // Gi?i ph¨®ng khi hu? ??i t??ng
}

bool SEASON3B::CNewUIGiftCode::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (pNewUIMng == nullptr)
		return false;

	// G?n UI manager
	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_GIFTCODE, this);

	SetPos(x, y);
	InitButtons();
	LoadImages();
	// Kh?i t?o ? nh?p GiftCode
	m_pGiftCodeInput = new CUITextInputBox;
	m_pGiftCodeInput->Init(g_hWnd, 80, 10, 10, false); // false = kh?ng ?n text
	m_pGiftCodeInput->SetPosition(m_Pos.x + 165, m_Pos.y + 192);
	m_pGiftCodeInput->SetTextColor(255, 255, 0, 255);
	m_pGiftCodeInput->SetBackColor(0, 0, 0, 128);     // n?n t?i m?, d? nh¨¬n
	m_pGiftCodeInput->SetFont(g_hFont);
	m_pGiftCodeInput->SetState(UISTATE_NORMAL);

	Show(false);
	return true;
}

void SEASON3B::CNewUIGiftCode::Release()
{
	// Gi?i ph¨®ng textbox
	SAFE_DELETE(m_pGiftCodeInput);

	// H?y li¨ºn k?t v?i UI manager
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = nullptr;
	}
}

void SEASON3B::CNewUIGiftCode::ClearTextBoxes()
{
	// Reset text nh?p v? r?ng
	if (m_pGiftCodeInput)
		m_pGiftCodeInput->SetText("");
}

void SEASON3B::CNewUIGiftCode::SetPos(int x, int y)
{
	m_Pos.x = (IsToolKit.GetPositionScreen() - WINDOW_WIDTH) / 2;
	m_Pos.y = (IsToolKit.GetCreatePosHeight() - (WINDOW_HEIGHT + 70.0f)) / 2;
}

bool SEASON3B::CNewUIGiftCode::Update()
{
	if (!IsVisible())
		return true;

	// C?p nh?t tr?ng th¨¢i input box
	if (m_pGiftCodeInput)
		m_pGiftCodeInput->DoAction();

	return true;
}

bool SEASON3B::CNewUIGiftCode::UpdateMouseEvent()
{
	if (!IsVisible())
	{
		// N?u UI b? ?n nh?ng textbox c¨°n gi? focus -> reset v? c?a s? game
		if (m_pGiftCodeInput && m_pGiftCodeInput->HaveFocus())
			SetFocus(g_hWnd);
		return false;
	}

	if (m_Btn[0].UpdateMouseEvent()) // N¨²t x¨¢c nh?n
	{
		char GiftCode[11] = { 0 };

		if (m_pGiftCodeInput)
			m_pGiftCodeInput->GetText(GiftCode);

		if (strlen(GiftCode) > 0)
		{
			char zChat[64];
			sprintf(zChat, "/code %s", GiftCode);

			// G?i qua h¨¤m chu?n
			SendChat(zChat);
			// Reset UI
			m_pGiftCodeInput->SetText("");
			if (m_pGiftCodeInput->HaveFocus())SetFocus(g_hWnd);g_pNewUIMenuOption->Hide(SEASON3B::INTERFACE_GIFTCODE);
			PlayBuffer(SOUND_CLICK01);

		}
		return true;
	}
	// X? ly n¨²t tho¨¢t (d?u X)
	if (SEASON3B::IsRelease(VK_LBUTTON) && CheckMouseIn((m_Pos.x + WINDOW_WIDTH) - 40, m_Pos.y + 5, 16, 16))
	{
		if (m_pGiftCodeInput && m_pGiftCodeInput->HaveFocus())
			SetFocus(g_hWnd);

		g_pNewUIMenuOption->Hide(SEASON3B::INTERFACE_GIFTCODE);
	}

	// N?u click trong v¨´ng UI th¨¬ kh?ng cho click xuy¨ºn
	if (CheckMouseIn(m_Pos.x, m_Pos.y, WINDOW_WIDTH - 10, WINDOW_HEIGHT + 70))
		return false;

	return true;
}

bool SEASON3B::CNewUIGiftCode::UpdateKeyEvent()
{
	if (IsVisible())
	{
		// Nh?n ESC ?? ?¨®ng
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			if (m_pGiftCodeInput && m_pGiftCodeInput->HaveFocus())
				SetFocus(g_hWnd);

			g_pNewUIMenuOption->Hide(SEASON3B::INTERFACE_GIFTCODE);
			PlayBuffer(SOUND_CLICK01);

			return false;
		}
	}
	return true;
}

float SEASON3B::CNewUIGiftCode::GetLayerDepth()
{
	return 3.4f;
}

float SEASON3B::CNewUIGiftCode::GetKeyEventOrder()
{
	return 3.4f;
}

void SEASON3B::CNewUIGiftCode::LoadImages()
{
	LoadBitmap("Interface\\ACuoi\\Gifcode.tga", IMAGE_HD_LOGO_0, GL_LINEAR);
}

bool SEASON3B::CNewUIGiftCode::Render()
{
	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);

	g_pUIForm->NewRenderForm(m_Pos.x, m_Pos.y, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, "Giftcode Tan Th?");
	//g_pUIForm->RenderBack(m_Pos.x + 20, m_Pos.y + 40, 257.f, 25);
	//g_pUIForm->RenderBack(m_Pos.x + 5, m_Pos.y + 10, 257.f, 260);

	RenderBitmap((SEASON3B::CNewUIGiftCode::IMAGE_HD_LOGO_0), 252,127, 588, 322, 0.f, 0.f, 1.0, 128.f / 128.f, 1, 1, 0);

	IsToolKit.ThisFont(m_Pos.x, m_Pos.y + 30, 0xFFDE26FF, 0, 360, 0, RT3_SORT_CENTER, "?? Tr?ng H¨°m ?? Tr??c Khi Nh?n Code");

	m_Btn[0].Render();
	// Render n¨²t b?m
	m_Btn[0].Render();

	// Render textbox nh?p li?u
	if (m_pGiftCodeInput)
		m_pGiftCodeInput->Render();

	DisableAlphaBlend();
	return true;
}

void SEASON3B::CNewUIGiftCode::InitButtons()
{
	// N¨²t x¨¢c nh?n GiftCode
	g_pUIForm->SetButtonInfo(&m_Btn[0],CNewUIGiftCode::IMAGE_IGS_BUTTON,m_Pos.x + 165,m_Pos.y + 212,52, 26,1, 0, 1, 1u,"Nh?n Code", "", 0
	);
}
