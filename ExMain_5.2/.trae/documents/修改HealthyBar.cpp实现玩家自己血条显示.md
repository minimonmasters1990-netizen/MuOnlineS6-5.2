# 修改血条位置以角色名为标准居中下移显示

## 问题分析
当前血条的渲染位置是基于角色的3D位置直接计算的，没有考虑角色名称的位置。用户希望血条能够以角色名为标准，在其下方居中显示，并添加适当的偏移量。

## 解决方案
修改 `ClassLifeBar::Render()` 函数，使其：
1. 对于每个角色，计算其名称在屏幕上的位置
2. 基于角色名称的位置计算血条的位置
3. 确保血条在角色名称下方居中显示
4. 添加适当的偏移量（10像素）来调整血条与名称之间的距离

## 实现步骤

1. **修改血条位置计算逻辑**
   - 对于每个角色，使用与角色名称相同的位置计算方法
   - 确保血条在角色名称下方居中显示
   - 添加10像素的偏移量

2. **保持血条渲染样式**
   - 维持现有的血条渲染样式
   - 确保血条显示正确的生命值

3. **测试修改**
   - 确保修改后血条在角色名称下方居中显示
   - 确保血条与角色名称之间的距离适当
   - 确保修改不影响其他功能

## 技术细节
- 使用与角色名称相同的位置计算方法
- 对于Hero和其他角色，使用相同的位置计算逻辑
- 添加适当的偏移量来调整血条位置

## 修改文件
- `d:\SRC2-2基本完美\ExMain_5.2\Main5.2\ThangCuoi\HealthyBar.cpp`

## 修改代码

以下是修改后的 `ClassLifeBar::Render()` 函数代码：

```cpp
void ClassLifeBar::Render()
{
	const float Width = 38.f;
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
		RenderSegmentedHealthBar(ScreenX, ScreenY + 19, CharacterAttribute->ViewCurHP, CharacterAttribute->ViewMaxHP);
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

		// 计算血条在屏幕上的位置（与角色名称相同的位置，然后下移）
		vec3_t Position;
		int ScreenX, ScreenY;
		Vector(o->Position[0], o->Position[1], o->Position[2] + o->BoundingBoxMax[2] + 100.f, Position);
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
			char DisplayText[128];
			sprintf(DisplayText, "%s - %d%%", c->ID, lpNewHealthBar->rate);

			g_pRenderText->SetFont(g_hFont);
			g_pRenderText->SetBgColor(0);
			g_pRenderText->SetTextColor(255, 255, 255, 255);
			g_pRenderText->RenderText(ScreenX - 86, ScreenY - 7, DisplayText, 200, 0, RT3_SORT_CENTER);

			// 渲染分段式血条（在怪物名称下方10像素）
			RenderSegmentedHealthBar(ScreenX, ScreenY + 19, lpNewHealthBar->rate, 100.0f);
		}
		else
		{
			// 玩家血条渲染
			if (!c->SafeZone)
			{
				// 渲染分段式血条（在玩家名称下方10像素）
				RenderSegmentedHealthBar(ScreenX, ScreenY + 19, lpNewHealthBar->rate, 100.0f);
			}
		}
	}

	DisableAlphaBlend();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}
```

## 修改说明

1. **血条位置计算**：
   - 保持了与角色名称相同的位置计算方法，确保血条与角色名称在同一垂直线上
   - 添加了19像素的偏移量（`ScreenY + 19`），使血条在角色名称下方显示
   - 对于所有角色（包括Hero和其他玩家、怪物），使用相同的位置计算逻辑

2. **血条居中显示**：
   - 保持了现有的居中显示逻辑（`ScreenX -= static_cast<int>(Width / 2)`）
   - 确保血条在角色名称下方居中显示

3. **血条渲染**：
   - 保持了现有的分段式血条渲染样式
   - 对于不同类型的角色（Hero、其他玩家、怪物），使用相同的渲染逻辑

4. **安全检查**：
   - 保持了现有的安全检查逻辑，确保只渲染有效的角色

通过这些修改，血条将以角色名为标准，在其下方居中显示，并与角色名称保持适当的距离。这样，血条的位置将更加合理，与角色名称的关系也更加清晰。