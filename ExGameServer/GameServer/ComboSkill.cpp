#include "stdafx.h"
#include "ComboSkill.h"
#include "CustomCombo.h"
#include "ServerInfo.h"
#include "SkillManager.h"


void CComboSkill::Init()
{
	this->m_time = 0;
	this->m_skill[0] = 0xFFFF;
	this->m_skill[1] = 0xFFFF;
	this->m_index = -1;
}

int CComboSkill::GetSkillType(WORD skill)
{
	// Check custom combo definitions first
	if (gCustomCombo.CheckSkillPrimary(skill) == 1)
	{
		return 0;
	}
	else if (gCustomCombo.CheckSkillSecundary(skill) == 1)
	{
		return 1;
	}

	// Fallback to default definitions if not found in custom (and if default switch is off, maybe we should still support them or just rely on custom)
	// But based on user request "all classes custom combo", we should prioritize custom checks.
	// If the user wants to use default skills, they should add them to CustomCombo.txt or we keep the hardcoded check as fallback.
	
	if (gServerInfo.m_EnableComboToAllSwitch == 1)
	{
		// Already checked custom above. If not found, check hardcoded defaults? 
		// Or maybe the user implies that *only* custom defined skills should work?
		// Let's keep the hardcoded ones as a fallback for standard Knight combo skills if they aren't in custom list.
		
		if(skill == SKILL_FALLING_SLASH || skill == SKILL_LUNGE || skill == SKILL_UPPERCUT || skill == SKILL_CYCLONE || skill == SKILL_SLASH)
		{
			return 0;
		}
		else if(skill == SKILL_TWISTING_SLASH || skill == SKILL_RAGEFUL_BLOW || skill == SKILL_DEATH_STAB || skill == SKILL_FROZEN_STAB || skill == SKILL_BLOOD_STORM)
		{
			return 1;
		}
	}
	else
	{
		// Original logic for BK class
		if(skill == SKILL_FALLING_SLASH || skill == SKILL_LUNGE || skill == SKILL_UPPERCUT || skill == SKILL_CYCLONE || skill == SKILL_SLASH)
		{
			return 0;
		}
		else if(skill == SKILL_TWISTING_SLASH || skill == SKILL_RAGEFUL_BLOW || skill == SKILL_DEATH_STAB || skill == SKILL_FROZEN_STAB || skill == SKILL_BLOOD_STORM)
		{
			return 1;
		}
	}

	return -1;
}

bool CComboSkill::CheckCombo(WORD skill)
{
	int type = this->GetSkillType(skill);

	if (skill == SKILL_SWORD_SLASH && ((rand() % 100) < gServerInfo.m_TyLeRaComboMG))
	{
		return 1;
	}

	if(type == -1)
	{
		this->Init();
		return 0;
	}

	if(type == 0)
	{
		this->m_time = GetTickCount()+3000;
		this->m_skill[0] = skill;
		this->m_index = 0;
		return 0;
	}

	if(type == 1)
	{
		if(this->m_time < GetTickCount())
		{
			this->Init();
			return 0;
		}

		if(this->m_skill[0] == 0xFFFF)
		{
			this->Init();
			return 0;
		}

		if(this->m_index == 0)
		{
			this->Init();
			return 1;
		}
	}

	this->Init();
	return 0;
}
