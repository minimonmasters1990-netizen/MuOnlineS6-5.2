//*****************************************************************************
// File: NewUINPCDialogue.cpp
//*****************************************************************************

#include "stdafx.h"
#include "NewUINPCDialogue.h"
#include "wsclientinline.h"
#include "CRTDBG.h"

using namespace SEASON3B;

#define ND_NPC_MAX_LINE_PER_PAGE		7
#define ND_SEL_TEXT_MAX_LINE_PER_PAGE	11
#define ND_TEXT_GAP						15

CNewUINPCDialogue::CNewUINPCDialogue()
{
	m_pNewUIMng = NULL;
	m_Pos.x = m_Pos.y = 0;
	m_dwContributePoint = 0;
}

CNewUINPCDialogue::~CNewUINPCDialogue()
{
	Release();
}

bool CNewUINPCDialogue::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_NPC_DIALOGUE, this);

	SetPos(x, y);
	LoadImages();

	m_btnProgressL.ChangeButtonImgState(true, IMAGE_ND_BTN_L);
	m_btnProgressL.ChangeButtonInfo(x + 131, y + 165, 17, 18);
	m_btnProgressR.ChangeButtonImgState(true, IMAGE_ND_BTN_R);
	m_btnProgressR.ChangeButtonInfo(x + 153, y + 165, 17, 18);

	m_btnSelTextL.ChangeButtonImgState(true, IMAGE_ND_BTN_L);
	m_btnSelTextL.ChangeButtonInfo(x + 131, y + 372, 17, 18);
	m_btnSelTextR.ChangeButtonImgState(true, IMAGE_ND_BTN_R);
	m_btnSelTextR.ChangeButtonInfo(x + 153, y + 372, 17, 18);

	m_btnClose.ChangeButtonImgState(true, IMAGE_ND_BTN_CLOSE);
	m_btnClose.ChangeButtonInfo(x + 13, y + 392, 36, 29);
	m_btnClose.ChangeToolTipText(GlobalText[1002], true);

	m_nSelTextCount = 0;
	m_bQuestListMode = false;

	Show(false);

	return true;
}

void CNewUINPCDialogue::Release()
{
	UnloadImages();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void CNewUINPCDialogue::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;

	m_btnProgressL.SetPos(x + 131, y + 165);
	m_btnProgressR.SetPos(x + 153, y + 165);
	m_btnSelTextL.SetPos(x + 131, y + 372);
	m_btnSelTextR.SetPos(x + 153, y + 372);
	m_btnClose.SetPos(x + 13, y + 392);
}

bool CNewUINPCDialogue::UpdateMouseEvent()
{
	if (ProcessBtns())
		return false;

	if (UpdateSelTextMouseEvent())
		return false;

	if(CheckMouseIn(m_Pos.x, m_Pos.y, ND_WIDTH, ND_HEIGHT))
		return false;

	return true;
}

bool CNewUINPCDialogue::ProcessBtns()
{
	if (m_btnClose.UpdateMouseEvent())
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_NPC_DIALOGUE);
		return true;
	}
	else if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(m_Pos.x+169, m_Pos.y+7, 13, 12))
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_NPC_DIALOGUE);
		return true;
	}
	else if (m_btnProgressR.UpdateMouseEvent())
	{
		m_nSelNPCPage = MIN(++m_nSelNPCPage, m_nMaxNPCPage);
		if (m_nSelNPCPage == m_nMaxNPCPage)
		{
			if (NON_SEL_TEXTS_MODE == m_eLowerView)
				m_eLowerView = SEL_TEXTS_MODE;
		}
		
		::PlayBuffer(SOUND_CLICK01);
		
		if (m_nSelNPCPage == m_nMaxNPCPage && NON_SEL_TEXTS_MODE != m_eLowerView)
			m_btnProgressR.Lock();
		if (0 != m_nMaxNPCPage)
			m_btnProgressL.UnLock();
		
		return true;
	}
	else if (m_btnProgressL.UpdateMouseEvent())
	{
		m_nSelNPCPage = MAX(--m_nSelNPCPage, 0);
		::PlayBuffer(SOUND_CLICK01);
		
		if (0 == m_nSelNPCPage)
			m_btnProgressL.Lock();
		m_btnProgressR.UnLock();
		
		return true;
	}
	else if (m_btnSelTextR.UpdateMouseEvent())
	{
		m_nSelSelTextPage = MIN(++m_nSelSelTextPage, m_nMaxSelTextPage);
		::PlayBuffer(SOUND_CLICK01);

		if (m_nSelSelTextPage == m_nMaxSelTextPage)
			m_btnSelTextR.Lock();
		if (0 != m_nMaxSelTextPage)
			m_btnSelTextL.UnLock();
		
		return true;
	}
	else if (m_btnSelTextL.UpdateMouseEvent())
	{
		m_nSelSelTextPage = MAX(--m_nSelSelTextPage, 0);
		::PlayBuffer(SOUND_CLICK01);

		if (0 == m_nSelSelTextPage)
			m_btnSelTextL.Lock();
		m_btnSelTextR.UnLock();
		
		return true;
	}

	return false;
}

bool CNewUINPCDialogue::UpdateSelTextMouseEvent()
{
	if (SEL_TEXTS_MODE != m_eLowerView || !m_bCanClick)
		return false;

	m_nSelSelText = 0;
	if (MouseX < m_Pos.x+11 || MouseX > m_Pos.x+179)
		return false;

	int i;

	int nStartSelText = 0;
	for (i = 0; i < m_nSelSelTextPage; ++i)
		nStartSelText += m_anSelTextCountPerPage[i];

	int nEndSelText = nStartSelText + m_anSelTextCountPerPage[m_nSelSelTextPage];

	int nTopY;
	int nBottomY = m_Pos.y+203;
	for (i = nStartSelText; i < nEndSelText; ++i)
	{
		nTopY = nBottomY;
		nBottomY += m_anSelTextLine[i] * ND_TEXT_GAP;

		if (nTopY <= MouseY && MouseY < nBottomY)
		{
			m_nSelSelText = i + 1;
			if (SEASON3B::IsRelease(VK_LBUTTON))
			{
				m_bCanClick = false;
				ProcessSelTextResult();
				::PlayBuffer(SOUND_CLICK01);
				return true;
			}
			break;
		}
	}

	return false;
}

bool CNewUINPCDialogue::UpdateKeyEvent()
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPC_DIALOGUE))
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_NPC_DIALOGUE);
			return false;
		}
	}

	return true;
}

bool CNewUINPCDialogue::Update()
{
	return true;
}

bool CNewUINPCDialogue::Render()
{
	::EnableAlphaTest();

	::glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	RenderBackImage();
	RenderSelTextBlock();
	RenderText();
	RenderContributePoint();

	if (!m_btnProgressL.IsLock())
		m_btnProgressL.Render();
	if (!m_btnProgressR.IsLock())
		m_btnProgressR.Render();

	if (!m_btnSelTextL.IsLock())
		m_btnSelTextL.Render();
	if (!m_btnSelTextR.IsLock())
		m_btnSelTextR.Render();

	m_btnClose.Render();

	::DisableAlphaBlend();
	
	return true;
}

void CNewUINPCDialogue::RenderBackImage()
{
	RenderImage(IMAGE_ND_BACK, m_Pos.x + 2, m_Pos.y + 3, 186.f, 426.f);
	RenderImage(IMAGE_ND_TOP, m_Pos.x, m_Pos.y, float(ND_WIDTH), 64.f);
	RenderImage(IMAGE_ND_LEFT, m_Pos.x, m_Pos.y+64, 21.f, 320.f);
	RenderImage(IMAGE_ND_RIGHT, m_Pos.x+ND_WIDTH-21, m_Pos.y+64, 21.f, 320.f);
	RenderImage(IMAGE_ND_BOTTOM, m_Pos.x, m_Pos.y+ND_HEIGHT-45, float(ND_WIDTH), 45.f);

	RenderImage(IMAGE_ND_LINE, m_Pos.x+1, m_Pos.y+181, 188.f, 21.f);
}

void CNewUINPCDialogue::RenderSelTextBlock()
{
	if (SEL_TEXTS_MODE != m_eLowerView)
		return;

	if (0 == m_nSelSelText)
		return;

	// 检查m_nSelSelText是否有效
	if (m_nSelSelText > ND_QUEST_INDEX_MAX_COUNT)
	{
		return;
	}

	int i;

	int nStartSelText = 0;
	for (i = 0; i < m_nSelSelTextPage; ++i)
		nStartSelText += m_anSelTextCountPerPage[i];

	int nBlockPosY = m_Pos.y+203;

	for (i = nStartSelText; i < m_nSelSelText - 1; ++i)
	{
		// 检查i是否有效
		if (i < 0 || i >= ND_QUEST_INDEX_MAX_COUNT)
		{
			return;
		}
		nBlockPosY += ND_TEXT_GAP * m_anSelTextLine[i];
	}

	// 检查m_nSelSelText - 1是否有效
	if (m_nSelSelText - 1 < 0 || m_nSelSelText - 1 >= ND_QUEST_INDEX_MAX_COUNT)
	{
		return;
	}

	::glColor4f(0.5f, 0.7f, 0.3f, 0.5f);
	::RenderColor(m_Pos.x+11, nBlockPosY, 168.f, ND_TEXT_GAP * m_anSelTextLine[m_nSelSelText - 1]);
	::EndRenderColor();
}

void CNewUINPCDialogue::RenderText()
{
	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetBgColor(0);

	g_pRenderText->SetTextColor(150, 255, 240, 255);
	g_pRenderText->RenderText(m_Pos.x, m_Pos.y+10, g_QuestMng.GetNPCName(),ND_WIDTH, 0, RT3_SORT_CENTER);

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(255, 230, 210, 255);
	int i;
	for (i = 0; i < ND_NPC_MAX_LINE_PER_PAGE; ++i)
		g_pRenderText->RenderText(m_Pos.x+13, m_Pos.y+59+(ND_TEXT_GAP*i),
			m_aszNPCWords[i + ND_NPC_MAX_LINE_PER_PAGE * m_nSelNPCPage],
			0, 0, RT3_SORT_LEFT);

	if (SEL_TEXTS_MODE == m_eLowerView)
	{
		g_pRenderText->SetTextColor(255, 230, 210, 255);
		
		int nStartSelTextLine = 0;
		for (i = 0; i < m_nSelSelTextPage; ++i)
			nStartSelTextLine += m_anSelTextLinePerPage[i];

		for (i = 0; i < m_anSelTextLinePerPage[m_nSelSelTextPage]; ++i)
			g_pRenderText->RenderText(m_Pos.x+13, m_Pos.y+207+(ND_TEXT_GAP*i),
				m_aszSelTexts[nStartSelTextLine+i], 0, 0, RT3_SORT_LEFT);
	}
}

void CNewUINPCDialogue::RenderContributePoint()
{
	if ((543 == g_QuestMng.GetNPCIndex() && 1 == Hero->m_byGensInfluence)
		|| (544 == g_QuestMng.GetNPCIndex() && 2 == Hero->m_byGensInfluence))
	{
		RenderImage(IMAGE_ND_CONTRIBUTE_BG, m_Pos.x+11, m_Pos.y+27, 168.f, 18.f);

		char szContribute[32];
		::sprintf(szContribute, GlobalText[2986], m_dwContributePoint);
		g_pRenderText->SetTextColor(255, 230, 210, 255);
		g_pRenderText->RenderText(m_Pos.x, m_Pos.y+30, szContribute, ND_WIDTH, 0, RT3_SORT_CENTER);
	}
}

bool CNewUINPCDialogue::IsVisible() const
{ 
	return CNewUIObj::IsVisible(); 
}

float CNewUINPCDialogue::GetLayerDepth()
{
	return 3.1f;
}

void CNewUINPCDialogue::LoadImages()
{
	LoadBitmap("Interface\\newui_msgbox_back.jpg", IMAGE_ND_BACK, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back04.tga", IMAGE_ND_TOP, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-L.tga", IMAGE_ND_LEFT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-R.tga", IMAGE_ND_RIGHT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back03.tga", IMAGE_ND_BOTTOM, GL_LINEAR);

	LoadBitmap("Interface\\newui_myquest_Line.tga", IMAGE_ND_LINE, GL_LINEAR);
	LoadBitmap("Interface\\Quest_bt_L.tga", IMAGE_ND_BTN_L, GL_LINEAR);
	LoadBitmap("Interface\\Quest_bt_R.tga", IMAGE_ND_BTN_R, GL_LINEAR);
	LoadBitmap("Interface\\newui_exit_00.tga", IMAGE_ND_BTN_CLOSE, GL_LINEAR);
	LoadBitmap("Interface\\Gens_point.tga", IMAGE_ND_CONTRIBUTE_BG, GL_LINEAR);
}

void CNewUINPCDialogue::UnloadImages()
{
	DeleteBitmap(IMAGE_ND_CONTRIBUTE_BG);
	DeleteBitmap(IMAGE_ND_BTN_CLOSE);
	DeleteBitmap(IMAGE_ND_BTN_R);
	DeleteBitmap(IMAGE_ND_BTN_L);
	DeleteBitmap(IMAGE_ND_LINE);

	DeleteBitmap(IMAGE_ND_BOTTOM);
	DeleteBitmap(IMAGE_ND_RIGHT);
	DeleteBitmap(IMAGE_ND_LEFT);
	DeleteBitmap(IMAGE_ND_TOP);
	DeleteBitmap(IMAGE_ND_BACK);
}

void CNewUINPCDialogue::ProcessOpening()
{
	m_bQuestListMode = false;
	SetContents(0);
	::PlayBuffer(SOUND_INTERFACE01);
}

bool CNewUINPCDialogue::ProcessClosing()
{
	m_dwCurDlgIndex = 0;
	m_dwContributePoint = 0;
	m_bQuestListMode = false;
	SendExitInventory();
	::PlayBuffer(SOUND_CLICK01);
	return true;
}

void CNewUINPCDialogue::SetContents(DWORD dwDlgIndex)
{
	m_dwCurDlgIndex = dwDlgIndex;
	SetCurNPCWords();
	SetCurSelTexts();
	m_bCanClick = true;
	m_btnProgressL.Lock();
	m_btnSelTextL.Lock();
	m_nSelSelText = 0;
}

void CNewUINPCDialogue::SetCurNPCWords(int nQuestListCount)
{
	::memset(m_aszNPCWords[0], 0, sizeof(char) * ND_NPC_LINE_MAX * ND_WORDS_ROW_MAX);

	g_pRenderText->SetFont(g_hFont);
	const char* pszSrc;
	if (m_bQuestListMode)
		pszSrc = 0 < nQuestListCount ? g_QuestMng.GetWords(1501) : g_QuestMng.GetWords(1502);
	else
		pszSrc = g_QuestMng.GetNPCDlgNPCWords(m_dwCurDlgIndex);

	if (pszSrc == NULL)
	{
		// 设置默认对话内容，防止空指针访问
		::strcpy(m_aszNPCWords[0], "对话数据缺失");
		m_nMaxNPCPage = 0;
		m_eLowerView = SEL_TEXTS_MODE;
		m_btnProgressR.Lock();
		m_nSelNPCPage = 0;
		return;
	}

	int nLine = ::DivideStringByPixel(&m_aszNPCWords[0][0], ND_NPC_LINE_MAX, ND_WORDS_ROW_MAX,
		pszSrc, 160);

	if (1 > nLine)
		return;

	m_nMaxNPCPage = (nLine - 1) / ND_NPC_MAX_LINE_PER_PAGE;
	if (1 <= m_nMaxNPCPage)
	{
		m_eLowerView = NON_SEL_TEXTS_MODE;
		m_btnProgressR.UnLock();
	}
	else
	{
		m_eLowerView = SEL_TEXTS_MODE;
		m_btnProgressR.Lock();
	}

	m_nSelNPCPage = 0;
}

void CNewUINPCDialogue::SetCurSelTexts()
{
	::memset(m_aszSelTexts[0], 0, sizeof(char) * ND_SEL_TEXT_LINE_MAX * ND_WORDS_ROW_MAX);
	::memset(m_anSelTextLine, 0, sizeof(int) * (ND_QUEST_INDEX_MAX_COUNT+1));

	g_pRenderText->SetFont(g_hFont);

	char szAnswer[2 * ND_WORDS_ROW_MAX];
	const char* pszAnswer;
	int nSelTextLineSum = 0;
	int i;
	for (i = 0; i < QM_MAX_ND_ANSWER; ++i) 		
	{
		::sprintf(szAnswer, "%d. ", i + 1);
		pszAnswer = g_QuestMng.GetNPCDlgAnswer(m_dwCurDlgIndex, i);
		if (NULL == pszAnswer)
		{
			// 如果没有回答选项，添加一个默认选项
			if (i == 0)
			{
				::strcat(szAnswer, "确定");
				m_anSelTextLine[i] = ::DivideStringByPixel(&m_aszSelTexts[nSelTextLineSum][0],2, ND_WORDS_ROW_MAX, szAnswer, 160, false);
				nSelTextLineSum += m_anSelTextLine[i];
				++i;
			}
			break;
		}
		::strcat(szAnswer, pszAnswer);

		m_anSelTextLine[i] = ::DivideStringByPixel(&m_aszSelTexts[nSelTextLineSum][0],2, ND_WORDS_ROW_MAX, szAnswer, 160, false);

		nSelTextLineSum += m_anSelTextLine[i];

		if (ND_SEL_TEXT_LINE_MAX <= nSelTextLineSum)
			break;
	}

	m_nSelTextCount = i;

	CalculateSelTextMaxPage(i);
}

void CNewUINPCDialogue::CalculateSelTextMaxPage(int nSelTextCount)
{
	m_nSelSelTextPage = 0;
	m_nMaxSelTextPage = 0;
	::memset(m_anSelTextLinePerPage, 0, sizeof(int)*ND_SEL_TEXT_PAGE_LIMIT);
	::memset(m_anSelTextCountPerPage, 0, sizeof(int)*ND_SEL_TEXT_PAGE_LIMIT);
	
	int i;
	for (i = 0; i < nSelTextCount; ++i)
	{
		// 检查i是否有效
		if (i < 0 || i >= ND_QUEST_INDEX_MAX_COUNT)
		{
			break;
		}
		
		// 检查m_nMaxSelTextPage是否有效
		if (m_nMaxSelTextPage >= ND_SEL_TEXT_PAGE_LIMIT)
		{
			break;
		}
		
		++m_anSelTextCountPerPage[m_nMaxSelTextPage];
		m_anSelTextLinePerPage[m_nMaxSelTextPage] += m_anSelTextLine[i];

		if (m_anSelTextLinePerPage[m_nMaxSelTextPage] > ND_SEL_TEXT_MAX_LINE_PER_PAGE)
		{
			--m_anSelTextCountPerPage[m_nMaxSelTextPage];
			m_anSelTextLinePerPage[m_nMaxSelTextPage] -= m_anSelTextLine[i];
			if (ND_SEL_TEXT_PAGE_LIMIT == ++m_nMaxSelTextPage)
				break;
			++m_anSelTextCountPerPage[m_nMaxSelTextPage];
			m_anSelTextLinePerPage[m_nMaxSelTextPage] += m_anSelTextLine[i];
		}
	}
	
	if (1 <= m_nMaxSelTextPage)
		m_btnSelTextR.UnLock();
	else
		m_btnSelTextR.Lock();
}

void CNewUINPCDialogue::SetQuestListText(DWORD* adwSrcQuestIndex, int nIndexCount)
{
	_ASSERT(0 <= nIndexCount && nIndexCount <= ND_QUEST_INDEX_MAX_COUNT);

	::memset(m_adwQuestIndex, 0, sizeof(DWORD)*ND_QUEST_INDEX_MAX_COUNT);
	::memcpy(m_adwQuestIndex, adwSrcQuestIndex, sizeof(DWORD) * nIndexCount);

	m_nSelTextCount = nIndexCount + 1;

	::memset(m_aszSelTexts[0], 0, sizeof(char) * ND_SEL_TEXT_LINE_MAX * ND_WORDS_ROW_MAX);
	::memset(m_anSelTextLine, 0, sizeof(int) * (ND_QUEST_INDEX_MAX_COUNT+1));

	char szSelText[2 * ND_WORDS_ROW_MAX];
	const char* pszSelText;
	int nSelTextRow = 0;
	g_pRenderText->SetFont(g_hFont);
	int i;
	for (i = 0; i < m_nSelTextCount; ++i)
	{
		if (m_nSelTextCount - 1 == i)
		{
			::sprintf(szSelText, "%d. ", i + 1);
			pszSelText = g_QuestMng.GetWords(1007);
		}
		else
		{
			::sprintf(szSelText, "%d. [Q]", i + 1);
			pszSelText = g_QuestMng.GetSubject(m_adwQuestIndex[i]);
		}

		if (NULL == pszSelText)
			break;
		::strcat(szSelText, pszSelText);

		m_anSelTextLine[i] = ::DivideStringByPixel(&m_aszSelTexts[nSelTextRow][0],
			2, ND_WORDS_ROW_MAX, szSelText, 160, false);
		
		nSelTextRow += m_anSelTextLine[i];
		
		if (ND_SEL_TEXT_LINE_MAX < nSelTextRow)
			break;
	}

	CalculateSelTextMaxPage(i);
}

void CNewUINPCDialogue::SetContributePoint(DWORD dwContributePoint)
{
	if (543 == g_QuestMng.GetNPCIndex() || 544 == g_QuestMng.GetNPCIndex())
		m_dwContributePoint = dwContributePoint;
}

void CNewUINPCDialogue::ProcessSelTextResult()
{
	if (m_bQuestListMode)
	{
		if (m_nSelSelText == m_nSelTextCount)
		{
			m_bQuestListMode = false;
			SetContents(0);
		}
		else
		{
			// 检查m_nSelSelText是否有效
			if (m_nSelSelText > 0 && m_nSelSelText <= ND_QUEST_INDEX_MAX_COUNT)
			{
				SendQuestSelection(m_adwQuestIndex[m_nSelSelText - 1], (BYTE)m_nSelSelText);
			}
			else
			{
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_NPC_DIALOGUE);
				return;
			}
		}
	}
	else
	{
		// 检查m_nSelSelText是否有效
		if (m_nSelSelText < 1 || m_nSelSelText > QM_MAX_ND_ANSWER)
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_NPC_DIALOGUE);
			return;
		}
		
		int nAnswerResult = g_QuestMng.GetNPCDlgAnswerResult(m_dwCurDlgIndex, m_nSelSelText-1);
		if (nAnswerResult == -1)
		{
			// 如果没有找到对应的回答结果，关闭对话窗口
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_NPC_DIALOGUE);
			return;
		}
		if (900 >= nAnswerResult)
		{
			SetContents(nAnswerResult);
		}
		else
		{
			// 检查当前NPC是否是370（再生宝石属性洗练NPC）
			if (g_QuestMng.GetNPCIndex() == 370)
			{
				// 打开再生宝石属性洗练窗口
				g_pNewUISystem->Show(SEASON3B::INTERFACE_EVENRELIFE);
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_NPC_DIALOGUE);
				PlayBuffer(SOUND_CLICK01);
				return;
			}
			
			switch (nAnswerResult)
			{
			case 901:
				SendRequestQuestByNPCEPList();
				break;

			case 902:
				SendRequestAPDPUp();
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_NPC_DIALOGUE);
				break;
			case 903:
				SendRequestGensJoining(1);
				break;

			case 904:
				SendRequestGensJoining(2);
				break;

			case 905:
				SendRequestGensSecession();
				break;
			case 906:
				SendRequestGensReward(1);
				break;
			case 907:
				SendRequestGensReward(2);
				break;

			default:
				// 避免执行SetContents(999)导致的空指针访问
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_NPC_DIALOGUE);
			}
		}
	}
}

void CNewUINPCDialogue::ProcessQuestListReceive(DWORD* adwSrcQuestIndex, int nIndexCount)
{
	m_bQuestListMode = true;
	SetCurNPCWords(nIndexCount);
	SetQuestListText(adwSrcQuestIndex, nIndexCount);
	m_bCanClick = true;
	m_btnProgressL.Lock();
	m_btnSelTextL.Lock();
	m_nSelSelText = 0;
}

enum GENS_JOINING_ERR_CODE
{	
	GJEC_NONE_ERR = 0,
	GJEC_REG_GENS_ERR,
	GJEC_GENS_SECEDE_DAY_ERR,
	GJEC_REG_GENS_LV_ERR,
	GJEC_REG_GENS_NOT_EQL_GUILDMA_ERR,
	GJEC_NONE_REG_GENS_GUILDMA_ERR,
	GJEC_PARTY,
	GJEC_GUILD_UNION_MASTER
};

void CNewUINPCDialogue::ProcessGensJoiningReceive(BYTE byResult, BYTE byInfluence)
{
	switch (byResult)
	{
	case GJEC_NONE_ERR:
		Hero->m_byGensInfluence = byInfluence;
		SetContents(5);
		break;
	case GJEC_REG_GENS_ERR:
		SetContents(9);
		break;
	case GJEC_GENS_SECEDE_DAY_ERR:
		SetContents(11);
		break;
	case GJEC_REG_GENS_LV_ERR:
		SetContents(8);
		break;
	case GJEC_REG_GENS_NOT_EQL_GUILDMA_ERR:
		SetContents(10);
		break;
	case GJEC_NONE_REG_GENS_GUILDMA_ERR:
		SetContents(12);
		break;
	case GJEC_PARTY:
		SetContents(18);
		break;
	case GJEC_GUILD_UNION_MASTER:
		SetContents(19);
		break;
	}
}

enum GENS_SECEDE_ERR_CODE
{	
	GSEC_NONE_ERR = 0,
	GSEC_IS_NOT_REG_GENS,
	GSEC_GUILD_MASTER_CAN_NOT_SECEDE,
	GSEC_IS_NOT_INFLUENCE_NPC
};

enum GENS_REWARD_ERR_CODE
{
	GENS_REWARD_CALL = 0,
	GENS_REWARD_TERM,
	GENS_REWARD_TARGET,
	GENS_REWARD_SPACE,
	GENS_REWARD_ALREADY,
	GENS_REWARD_DIFFERENT,
	GENS_REWARD_NOT_REG,
};

void CNewUINPCDialogue::ProcessGensSecessionReceive(BYTE byResult)
{
	switch (byResult)
	{
	case GSEC_NONE_ERR:
		Hero->m_byGensInfluence = 0;
		SetContents(16);
		break;
	case GSEC_IS_NOT_REG_GENS:
		SetContents(15);
		break;
	case GSEC_GUILD_MASTER_CAN_NOT_SECEDE:
		SetContents(14);
		break;
	case GSEC_IS_NOT_INFLUENCE_NPC:
		SetContents(17);
		break;
	}
}

void CNewUINPCDialogue::ProcessGensRewardReceive(BYTE byResult)
{
	switch(byResult)
	{
	case GENS_REWARD_CALL:
		SetContents(20);
		break;
	case GENS_REWARD_TERM:
		SetContents(21);
		break;
	case GENS_REWARD_TARGET:
		SetContents(22);
		break;
	case GENS_REWARD_SPACE:
		SetContents(23);
		break;
	case GENS_REWARD_ALREADY:
		SetContents(24);
		break;
	case GENS_REWARD_DIFFERENT:
		SetContents(17);
		break;
	case GENS_REWARD_NOT_REG:
		SetContents(25);
		break;
	}
}
