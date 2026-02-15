我将修改ReiDoMU.cpp文件中的GiveWinningRewards()方法，移除BUFF奖励和物品奖励的代码，只保留金币奖励。

**修改步骤**：

1. **打开ReiDoMU.cpp文件**
2. **找到GiveWinningRewards()方法**（大约在1096行）
3. **移除BUFF奖励部分**：
   - 删除1119-1125行的BUFF奖励代码
4. **移除物品奖励部分**：
   - 删除1128-1138行的物品奖励代码
5. **保留金币奖励部分**：
   - 保留1141行的金币奖励代码

**修改后的代码结构**：
```cpp
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
```

这样修改后，获胜的战盟成员将只获得金币奖励，而不会获得BUFF和物品奖励。