#include "Stdafx.h"
#include "NewUIRenderInvasion.h"

#include "NewUISystem.h"
#include "WSclientinline.h"
#include "ThangCuoi\ZzzToolKit.h"
#include "ThangCuoi\WideData.h"
#include "ThangCuoi\StructSendGs.h"
#include "NewUIScrollBar.h"

CNewUIRenderInvasionExt gInvasionExt;

SEASON3B::CNewUIScrollBar* CUIScrollBar = NULL;

// ---------- 可调常量：修改下面几个值即可改变界面位置与尺寸 ----------
static const int INVASION_WINDOW_WIDTH = 160;      // 窗口宽度（像素）
static const int INVASION_WINDOW_HEIGHT = 150;     // 窗口高度（像素）
static const int INVASION_LIST_MAX = 10;           // 每页显示多少条
static const int INVASION_ITEM_HEIGHT = 10;        // 每条高度（像素）
static const int INVASION_PADDING_LEFT = 308;        // 窗口内左边距（文本起始x偏移）
static const int INVASION_PADDING_TOP = 10;        // 窗口内上边距（文本起始y偏移）
static const int INVASION_SCROLLBAR_WIDTH = 12;    // 滚动条宽度
// ----------------------------------------------------------------

SEASON3B::CNewUIRenderInvasion::CNewUIRenderInvasion()
{
	m_pNewUIMng = nullptr;
	m_Pos.x = 0;
	m_Pos.y = 0;
	gInvasionExt.m_Data.clear();
}

SEASON3B::CNewUIRenderInvasion::~CNewUIRenderInvasion()
{
	Release();
}

bool SEASON3B::CNewUIRenderInvasion::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_EVENINVASION, this);

	SetPos(x, y);

	Show(false);

	return true;
}

void SEASON3B::CNewUIRenderInvasion::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUIRenderInvasion::SetPos(int x, int y)
{
	// 特殊约定：如果传入 x == -1 && y == -1，窗口固定到屏幕右上角（带 margin）
	const int marginRight = 8; // 距屏幕右边距（像素），需要可根据需求调整
	const int marginTop = 8;   // 距屏幕顶部（像素）

	if (x == -1 && y == -1)
	{
		int screenW = IsToolKit.GetPositionScreen(); // 原代码也用过此函数获取屏宽
		// 设 gInvasionExt.m_Pos.x，使得 winLeft = screenW - INVASION_WINDOW_WIDTH
		// 因为 winLeft = gInvasionExt.m_Pos.x - INVASION_WINDOW_WIDTH + INVASION_PADDING_LEFT
		// 解出 gInvasionExt.m_Pos.x = winLeft + INVASION_WINDOW_WIDTH - INVASION_PADDING_LEFT
		int winLeft = screenW - INVASION_WINDOW_WIDTH - marginRight;
		gInvasionExt.m_Pos.x = winLeft + INVASION_WINDOW_WIDTH - INVASION_PADDING_LEFT;
		gInvasionExt.m_Pos.y = marginTop;

		// 同步 m_Pos，保持内部一致（若其他逻辑使用 m_Pos）
		m_Pos.x = gInvasionExt.m_Pos.x;
		m_Pos.y = gInvasionExt.m_Pos.y;
		return;
	}

	// 之前的行为：以传入 x,y（+偏移）作为基准
	m_Pos.x = x + gPosWide.x_GetAddPos + 200;
	m_Pos.y = y;

	// gInvasionExt.m_Pos 表示界面右侧基准点（原代码用 GetPositionScreen 计算）
	// 现在把界面基准统一到 m_Pos，调整偏移量用常量控制
	// 如果想让窗口固定在屏幕中央，可直接使用 IsToolKit.GetPositionScreen()/2 等
	gInvasionExt.m_Pos.x = m_Pos.x; // 可以改为 m_Pos.x + OFFSET 来微调
	gInvasionExt.m_Pos.y = m_Pos.y;
}

bool SEASON3B::CNewUIRenderInvasion::Update()
{
	return true;
}

bool SEASON3B::CNewUIRenderInvasion::UpdateMouseEvent()
{
	// 鼠标检查区域改为以 gInvasionExt.m_Pos 为基准并使用常量尺寸
	int winLeft = gInvasionExt.m_Pos.x - INVASION_WINDOW_WIDTH + INVASION_PADDING_LEFT;
	int winTop = gInvasionExt.m_Pos.y;
	if (CheckMouseIn(winLeft, winTop, INVASION_WINDOW_WIDTH, INVASION_WINDOW_HEIGHT))
		return false;
	return true;
}

bool SEASON3B::CNewUIRenderInvasion::UpdateKeyEvent()
{
	if (IsVisible())
	{
		if (SEASON3B::IsPress(VK_ESCAPE) == true)
		{
			g_pNewUIMenuOption->Hide(SEASON3B::INTERFACE_EVENINVASION);
			PlayBuffer(SOUND_CLICK01);

			return false;
		}
	}
	return true;
}

float SEASON3B::CNewUIRenderInvasion::GetLayerDepth()
{
	return 3.4;
}

float SEASON3B::CNewUIRenderInvasion::GetKeyEventOrder()
{
	return 3.4;
}

bool SEASON3B::CNewUIRenderInvasion::Render()
{
	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);

	gInvasionExt.Initz();

	DisableAlphaBlend();
	return true;
}

void CNewUIRenderInvasionExt::GetInfoReceived(const BYTE* lpMsg)
{
	auto DataSPK = reinterpret_cast<const InvasionDataReceived*>(lpMsg);
	this->m_Data.clear();

	for (auto n = 0; n < DataSPK->Counter; ++n)
	{
		auto info = reinterpret_cast<const InvasionDataActive*>(
			lpMsg + sizeof(InvasionDataReceived) + sizeof(InvasionDataActive) * n
			);

		if (info->Counter.IsCount == 0 && info->Counter.IsMaxCount == 0)
		{
			continue;
		}

		InvasionActive g_Data;
		g_Data.MonsterIndex = info->MonsterIndex;
		g_Data.Counter = info->Counter;

		this->m_Data.push_back(g_Data);
	}

	g_pNewUIMenuOption->IsVisible(SEASON3B::INTERFACE_EVENINVASION);
}

void CNewUIRenderInvasionExt::GetUpdateMonster(const BYTE* lpMsg)
{
	auto DataSPK = reinterpret_cast<const InvasionMonsterReceived*>(lpMsg);

	for (auto it = this->m_Data.begin(); it != this->m_Data.end();)
	{
		if (it->MonsterIndex == DataSPK->MonsterIndex)
		{
			it->Counter.IsCount = DataSPK->Counter;

			if (it->Counter.IsCount == 0 && it->Counter.IsMaxCount == 0)
			{
				it = this->m_Data.erase(it);
			}
			break;
		}
		else
		{
			++it;
		}
	}
}

void CNewUIRenderInvasionExt::Initz()
{
	// 计算窗口左上角坐标（以 m_Pos 为右侧参考）
	int winLeft = this->m_Pos.x - INVASION_WINDOW_WIDTH + INVASION_PADDING_LEFT;
	int winTop = this->m_Pos.y;

	IsToolKit.ThisFont(winLeft, winTop, HEX_COLOR_WHITE, 200, INVASION_WINDOW_WIDTH, 145, 1, " ");

	this->ListMaxPer[0] = INVASION_LIST_MAX;
	this->DataList[0] = this->m_Data.size();

	if (CUIScrollBar == NULL)
	{
		CUIScrollBar = new SEASON3B::CNewUIScrollBar();
		// 将滚动条放在窗口右侧内边距位置
		int scrollbarX = winLeft + INVASION_WINDOW_WIDTH - INVASION_SCROLLBAR_WIDTH - 6;
		CUIScrollBar->Create(scrollbarX, winTop + INVASION_PADDING_TOP, INVASION_WINDOW_HEIGHT - INVASION_PADDING_TOP - 10);
	}

	if (CUIScrollBar)
	{
		int maxPos = (this->DataList[0] > this->ListMaxPer[0]) ? (this->DataList[0] - this->ListMaxPer[0]) : 0;
		CUIScrollBar->SetMaxPos(maxPos);

		CUIScrollBar->MouseWheelWindow = CheckMouseIn(winLeft + 16, winTop + INVASION_PADDING_TOP, INVASION_WINDOW_WIDTH - 30, INVASION_WINDOW_HEIGHT - INVASION_PADDING_TOP);
		CUIScrollBar->Render();
		CUIScrollBar->UpdateMouseEvent();
		CUIScrollBar->Update();
	}
	this->CurPos[0] = CUIScrollBar ? CUIScrollBar->GetCurPos() : 0;
	this->Count[0] = 0;

	EnableAlphaTest();

	if (this->m_Data.size() == 0)
	{
		IsToolKit.ThisFont(winLeft + 8, winTop + INVASION_WINDOW_HEIGHT - 25, HEX_COLOR_WHITE, 0, INVASION_WINDOW_WIDTH - 16, 0, 3, GlobalText[3178]);
	}
	else
	{
		for (int i = this->CurPos[0]; i < (int)this->m_Data.size(); i++)
		{
			if (this->Count[0] >= this->ListMaxPer[0])
				break;

			float PosY = winTop + INVASION_PADDING_TOP + (this->Count[0] * INVASION_ITEM_HEIGHT);
			char* name = getMonsterName(this->m_Data[i].MonsterIndex);

			// 名称和计数显示
			IsToolKit.ThisFont(winLeft + 8,  PosY + 3, HEX_COLOR_Yellow, 0, INVASION_WINDOW_WIDTH - INVASION_SCROLLBAR_WIDTH - 24, 0, 1, "%d.%s", i + 1, name);
			IsToolKit.ThisFont(winLeft + 8, PosY + 3, HEX_COLOR_Yellow, 0, INVASION_WINDOW_WIDTH - INVASION_SCROLLBAR_WIDTH - 24, 0, 4, "%d/%d", this->m_Data[i].Counter.IsCount, this->m_Data[i].Counter.IsMaxCount);
			this->Count[0]++;
		}
	}
	IsToolKit.ThisFont(winLeft, winTop, HEX_COLOR_WHITE, 0x00FBFF69, INVASION_WINDOW_WIDTH, 0, 3, GlobalText[3177]);

	DisableAlphaBlend();
}