// ReiDoMu.cpp: Evento rei do MU by louis || www.jogandomu.com.br
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSProtocol.h"
#include "GuildClass.h"
#include "Map.h"
#include "MapServerManager.h"
#include "MemScript.h"

#include "ServerInfo.h"
#include "SkillManager.h"
#include "ScheduleManager.h"
#include "Notice.h"
#include "Util.h"
#include "CommandManager.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "Guild.h"
#include "ReiDoMU.h"
#include "ACuoi_MessLang.h"

CReiDoMU gReiDoMU;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReiDoMU::CReiDoMU()
{
	this->m_State = REI_STATE_BLANK;
	this->m_RemainTime = 0;
	this->m_TargetTime = 0;
	this->m_TickCount = GetTickCount();
	this->MinutesLeft = -1;
	this->m_WarningTime = 5;
	this->m_EventTime = 30; // Default event duration: 30 minutes

	this->GRewardValue1 = 0;
	this->GRewardValue2 = 0;
	this->GRewardValue3 = 0;
	this->CRewardValue1 = 0;
	this->CRewardValue2 = 0;
	this->CRewardValue3 = 0;

	// Throne related initialization
	this->m_ThroneMap = 2;
	this->m_ThroneX = 225;
	this->m_ThroneY = 237;
	this->m_ThroneCountdownDuration = 120;
	this->m_ThroneCountdown = 0;
	this->m_CurrentThroneOwnerIndex = -1;
	this->m_CurrentThroneGuildIndex = -1;
	this->m_IsThroneCountingDown = false;

	// Buff reward initialization
	this->m_BuffStrength = 0;
	this->m_BuffAgility = 0;
	this->m_BuffVitality = 0;
	this->m_BuffEnergy = 0;

	// Item drop reward initialization
	this->m_ItemDropRewards.clear();

	this->m_ReiDoMUStartTime.clear();

	this->Init();
}

void CReiDoMU::Init()
{
	this->Clear();

	if (gServerInfo.m_ReiDoMUEvent == 0)
	{
		this->SetState(REI_STATE_BLANK);
	}
	else
	{
		this->SetState(REI_STATE_EMPTY);
	}
}

void CReiDoMU::Load(char* path)
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_ReiDoMUStartTime.clear();
	this->m_ItemDropRewards.clear();

	this->Clear();

	try
	{
		// Read the configuration file section by section
		while (true)
		{
			// Get the next token
			eTokenResult tokenResult = lpMemScript->GetToken();

			// Check if we've reached the end of the file
			if (tokenResult == TOKEN_END)
			{
				break;
			}

			// Get the section number
			int section = lpMemScript->GetNumber();

			// Process each section
			if (section == 0)
			{
				// Section 0: Basic Event Settings
				while (true)
				{
					tokenResult = lpMemScript->GetToken();
					if (tokenResult == TOKEN_END)
					{
						break;
					}
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					this->m_WarningTime = lpMemScript->GetAsNumber();
					this->m_EventTime = lpMemScript->GetAsNumber();
				}
			}
			else if (section == 1)
			{
				// Section 1: Event Start Times
				while (true)
				{
					tokenResult = lpMemScript->GetToken();
					if (tokenResult == TOKEN_END)
					{
						break;
					}
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					REI_START_TIME info;
					info.Year = lpMemScript->GetAsNumber();
					info.Month = lpMemScript->GetAsNumber();
					info.Day = lpMemScript->GetAsNumber();
					info.DayOfWeek = lpMemScript->GetAsNumber();
					info.Hour = lpMemScript->GetAsNumber();
					info.Minute = lpMemScript->GetAsNumber();
					info.Second = lpMemScript->GetAsNumber();
					this->m_ReiDoMUStartTime.push_back(info);
				}
			}
			else if (section == 2)
			{
				// Section 2: Guild Rewards
				while (true)
				{
					tokenResult = lpMemScript->GetToken();
					if (tokenResult == TOKEN_END)
					{
						break;
					}
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					this->GRewardValue1 = lpMemScript->GetAsNumber();
					this->GRewardValue2 = lpMemScript->GetAsNumber();
					this->GRewardValue3 = lpMemScript->GetAsNumber();
				}
			}
			else if (section == 3)
			{
				// Section 3: Character Rewards
				while (true)
				{
					tokenResult = lpMemScript->GetToken();
					if (tokenResult == TOKEN_END)
					{
						break;
					}
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					this->CRewardValue1 = lpMemScript->GetAsNumber();
					this->CRewardValue2 = lpMemScript->GetAsNumber();
					this->CRewardValue3 = lpMemScript->GetAsNumber();
				}
			}
			else if (section == 4)
			{
				// Section 4: Throne Coordinates
				while (true)
				{
					tokenResult = lpMemScript->GetToken();
					if (tokenResult == TOKEN_END)
					{
						break;
					}
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					this->m_ThroneMap = lpMemScript->GetAsNumber();
					this->m_ThroneX = lpMemScript->GetAsNumber();
					this->m_ThroneY = lpMemScript->GetAsNumber();
				}
			}
			else if (section == 5)
			{
				// Section 5: Event Timings
				while (true)
				{
					tokenResult = lpMemScript->GetToken();
					if (tokenResult == TOKEN_END)
					{
						break;
					}
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					// Only load throne countdown duration, not event time (to avoid conflict with section 0)
					this->m_ThroneCountdownDuration = lpMemScript->GetAsNumber();
				}
			}
			else if (section == 6)
			{
				// Section 6: Buff Rewards
				while (true)
				{
					tokenResult = lpMemScript->GetToken();
					if (tokenResult == TOKEN_END)
					{
						break;
					}
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					this->m_BuffStrength = lpMemScript->GetAsNumber();
					this->m_BuffAgility = lpMemScript->GetAsNumber();
					this->m_BuffVitality = lpMemScript->GetAsNumber();
					this->m_BuffEnergy = lpMemScript->GetAsNumber();
				}
			}
			else if (section == 7)
			{
				// Section 7: Item Drop Rewards
				while (true)
				{
					tokenResult = lpMemScript->GetToken();
					if (tokenResult == TOKEN_END)
					{
						break;
					}
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					ITEM_DROP_REWARD item;
					item.ItemID = lpMemScript->GetAsNumber();
					item.DropRate = lpMemScript->GetAsNumber();
					item.ItemLevel = lpMemScript->GetAsNumber();
					item.ExcellentOptions = lpMemScript->GetAsNumber();
					item.LuckOption = lpMemScript->GetAsNumber();
					item.SkillOption = lpMemScript->GetAsNumber();
					item.SocketCount = lpMemScript->GetAsNumber();
					this->m_ItemDropRewards.push_back(item);
				}
			}
		}
	}
	catch (...)
	{
		// Handle any exceptions gracefully
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}


void CReiDoMU::MainProc()
{
	DWORD elapsed = GetTickCount() - this->m_TickCount;

	if (elapsed < 1000)
	{
		return;
	}

	this->m_TickCount = GetTickCount();

	this->m_RemainTime = (int)difftime(this->m_TargetTime, time(0));

	if (gServerInfo.m_ReiDoMUEvent == 0)
	{
		if (gServerDisplayer.EventKing != -1)
		{
			gServerDisplayer.EventKing = -1;
		}
	}
	else
	{
		if (this->m_State == REI_STATE_EMPTY)
		{
			gServerDisplayer.EventKing = this->m_RemainTime;
		}
		else
		{
			if (gServerDisplayer.EventKing != 0)
			{
				gServerDisplayer.EventKing = 0;
			}
		}
	}

	switch (this->m_State)
	{
	case REI_STATE_BLANK:
		this->ProcState_BLANK();
		break;
	case REI_STATE_EMPTY:
		this->ProcState_EMPTY();
		break;
	case REI_STATE_START:
		this->ProcState_START();
		break;
	}

	// Throne countdown handling
	if (this->m_IsThroneCountingDown)
	{
		// Check if throne owner is still valid
		this->CheckThrone();

		if (this->m_IsThroneCountingDown)
		{
			// Update countdown
			this->m_ThroneCountdown--;

			// Send countdown announcements every second
			if (this->m_ThroneCountdown > 0)
			{
				// Validate current throne owner
				if (OBJECT_RANGE(this->m_CurrentThroneOwnerIndex) != 0 && gObjIsConnected(this->m_CurrentThroneOwnerIndex) != 0)
				{
					LPOBJ lpObj = &gObj[this->m_CurrentThroneOwnerIndex];
					if (lpObj != NULL)
					{
						gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[Icewind Valley] %s from guild %s is holding the throne! %d seconds remaining!", lpObj->Name, lpObj->GuildName, this->m_ThroneCountdown);
					}
				}
			}

			// Check if countdown finished
			if (this->m_ThroneCountdown <= 0)
			{
				// Crown the guild as winner
				this->WinnerNumber = this->m_CurrentThroneGuildIndex;
				this->WinnerTime = this->m_EventTime * 60; // Full event time as winner time

				// Send victory announcement
				GUILD_INFO_STRUCT* lpGuildInfo = gGuildClass.SearchGuild_Number(this->WinnerNumber);
				if (lpGuildInfo != NULL)
				{
					gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[Icewind Valley] Guild %s has successfully captured the throne and won the battle!", lpGuildInfo->Name);
					LogAdd(LOG_EVENT, "[KING OF MU] Guild %s won by capturing the throne", lpGuildInfo->Name);
				}

				// Give rewards
				this->GiveWinningRewards();

				// End event
				this->m_RemainTimeWinner = 40;
				this->m_Active = 0;
				this->SetState(REI_STATE_EMPTY);
				this->StopThroneCountdown();
			}
		}
	}

	if (this->m_RemainTimeWinner > 0)
	{
		if (this->WinnerNumber >= 0)
		{

			if (this->m_RemainTimeWinner == 40)
			{
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(518));
			}

			if (this->m_RemainTimeWinner == 38)
			{
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(518));
			}

			if (this->m_RemainTimeWinner == 28)
			{
				GUILD_INFO_STRUCT* lpGuildInfo = gGuildClass.SearchGuild_Number(this->WinnerNumber);

				if (lpGuildInfo == 0)
				{

				}
				else {
					LogAdd(LOG_EVENT, "[KING OF MU] Guild Winner: %s", lpGuildInfo->Name);
					gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(519), lpGuildInfo->Name);
					gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(520), this->WinnerTime);
					gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(521));
					gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(522));

					this->GDRankingKingGuildSaveSend(lpGuildInfo->Name, this->WinnerTime);

					for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
					{
						if (gObjIsConnectedGP(n) != 0 && gObj[n].Type == OBJECT_USER)
						{
							if (strcmp(gObj[n].GuildName, lpGuildInfo->Name) == 0)
							{
								LogAdd(LOG_EVENT, "[KING OF MU] Member Reward: %s", gObj[n].Name);
								GDSetCoinSend(gObj[n].Index, this->GRewardValue1, this->GRewardValue2, this->GRewardValue3, 0, 0, "KingOfMu");
							}
						}
					}

				}

			}

			if (this->m_RemainTimeWinner == 20)
			{
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(523));
			}
			if (this->m_RemainTimeWinner == 16)
			{
				if (this->rank1 >= 0)
				{
					LPOBJ lpObj = &gObj[this->rank1];
					gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(524), 1, lpObj->Name, lpObj->GuildName);
					//Reward
					LogAdd(LOG_EVENT, "[KING OF MU] 1� %s (Guild: %s)", lpObj->Name, lpObj->GuildName);
					GDSetCoinSend(lpObj->Index, this->CRewardValue1, this->CRewardValue2, this->CRewardValue3, 0, 0, "KingOfMu");

				}
			}
			if (this->m_RemainTimeWinner == 14)
			{
				if (this->rank2 >= 0)
				{
					LPOBJ lpObj = &gObj[this->rank2];
					gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(524), 2, lpObj->Name, lpObj->GuildName);
					//Reward
					LogAdd(LOG_EVENT, "[KING OF MU] 2� %s (Guild: %s)", lpObj->Name, lpObj->GuildName);
					GDSetCoinSend(lpObj->Index, this->CRewardValue1, this->CRewardValue2, this->CRewardValue3, 0, 0, "KingOfMu");
				}
			}
			if (this->m_RemainTimeWinner == 12)
			{
				if (this->rank3 >= 0)
				{
					LPOBJ lpObj = &gObj[this->rank3];
					gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(524), 3, lpObj->Name, lpObj->GuildName);
					//Reward
					LogAdd(LOG_EVENT, "[KING OF MU] 3� %s (Guild: %s)", lpObj->Name, lpObj->GuildName);
					GDSetCoinSend(lpObj->Index, this->CRewardValue1, this->CRewardValue2, this->CRewardValue3, 0, 0, "KingOfMu");
				}
			}
			if (this->m_RemainTimeWinner == 10)
			{
				if (this->rank4 >= 0)
				{
					LPOBJ lpObj = &gObj[this->rank4];
					gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(524), 4, lpObj->Name, lpObj->GuildName);
					//Reward
					LogAdd(LOG_EVENT, "[KING OF MU] 4� %s (Guild: %s)", lpObj->Name, lpObj->GuildName);
					GDSetCoinSend(lpObj->Index, this->CRewardValue1, this->CRewardValue2, this->CRewardValue3, 0, 0, "KingOfMu");
				}
			}
			if (this->m_RemainTimeWinner == 8)
			{
				if (this->rank5 >= 0)
				{
					LPOBJ lpObj = &gObj[this->rank5];
					gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(524), 5, lpObj->Name, lpObj->GuildName);
					//Reward
					LogAdd(LOG_EVENT, "[KING OF MU] 5� %s (Guild: %s)", lpObj->Name, lpObj->GuildName);
					GDSetCoinSend(lpObj->Index, this->CRewardValue1, this->CRewardValue2, this->CRewardValue3, 0, 0, "KingOfMu");
				}
			}

			if (this->m_RemainTimeWinner == 2)
			{
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(525));
			}

			this->m_RemainTimeWinner--;

			if (this->m_RemainTimeWinner <= 0)
			{
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(301));
				LogAdd(LOG_EVENT, "[KING OF MU] Finish");

				this->Clear();
			}
		}
		else
		{
			LogAdd(LOG_EVENT, "[KING OF MU] Finish - No Winner");
			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(526));
			this->m_RemainTimeWinner = 0;
			this->Clear();
		}


	}

}

void CReiDoMU::ProcState_BLANK()
{

}

void CReiDoMU::ProcState_EMPTY()
{

	if (this->m_RemainTime > 0 && this->m_RemainTime <= (this->m_WarningTime * 60))
	{
		int minutes = this->m_RemainTime / 60;

		if ((this->m_RemainTime % 60) == 0)
		{
			minutes--;
		}

		if (this->MinutesLeft != minutes)
		{
			this->MinutesLeft = minutes;

			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(302), (MinutesLeft + 1));
		}
	}

	if (this->m_RemainTime > 0 && this->m_RemainTime <= 5)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(304), m_RemainTime);
	}

	if (this->m_RemainTime <= 0)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(500));

		this->SetState(REI_STATE_START);
	}
}

void CReiDoMU::ProcState_START()
{
	if (this->m_RemainTime > 0 && this->m_RemainTime <= 300)
	{
		int minutes = this->m_RemainTime / 60;

		if ((this->m_RemainTime % 60) == 0)
		{
			minutes--;
			this->AddTime();
		}

		if (this->MinutesLeft != minutes)
		{
			this->MinutesLeft = minutes;

			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(311), (MinutesLeft + 1));
		}
	}

	if (this->m_RemainTime <= 0)
	{
		this->CheckChar();
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessageNew.GetMessage(301));
		this->CalcUserRank();
		this->SetGuildTop();
		this->m_RemainTimeWinner = 40;
		this->m_Active = 0;
		this->SetState(REI_STATE_EMPTY);

	}
}

void CReiDoMU::SetState(int state)
{
	this->m_State = state;

	switch (this->m_State)
	{
	case REI_STATE_BLANK:
		this->SetState_BLANK();
		break;
	case REI_STATE_EMPTY:
		this->SetState_EMPTY();
		break;
	case REI_STATE_START:
		this->SetState_START();
		break;
	}
}

void CReiDoMU::SetState_BLANK()
{

}

void CReiDoMU::SetState_EMPTY()
{
	this->CheckSync();
}

void CReiDoMU::SetState_START()
{
	this->m_Active = 1;

	// Set event duration (in seconds)
	this->m_RemainTime = this->m_EventTime * 60;

	// Calculate target time
	this->m_TargetTime = (int)(time(0) + this->m_RemainTime);

	// Set map to non-outlaw (PK without red name)
	// Note: This assumes map 2 is Icewind Valley
	// We'll modify the attack logic instead to allow PK without red name during event

	// Add all online players to the event
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) != 0 && gObj[n].Type == OBJECT_USER)
		{
			this->SetChar(n);
			
			// Add to guild if they have one
			if (gObj[n].GuildNumber > 0)
			{
				this->SetGuild(gObj[n].GuildNumber);
			}
		}
	}

	LogAdd(LOG_EVENT, "[KING OF MU] Start - Duration: %d minutes", this->m_EventTime);

	// Send start announcement
	gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[Icewind Valley] Battle event has started! Fight for the throne!");

}

void CReiDoMU::CheckSync()
{

	if (this->m_ReiDoMUStartTime.empty() != 0)
	{
		this->SetState(REI_STATE_BLANK);
		return;
	}

	CTime ScheduleTime;

	CScheduleManager ScheduleManager;

	for (std::vector<REI_START_TIME>::iterator it = this->m_ReiDoMUStartTime.begin(); it != this->m_ReiDoMUStartTime.end(); it++)
	{
		ScheduleManager.AddSchedule(it->Year, it->Month, it->Day, it->Hour, it->Minute, it->Second, it->DayOfWeek);
	}

	if (ScheduleManager.GetSchedule(&ScheduleTime) == 0)
	{
		this->SetState(REI_STATE_BLANK);
		return;
	}

	this->m_RemainTime = (int)difftime(ScheduleTime.GetTime(), time(0));

	this->m_TargetTime = (int)ScheduleTime.GetTime();

}

void CReiDoMU::Clear()
{
	for (int n = 0; n < MAX_REI_GUILD; n++)
	{
		this->Guild[n].Reset();
	}

	for (int n = 0; n < MAX_REI_CHAR; n++)
	{
		this->Char[n].Reset();
	}

	this->m_RemainTimeWinner = 0;

	this->WinnerNumber = -1;
	this->WinnerTime = 0;
	this->rank1 = -1;
	this->rank2 = -1;
	this->rank3 = -1;
	this->rank4 = -1;
	this->rank5 = -1;

	// Clear throne related variables
	this->m_ThroneCountdown = 0;
	this->m_CurrentThroneOwnerIndex = -1;
	this->m_CurrentThroneGuildIndex = -1;
	this->m_IsThroneCountingDown = false;
}

void CReiDoMU::CommandReiDoMU(LPOBJ lpObj, char* arg)
{

	GUILD_INFO_STRUCT* lpGuildInfo = lpObj->Guild;

	if (gServerInfo.m_ReiDoMUEvent == 0) //Verifica se o evento est� ativo
	{
		return;
	}

	if (lpGuildInfo == NULL)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessageNew.GetMessage(306));
		return;
	}

	if (lpObj->GuildNumber == 0) //Verifica se o personagem faz parte de uma guild
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessageNew.GetMessage(306));
		return;
	}

	if (lpObj->Map != this->m_ThroneMap || lpObj->X != this->m_ThroneX || lpObj->Y != this->m_ThroneY) //Verifica se o personagem est� na coordenada correta do trono
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessageNew.GetMessage(308));
		return;
	}

	// Verifica se o personagem � o l�der da guild
	if (lpObj->GuildStatus != 0x80) // 0x80 = Guild Master
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Only guild masters can sit on the throne!");
		return;
	}

	// Automatically start the event if it's not already active
	if (this->m_Active == 0)
	{
		// Start the event
		this->SetState(REI_STATE_START);
		LogAdd(LOG_EVENT, "[KING OF MU] Event automatically started by guild master %s (Guild: %s)", lpObj->Name, lpObj->GuildName);
	}

	// Inicia a contagem regressiva do trono
	this->StartThroneCountdown(lpObj->GuildNumber, lpObj->Index);

	this->SetGuild(lpObj->GuildNumber);

	this->SetChar(lpObj->Index);

	gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[Icewind Valley] %s has sat on the throne! Countdown started!", lpObj->GuildName);

	memcpy(this->m_GuildOwner, lpObj->GuildName, sizeof(this->m_GuildOwner));

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "You have sat on the throne! Hold your position until the countdown ends!");

	LogAdd(LOG_EVENT, "[KING OF MU] Throne occupied by %s (Guild: %s)", lpObj->Name, lpObj->GuildName);
}

void CReiDoMU::SetGuild(int gIndex)
{

	if (this->GetGuild(gIndex) == 0)
	{
		this->AddGuild(gIndex);
	}

	for (int n = 0; n < MAX_REI_GUILD; n++)
	{
		if (Guild[n].Number == gIndex)
		{
			this->Guild[n].Owner = 1;
		}
		else
		{
			this->Guild[n].Owner = 0;
		}
	}
}

void CReiDoMU::AddGuild(int gIndex)
{
	for (int n = 0; n < MAX_REI_GUILD; n++)
	{
		if (this->Guild[n].Number >= 0)
		{
			continue;
		}

		this->Guild[n].Number = gIndex;
		this->Guild[n].Owner = 0;
		this->Guild[n].Time = 0;
		return;
	}
}

bool CReiDoMU::GetGuild(int gIndex)
{
	for (int n = 0; n < MAX_REI_GUILD; n++)
	{
		if (this->Guild[n].Number == gIndex)
		{
			return 1;
		}
	}
	return 0;
}

void CReiDoMU::SetGuildTop()
{
	for (int n = 0; n < MAX_REI_GUILD; n++)
	{
		if (this->Guild[n].Time > this->WinnerTime)
		{
			this->WinnerNumber = this->Guild[n].Number;
			this->WinnerTime = this->Guild[n].Time;
		}
	}
}

void CReiDoMU::AddTime()
{
	for (int n = 0; n < MAX_REI_GUILD; n++)
	{
		if (this->Guild[n].Owner == 1)
		{
			this->Guild[n].Time++;
		}
	}
}

void CReiDoMU::SetChar(int cIndex)
{

	if (this->GetChar(cIndex) == 0)
	{
		this->AddChar(cIndex);
	}

	for (int n = 0; n < MAX_REI_CHAR; n++)
	{
		if (Char[n].Index == cIndex)
		{
			this->Char[n].Times++;
		}
	}
}

void CReiDoMU::AddChar(int cIndex)
{
	for (int n = 0; n < MAX_REI_CHAR; n++)
	{
		if (this->Char[n].Index >= 0)
		{
			continue;
		}

		this->Char[n].Index = cIndex;
		memcpy(this->Char[n].Name, gObj[cIndex].Name, sizeof(gObj[cIndex].Name));
		this->Char[n].Times = 0;
		this->Char[n].Rank = 0;
		return;
	}
}

bool CReiDoMU::GetChar(int cIndex)
{
	for (int n = 0; n < MAX_REI_CHAR; n++)
	{
		if (this->Char[n].Index == cIndex)
		{
			return 1;
		}
	}
	return 0;
}

void CReiDoMU::CheckChar()
{
	for (int n = 0; n < MAX_REI_CHAR; n++)
	{
		if (OBJECT_RANGE(this->Char[n].Index) == 0)
		{
			continue;
		}
		if (gObjIsConnected(this->Char[n].Index) == 0)
		{
			this->Char[n].Reset();
			continue;
		}
		//if (gObj[this->Char[n].Index].Map != gServerInfo.m_ReiDoMUMap)
		//{
		//	this->Char[n].Reset();
		//	continue;
		//}
		if (strcmp(this->Char[n].Name, gObj[this->Char[n].Index].Name) != 0)
		{
			this->Char[n].Reset();
			continue;
		}
	}
	return;
}

void CReiDoMU::CalcUserRank()
{
	for (int n = 0; n < MAX_REI_CHAR; n++)
	{
		if (this->Char[n].Index < 0)
		{
			continue;
		}

		int rank = MAX_REI_CHAR;

		for (int i = 0; i < MAX_REI_CHAR; i++)
		{
			if (this->Char[i].Index < 0)
			{
				rank--;
				continue;
			}

			if (this->Char[n].Index == this->Char[i].Index)
			{
				rank--;
				continue;
			}

			if (this->Char[n].Times > this->Char[i].Times)
			{
				rank--;
				continue;
			}

			if (this->Char[n].Times == this->Char[i].Times && n < i)
			{
				rank--;
				continue;
			}
		}

		this->Char[n].Rank = rank;
	}

	for (int n = 0; n < MAX_REI_CHAR; n++)
	{
		if (this->Char[n].Index >= 0)
		{
			this->Char[n].Rank++;

			if (this->Char[n].Rank == 1)
			{
				this->rank1 = this->Char[n].Index;
				GDRankingKingPlayerSaveSend(this->Char[n].Index, this->Char[n].Times);
			}
			if (this->Char[n].Rank == 2)
			{
				this->rank2 = this->Char[n].Index;
				GDRankingKingPlayerSaveSend(this->Char[n].Index, this->Char[n].Times);
			}
			if (this->Char[n].Rank == 3)
			{
				this->rank3 = this->Char[n].Index;
				GDRankingKingPlayerSaveSend(this->Char[n].Index, this->Char[n].Times);
			}
			if (this->Char[n].Rank == 4)
			{
				this->rank4 = this->Char[n].Index;
				GDRankingKingPlayerSaveSend(this->Char[n].Index, this->Char[n].Times);
			}
			if (this->Char[n].Rank == 5)
			{
				this->rank5 = this->Char[n].Index;
				GDRankingKingPlayerSaveSend(this->Char[n].Index, this->Char[n].Times);
			}
		}
	}
}

void CReiDoMU::GDRankingKingGuildSaveSend(char* name, DWORD Score)
{
	SDHP_RANKING_KING_GUILD_SAVE_SEND pMsg;

	pMsg.header.set(0x74, sizeof(pMsg));

	memcpy(pMsg.name, name, sizeof(pMsg.name));

	pMsg.score = Score;

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void CReiDoMU::GDRankingKingPlayerSaveSend(int aIndex, DWORD Score)
{

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	SDHP_RANKING_KING_PLAYER_SAVE_SEND pMsg;

	pMsg.header.set(0x75, sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	pMsg.score = Score;

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void CReiDoMU::StartKing()
{
	// Start event immediately
	this->Clear();
	this->SetState(REI_STATE_START);
	
	// Log event start
	LogAdd(LOG_EVENT, "[KING OF MU] Event started via StartKing method");
	
	// Send announcement
	gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[Icewind Valley] Battle event has started! Fight for the throne!");
}

void CReiDoMU::CheckThrone()
{
	if (!this->m_IsThroneCountingDown || this->m_CurrentThroneOwnerIndex < 0)
	{
		return;
	}

	// Check if the current throne owner is still at the throne position
	if (OBJECT_RANGE(this->m_CurrentThroneOwnerIndex) == 0 || gObjIsConnected(this->m_CurrentThroneOwnerIndex) == 0)
	{
		// Owner disconnected
		this->StopThroneCountdown();
		this->m_RemainTimeWinner = 40;
		this->m_Active = 0;
		this->SetState(REI_STATE_EMPTY);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[Icewind Valley] The guild master has disconnected! Event reset!");
		return;
	}

	LPOBJ lpObj = &gObj[this->m_CurrentThroneOwnerIndex];

	if (lpObj->Map != this->m_ThroneMap || lpObj->X != this->m_ThroneX || lpObj->Y != this->m_ThroneY)
	{
		// Owner moved away from throne
		this->StopThroneCountdown();
		this->m_RemainTimeWinner = 40;
		this->m_Active = 0;
		this->SetState(REI_STATE_EMPTY);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[Icewind Valley] The guild master has left the throne! Event reset!");
		return;
	}

	// Check if the owner is still the guild master
	GUILD_INFO_STRUCT* lpGuildInfo = lpObj->Guild;
	if (lpGuildInfo == NULL || lpObj->GuildStatus != 0x80)
	{
		// Owner is no longer guild master
		this->StopThroneCountdown();
		this->m_RemainTimeWinner = 40;
		this->m_Active = 0;
		this->SetState(REI_STATE_EMPTY);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[Icewind Valley] The guild master has lost their position! Event reset!");
		return;
	}
}

void CReiDoMU::StartThroneCountdown(int guildIndex, int charIndex)
{
	// Validate character index
	if (OBJECT_RANGE(charIndex) == 0 || gObjIsConnected(charIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[charIndex];
	if (lpObj == NULL)
	{
		return;
	}

	this->m_CurrentThroneGuildIndex = guildIndex;
	this->m_CurrentThroneOwnerIndex = charIndex;
	this->m_ThroneCountdown = this->m_ThroneCountdownDuration;
	this->m_IsThroneCountingDown = true;

	// Send announcement
	gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[Icewind Valley] %s from guild %s has sat on the throne! %d seconds countdown started!", lpObj->Name, lpObj->GuildName, this->m_ThroneCountdown);
	
	LogAdd(LOG_EVENT, "[KING OF MU] Throne countdown started by %s (Guild: %s)", lpObj->Name, lpObj->GuildName);
}

void CReiDoMU::StopThroneCountdown()
{
	this->m_CurrentThroneGuildIndex = -1;
	this->m_CurrentThroneOwnerIndex = -1;
	this->m_ThroneCountdown = 0;
	this->m_IsThroneCountingDown = false;
	
	// Log countdown stopped
	LogAdd(LOG_EVENT, "[KING OF MU] Throne countdown stopped");
}

void CReiDoMU::GiveWinningRewards()
{
	if (this->WinnerNumber < 0)
	{
		return;
	}

	GUILD_INFO_STRUCT* lpGuildInfo = gGuildClass.SearchGuild_Number(this->WinnerNumber);
	if (lpGuildInfo == NULL)
	{
		return;
	}

	// Give rewards to all guild members
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) != 0 && gObj[n].Type == OBJECT_USER)
		{
			if (strcmp(gObj[n].GuildName, lpGuildInfo->Name) == 0)
			{
				LPOBJ lpObj = &gObj[n];
				
				// Give coin rewards
				GDSetCoinSend(lpObj->Index, this->GRewardValue1, this->GRewardValue2, this->GRewardValue3, 0, 0, "KingOfMu");
			}
		}
	}

	LogAdd(LOG_EVENT, "[KING OF MU] Rewards given to guild: %s", lpGuildInfo->Name);
}


