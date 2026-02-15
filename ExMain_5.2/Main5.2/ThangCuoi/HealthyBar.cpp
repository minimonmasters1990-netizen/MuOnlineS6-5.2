#include "Stdafx.h"
#include "StructSendGs.h"
#include "UIControls.h"
#include "ZzzInventory.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "ACuoi_DanhHieu.h"
#include "ACuoi_QuanHam.h"
#include "ACuoi_TuLuyen.h"
#include "ACuoi_HonHoan.h"
#include "NewUIPartyInfoWindow.h"
#include "MapManager.h"
#include "NewUIMessageBox.h"
#include "UIGuildInfo.h"
#include "ZzzToolKit.h"
#include "NewUIAccountLevel.h"
#include <ZzzLodTerrain.h>

RankUserClass gRank;
ClassLifeBar gClassLifeBar;

inline float PosDanhHieuY()
{
	float a = 0;
	switch (m_Resolution)
	{
		case 0: a = 63; break;
		case 1: a = 63; break;
		case 2: a = 62; break;
		case 3: a = 52; break;
		case 4: a = 49; break;
		case 5: a = 43; break;
		case 6: a = 43; break;
		case 7: a = 39; break;
		case 8: a = 39; break;
		case 9: a = 39; break;
		case 10: a = 39; break;
	}
	return a;
}
inline float PosQuanHamY()
{
	float a = 0;
	switch (m_Resolution)
	{
	case 0: a = 63; break;
	case 1: a = 63; break;
	case 2: a = 62; break;
	case 3: a = 52; break;
	case 4: a = 49; break;
	case 5: a = 43; break;
	case 6: a = 43; break;
	case 7: a = 39; break;
	case 8: a = 39; break;
	case 9: a = 39; break;
	case 10: a = 39; break;
	}
	return a;
}
inline float PosTuLuyenY()
{
	float a = 0;
	switch (m_Resolution)
	{
	case 0: a = 63; break;
	case 1: a = 63; break;
	case 2: a = 62; break;
	case 3: a = 52; break;
	case 4: a = 49; break;
	case 5: a = 43; break;
	case 6: a = 43; break;
	case 7: a = 39; break;
	case 8: a = 39; break;
	case 9: a = 39; break;
	case 10: a = 39; break;
	}
	return a;
}
void RankUserClass::RenderShowLogRank(int index, int PosX, int PosY)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int GetRankLevel = 0;
	int GetRankLevel1 = 0;
	int GetRankLevel2 = 0;
	int GetAccountLevel = 0;

	if (g_bRenderImageLogo)
	{
		GetRankLevel = gRank.m_Data[index].rDanhHieu;
		GetRankLevel1 = gRank.m_Data[index].rQuanHam;
		GetRankLevel2 = gRank.m_Data[index].rTuLuyen;
		//ACuoi Chỉnh Rank F8
		if (GetRankLevel >= 1 && GetRankLevel <= 50)
		{
			RenderBitmap((SEASON3B::CNewUIDanhHieu::IMAGE_HD_LOGO_START + GetRankLevel) - 1, PosX - 67, PosY - PosDanhHieuY() - 16, 135, 30, 0.f, 0.f, 1.0, 64.f / 64.f, 1, 1, 0);
		}
		if (GetRankLevel1 >= 1 && GetRankLevel1 <= 50)
		{
			RenderBitmap((SEASON3B::CNewUIQuanHam::IMAGE_HD_LOGO_START2 + GetRankLevel1) - 1, PosX - 26, PosY - PosQuanHamY() - 62, 65, 45, 0.f, 0.f, 1.0, 64.f / 64.f, 1, 1, 0);
		}
		if (GetRankLevel2 >= 1 && GetRankLevel2 <= 50)
		{
			RenderBitmap((SEASON3B::CNewUITuLuyen::IMAGE_HD_LOGO_START3 + GetRankLevel2) - 1, PosX - 59, PosY - PosTuLuyenY() - 35, 115, 30, 0.f, 0.f, 1.0, 64.f / 64.f, 1, 1, 0);
		}

		//// Todo VIP Logo OK
		//GetAccountLevel = gRank.m_Data[index].AccountType;
		//
		//if (GetAccountLevel >= 3)
		//{
		//	RenderBitmap(SEASON3B::CNewUIAccountLevel::IMAGE_IGS_LOGOV1, PosX - 11, PosY - 65, 40, 12, 0.f, 0.f, 1.0, 64.f / 256.f, 1, 1, 0);
		//}
	}
	DisableAlphaBlend();
}

void RankUserClass::RenderBarName()
{
	if (gMapManager.InChaosCastle(gMapManager.WorldActive) == true)
	{
		return;
	}

	for (int j = 0; j < MAX_CHARACTERS_CLIENT; ++j)
	{
		CHARACTER* c = &CharactersClient[j];
		OBJECT* o = &c->Object;

		if (c->Dead > 0.f || !o->Live || o->Kind != KIND_PLAYER)
		{
			continue;
		}

		vec3_t Position;
		int ScreenX, ScreenY;

		Vector(o->Position[0], o->Position[1], o->Position[2] + o->BoundingBoxMax[2] + 100.f, Position);
		Projection(Position, &ScreenX, &ScreenY);
		int AddPosY = 0;

		if (c->GuildMarkIndex >= 0 && GuildMark[c->GuildMarkIndex].UnionName[0])
		{
			AddPosY = 1;
		}
		RenderShowLogRank(c->Key, ScreenX, ScreenY - AddPosY);
	}

	DisableAlphaBlend();
	glColor3f(1.f, 1.f, 1.f);
}

void RankUserClass::GCReqRankLevelUser(const BYTE* SPK)
{
	auto DataSPK = reinterpret_cast<const PMSG_CUSTOM_RANKUSER*>(SPK);
	if (!DataSPK)
	{
		return;
	}

	auto& IsUser		= m_Data[DataSPK->iIndex];

	IsUser.m_Index		= DataSPK->iIndex;
	IsUser.m_Level		= DataSPK->iLevel;
	IsUser.rDanhHieu	= DataSPK->rDanhHieu;
	IsUser.rQuanHam		= DataSPK->rQuanHam;
	IsUser.rTuLuyen		= DataSPK->rTuLuyen;
	IsUser.rHonHoan		= DataSPK->rHonHoan;
	IsUser.AccountType	= DataSPK->AccountType;

	//g_ConsoleDebug->Write(3, "rHonHoan: %d", IsUser.rHonHoan);
	
	for (auto i = 0; i < 5; ++i)
	{
		ReqResetChange[i] = DataSPK->ReqResetChange[i];
		ReqResetUpPoint[i] = DataSPK->ReqResetUpPoint[i];
		ReqResetCoin[i] = DataSPK->ReqResetCoin[i];
	}

	std::memcpy(IsUser.szName, DataSPK->szName, sizeof(IsUser.szName));
}

void ClassLifeBar::Clear()
{
	for (auto& SPK : this->gNewHealthBar)
	{
		SPK.index	= 0xFFFF;
		SPK.type	= 0;
		SPK.rate	= 0;
		SPK.rate2	= 0;
		SPK.Level	= 0;
		SPK.Life	= 0;
		SPK.MonsID	= 0;
	}
}

void ClassLifeBar::Insert(WORD index, BYTE type, BYTE rate, BYTE rate2, short Level, float Life, WORD MonsID)
{
	for (auto& SPK : this->gNewHealthBar)
	{
		if (SPK.index == 0xFFFF)
		{
			SPK.index	= index;
			SPK.type	= type;
			SPK.rate	= rate;
			SPK.rate2	= rate2;
			SPK.Level	= Level;
			SPK.Life	= Life;
			SPK.MonsID	= MonsID;
			return;
		}
	}
}

ClassLifeData* ClassLifeBar::Get(WORD index, BYTE type)
{
	for (auto& SPK : gNewHealthBar)
	{
		if (SPK.index == index && SPK.type == type)
		{
			return &SPK;
		}
	}
	return nullptr;
}

void ClassLifeBar::Receive(const BYTE* lpMsg)
{
	auto DataSPK = reinterpret_cast<const RecvLifeCount*>(lpMsg);
	this->Clear();

	for (auto n = 0; n < DataSPK->count; ++n)
	{
		auto lpInfo = reinterpret_cast<const RecvLifeData*>(
			lpMsg + sizeof(RecvLifeCount) + sizeof(RecvLifeData) * n
			);

		Insert(lpInfo->index, lpInfo->type, lpInfo->rate, lpInfo->rate2, lpInfo->Level, lpInfo->Life, lpInfo->MonsID);
	}
}

void GetHPColorByRate(int rate, float& r, float& g, float& b)
{
	static float blinkTimer = 0.0f;
	blinkTimer += FPS_ANIMATION_FACTOR;

	if (rate > 80)
	{
		r = 0.0f;  g = 0.6f;  b = 0.0f;
	}
	else if (rate > 60)
	{
		r = 0.5f;  g = 0.7f;  b = 0.0f;
	}
	else if (rate > 50)
	{
		r = 1.0f;  g = 1.0f;  b = 0.0f;
	}
	else
	{
		float speed = 0.15f;
		if (rate <= 40) speed = 0.20f;
		if (rate <= 30) speed = 0.25f;
		if (rate < 30) speed = 0.30f;

		float brightness = 0.5f + 0.5f * sinf(blinkTimer * speed);
		r = brightness;
		g = brightness * 0.2f;
		b = brightness * 0.2f;
	}
}

// 血量格式化函数，将血量转换为W为单位的字符串
void FormatHealth(float health, char* output, size_t size)
{
	if (health >= 10000)
	{
		sprintf_s(output, size, "%.1fW", health / 10000.0f);
	}
	else
	{
		sprintf_s(output, size, "%.0f", health);
	}
}

void RenderHealthBar(int ScreenX, int ScreenY, float currentHealth, float maxHealth, float width = 60.0f, float r = -1.0f, float g = -1.0f, float b = -1.0f)
{
	// 计算生命值百分比
	float healthPercent = currentHealth / maxHealth;
	if (healthPercent > 1.0f) healthPercent = 1.0f;
	if (healthPercent < 0.0f) healthPercent = 0.0f;

	// 血条宽度和高度
	const float totalWidth = width;
	const float totalHeight = 4.0f;
	// 边框宽度（加粗）
	const float borderWidth = 1.5f;

	// 计算血条起始位置（居中显示）
	int startX = ScreenX - static_cast<int>(totalWidth / 2);
	int startY = ScreenY;

	// 计算边框位置和大小（加粗）
	int borderStartX = startX - static_cast<int>(borderWidth);
	int borderStartY = startY - static_cast<int>(borderWidth);
	int borderEndX = startX + static_cast<int>(totalWidth) + static_cast<int>(borderWidth);
	int borderEndY = startY + static_cast<int>(totalHeight) + static_cast<int>(borderWidth);
	int borderTotalWidth = borderEndX - borderStartX;
	int borderTotalHeight = borderEndY - borderStartY;

	// 渲染血条背景（黑色边框，加粗）
	glColor3f(0.0f, 0.0f, 0.0f);

	// 绘制矩形边框
	RenderColor(borderStartX, borderStartY, borderTotalWidth, borderTotalHeight);

	// 渲染血条（非分段）
	if (r < 0.0f || g < 0.0f || b < 0.0f)
	{
		// 使用高亮绿色
		glColor3f(0.0f, 1.0f, 0.0f);
	}
	else
	{
		// 使用自定义颜色
		glColor3f(r, g, b);
	}

	// 渲染血条主体
	int healthBarWidth = static_cast<int>(totalWidth * healthPercent);
	RenderColor(startX, startY, healthBarWidth, totalHeight);

	// 恢复默认颜色
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
}

// 修改ClassLifeBar::Render函数
void ClassLifeBar::Render()
{
	const float Width = 50.f;
	char Text[100], RateHP[100];

	if (!g_bRenderNameMonster)
		return;

	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);

	// 渲染当前玩家（Hero）的血条
	if (Hero && Hero->Object.Live && Hero->Dead <= 0.f)
	{
		vec3_t Position;
		int ScreenX, ScreenY;

		// 计算血条位置（与角色名称相同的位置，然后下移）
		Vector(Hero->Object.Position[0], Hero->Object.Position[1], Hero->Object.Position[2] + Hero->Object.BoundingBoxMax[2] + 100.f, Position);
		Projection(Position, &ScreenX, &ScreenY);
		ScreenX -= static_cast<int>(Width / 2); // 使血条居中显示

		// 计算生命值百分比
		int LifePercent = 0;

		if (CharacterAttribute && CharacterAttribute->ViewMaxHP > 0)
		{
			LifePercent = static_cast<int>((static_cast<float>(CharacterAttribute->ViewCurHP) / CharacterAttribute->ViewMaxHP) * 100);
			LifePercent = LIMIT(LifePercent, 0, 100);
		}

		// 渲染血条（在角色名称下方10像素）
		RenderHealthBar(ScreenX +25, ScreenY - 2, CharacterAttribute->ViewCurHP, CharacterAttribute->ViewMaxHP);
	}

	// 遍历所有其他角色
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; ++i)
	{
		CHARACTER* c = &CharactersClient[i];
		OBJECT* o = &c->Object;

		// 跳过死亡或无效的角色，以及当前玩家（Hero）
		if (!o->Live || c->Dead > 0.f || c == Hero)
			continue;

		// 跳过 NPC
		if (c->Object.Kind == KIND_NPC)
			continue;

		// 获取血条数据
		ClassLifeData* lpNewHealthBar = Get(c->Key, c->Object.Kind);
		if (lpNewHealthBar == nullptr)
			continue;

		// 计算血条在屏幕上的位置
		vec3_t Position;
		int ScreenX, ScreenY;
		Vector(o->Position[0], o->Position[1], o->Position[2] + o->BoundingBoxMax[2] + 150.f, Position);
		Projection(Position, &ScreenX, &ScreenY);
		ScreenX -= static_cast<int>(Width / 2); // 使血条居中显示

		// 计算生命值百分比
		int LifePercent = lpNewHealthBar->rate / 1.8;
		float iHP = static_cast<float>(LifePercent);

		if (c->Dead != 0)
		{
			iHP -= c->Dead;
			if (iHP < 0)
				iHP = 0;
		}

		// 检查是否为怪物
		int ModelMonster = gCustomModelNPC.IsMonster(c->MonsterIndex);
		int MonsterClass = IsToolKit.GetIndexMonster(c->MonsterIndex);

		// 怪物血条渲染
		if (MonsterClass || ModelMonster || c->MonsterIndex == 34 || c->Object.SubType == MODEL_SKELETON1 || c->Object.SubType == MODEL_SKELETON2 || c->Object.SubType == MODEL_SKELETON3 || c->Object.Kind == KIND_MONSTER && c->Object.Type != MODEL_PLAYER && c->Object.Kind != KIND_TRAP)
		{
			// 渲染怪物名字
			g_pRenderText->SetFont(g_hFont);
			g_pRenderText->SetBgColor(0);
			g_pRenderText->SetTextColor(255, 255, 255, 255);
			g_pRenderText->RenderText(ScreenX - 86, ScreenY - 5, c->ID, 200, 0, RT3_SORT_CENTER);

			// 渲染怪物等级和血量
			char LevelHealthText[128];
			char HealthStr[32];
			FormatHealth(lpNewHealthBar->Life, HealthStr, sizeof(HealthStr));
			sprintf(LevelHealthText, "Lv.%d - %s", lpNewHealthBar->Level, HealthStr);
			
			g_pRenderText->SetTextColor(255, 255, 255, 255);
			g_pRenderText->RenderText(ScreenX - 86, ScreenY + 3, LevelHealthText, 200, 0, RT3_SORT_CENTER);

			IsToolKit.RenderToolTip(ScreenX - 15.f, ScreenY + 13.f, 55.f, 3.2f);
			EnableAlphaTest();

			float r, g, b;
			GetHPColorByRate(lpNewHealthBar->rate, r, g, b);
			glColor3f(r, g, b);
			RenderColor((ScreenX - 17) + 2, (ScreenY - 6) + 19, iHP, 3);
			glEnable(GL_TEXTURE_2D);
			glColor3f(1.0f, 1.0f, 1.0f);
		}
		else
		{
			// 玩家血条渲染
			if (!c->SafeZone)
			{
				IsToolKit.RenderToolTip(ScreenX - 15.f, ScreenY + 3.f, 55.f, 3.2f);
				RenderImage(SEASON3B::CNewUIPartyInfoWindow::IMAGE_PARTY_HPBAR, (ScreenX - 17) + 2, (ScreenY - 6) + 9, iHP, 3);
			}
		}
	}

	DisableAlphaBlend();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


