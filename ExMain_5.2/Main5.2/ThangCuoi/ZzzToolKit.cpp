#include "Stdafx.h"                     // VC++标准预编译头文件
#include "ZzzToolKit.h"                 // 当前工具类的头文件，声明类和成员函数
#include "UIControls.h"                 // UI控件基类，所有按钮/面板的基础定义
#include "StructSendGs.h"               // 网络发包结构体头文件【核心】，客户端→服务端的指令包定义
#include "wsclientinline.h"             // 网络客户端核心，封装了发包/收包底层函数
#include "MapManager.h"                 // 地图管理器，获取当前地图ID/地图信息
#include <d3d9.h>                       // D3D9图形库，用于颜色定义/字体渲染
#include "WideData.h"                   // 分辨率自适应全局配置，适配不同屏幕尺寸
#include "NewUIInventoryCtrl.h"         // 新背包UI控件，物品格子渲染依赖

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <windows.h>                    // Windows系统API，窗口/鼠标/字体/弹窗等功能依赖
#include "..\\..\\Util\\CCRC32.H"        // CRC32校验工具，文件防篡改核心
#include <string>

SPK_ToolKitMain IsToolKit;              // 【核心】全局唯一实例，所有功能调用入口

// ====================== 模块1：窗口标题定制【游戏窗口顶部标题栏】 ======================
// 功能：实时刷新游戏窗口标题，显示角色信息/帧率/鼠标位置/地图ID，分编辑模式和普通模式
void SPK_ToolKitMain::WindowRename()
{
	char WindowName[500];                 // 窗口标题缓存字符串
	#ifdef ENABLE_EDIT                   // 宏：是否开启编辑模式（GM/开发者模式）
	if (SceneFlag == 5)                  // SceneFlag=5 → 角色进入游戏主场景
	{
		// 编辑模式标题：窗口名+角色名+等级+转生/最大转生+帧率+鼠标坐标+鼠标左键状态+地图ID
		sprintf_s(WindowName, sizeof(WindowName), "%s - %s || Level: %d || Reset: %d/%d || FPS : %.1f || Mouse: %d %d [%d] - MapID: %d [World%d]"
			, gProtect->m_MainInfo.WindowName  // 游戏窗口基础名称
			, Hero->ID                        // 当前角色名称
			, CharacterAttribute->Level       // 当前角色等级
			, CharacterAttribute->ViewReset   // 当前转次数
			, CharacterAttribute->ViewMaxReset// 最大转次数
			, FPS_AVG                         // 平均帧率
			, MouseX, MouseY, MouseLButtonPush// 鼠标XY坐标+左键是否按下(1=按下,0=松开)
			, gMapManager.WorldActive         // 当前地图ID
			, gMapManager.WorldActive + 1     // 地图序号（地图ID+1，视觉友好）
		);
		SetWindowText(g_hWnd, WindowName);    // 设置Windows窗口标题
	}
	else
	{
		// 非主场景标题：简化信息，只显示窗口名+帧率+鼠标+地图ID
		sprintf_s(WindowName, sizeof(WindowName), "%s || FPS : %.1f || Mouse: %d %d [%d] - MapID: %d [World%d]"
			, gProtect->m_MainInfo.WindowName
			, FPS_AVG
			, MouseX, MouseY, MouseLButtonPush
			, gMapManager.WorldActive
			, gMapManager.WorldActive + 1
		);
		SetWindowText(g_hWnd, WindowName);
	}
	#else                                // 普通玩家模式（默认编译分支，无编辑模式）
	if (SceneFlag == 5)
	{
		// 中文定制版标题：窗口名+角色名+等级+转生+帧率+所在地图
		sprintf_s(WindowName, sizeof(WindowName), "%s || 角色名 : %s || 等级: %d || 转生: %d/%d || 帧率 : %.f"
			, gProtect->m_MainInfo.WindowName
			, Hero->ID
			, CharacterAttribute->Level
			, CharacterAttribute->ViewReset
			, CharacterAttribute->ViewMaxReset
			, FPS_AVG
			, gMapManager.WorldActive
			, gMapManager.WorldActive + 1
		);
		SetWindowText(g_hWnd, WindowName);
	}
	else
	{
		// 非主场景简化标题
		sprintf_s(WindowName, sizeof(WindowName), "%s || 帧率 : %.f"
			, gProtect->m_MainInfo.WindowName
			, FPS_AVG
			, gMapManager.WorldActive
			, gMapManager.WorldActive + 1
		);
		SetWindowText(g_hWnd, WindowName);
	}
	#endif
}

// ====================== 模块2：配置文件读取【UI位置读取】 ======================
// 功能：从Config.ini的[ACuoi]节点读取指定索引的UI控件 坐标X/Y/宽度W/高度H
// 入参：index → UI控件的编号（比如背包格子1、按钮2、面板3）
// 返回值：POINT_F结构体 → 包含x,y,w,h四个浮点值，用于UI控件的绘制定位
POINT_F SPK_ToolKitMain::VisiblePos(int index)
{
	char buf[32];
	char keyX[32];
	char keyY[32];
	char keyW[32];
	char keyH[32];

	// 拼接配置文件的键名：BODY_X0/BODY_Y0/BODY_W0/BODY_H0 对应编号0的UI控件
	sprintf_s(keyX, "BODY_X%d", index);
	sprintf_s(keyY, "BODY_Y%d", index);
	sprintf_s(keyW, "BODY_W%d", index);
	sprintf_s(keyH, "BODY_H%d", index);

	// 从Config.ini读取配置值，默认值0.0，读取后转浮点型
	GetPrivateProfileStringA("ACuoi", keyX, "0.0", buf, sizeof(buf), "./Config.ini");
	float x = (float)atof(buf);

	GetPrivateProfileStringA("ACuoi", keyY, "0.0", buf, sizeof(buf), "./Config.ini");
	float y = (float)atof(buf);

	GetPrivateProfileStringA("ACuoi", keyW, "0.0", buf, sizeof(buf), "./Config.ini");
	float w = (float)atof(buf);

	GetPrivateProfileStringA("ACuoi", keyH, "0.0", buf, sizeof(buf), "./Config.ini");
	float h = (float)atof(buf);

	return { x, y, w, h };
}

// ====================== 模块2：配置文件读取【UI颜色读取】 ======================
// 功能：从Config.ini的[ACuoi]节点读取指定索引的UI控件 颜色值(RGBA)
// 入参：index → UI控件编号
// 返回值：D3DCOLOR → D3D标准颜色格式，用于绘制带透明度的UI控件
D3DCOLOR SPK_ToolKitMain::VisibleCor(int index)
{
	char buf[128];
	char keyColor[32];

	// 拼接颜色键名：COLOR0/COLOR1/COLOR2...
	sprintf_s(keyColor, "COLOR%d", index);
	// 默认颜色：纯白色(255,255,255,255)，格式：R,G,B,A
	GetPrivateProfileStringA("ACuoi", keyColor, "255, 255, 255, 255", buf, sizeof(buf), "./Config.ini");

	int r, g, b, a;
	// 解析配置文件的颜色值，按逗号分隔
	sscanf_s(buf, "%d, %d, %d, %d", &r, &g, &b, &a);

	// 注意：D3DCOLOR_RGBA的参数顺序是 B,G,R,A 而非 R,G,B,A，是D3D的固定规则
	return D3DCOLOR_RGBA(b, g, r, a);
}

// ====================== 模块3：分辨率适配【屏幕宽度适配】 ======================
// 功能：根据当前客户端选择的分辨率编号，返回对应的屏幕宽度适配值
// 作用：解决不同分辨率下UI控件错位、拉伸的问题，是多分辨率适配的核心函数
int SPK_ToolKitMain::GetPositionScreen()
{
	int a = 0;
	switch (m_Resolution)  // m_Resolution：全局分辨率编号（0~10对应不同分辨率，如800*600/1024*768/1920*1080）
	{
		case 0: a = 854; break;
		case 1: a = 854; break;
		case 2: a = 854; break;
		case 3: a = 854; break;
		case 4: a = 854; break;
		case 5: a = 854; break;
		case 6: a = 854; break;
		case 7: a = 854; break;
		case 8: a = 915; break;  // 适配1280*720
		case 9: a = 995; break;  // 适配1366*768
		case 10: a = 1218; break;// 适配1920*1080
		default:a = 640; break;  // 默认最低分辨率640
	}
	return a;
}

// ====================== 模块3：分辨率适配【固定高度值】 ======================
// 功能：返回固定的创建UI控件的高度值，所有UI面板的基础高度基准
int SPK_ToolKitMain::GetCreatePosHeight()
{
	return 430;
}

// ====================== 模块4：职业名称多语言转换【核心定制】 ======================
#define ClassLang 0   // 开关宏：0=越南语（默认），1=英语，改这个值即可切换职业名语言
// 功能：根据角色职业编号，返回对应的职业名称字符串
// 入参：a → 职业编号（奇迹MU的职业编号是固定的，0=法魔，16=剑士，32=精灵等）
// 返回值：职业名称字符串（越南语/英语二选一）
char* CharacterCode(int a)
{
	switch (a)
	{
		#if ClassLang // 英语分支
		case 0:  return "Dark Wizard";     // 法魔
		case 1:  return "Soul Master";     // 魔导
		case 2:  return "Grand Master";    // 法神
		case 16: return "Dark Knight";     // 剑士
		case 17: return "Blade Knight";    // 骑士
		case 18: return "Blade Master";    // 剑圣
		case 32: return "Fairy Elf";       // 精灵
		case 33: return "Muse Elf";        // 圣精灵
		case 34: return "High Elf";        // 精灵王
		case 48: return "Magic Gladiator"; // 魔剑
		case 50: return "Duel Master";     // 剑魔
		case 64: return "Dark Lord";       // 圣导
		case 66: return "Lord Emperor";    // 圣帝
		case 80: return "Summoner";        // 召唤
		case 81: return "Bloody Summoner"; // 血召
		case 82: return "Dimension Master";// 召神
		case 96: return "Rage Fighter";    // 格斗
		case 98: return "Fist Master";     // 斗神
		#else // 越南语分支（默认，代码原版）
		case 0:  return "Phu Thuy";
		case 1:  return "Phap Su";
		case 2:  return "Thien Su";
		case 16: return "Chien Binh";
		case 17: return "Ky Su";
		case 18: return "Thien Kiem";
		case 32: return "Tien Nu";
		case 33: return "Thanh Nu";
		case 34: return "Thien Nu";
		case 48: return "Dau Su";
		case 50: return "Thien Tong";
		case 64: return "Chua Te";
		case 66: return "Thien Vuong";
		case 80: return "Thuat Su";
		case 81: return "Thuat Su";
		case 82: return "Phuc Ma";
		case 96: return "Thiet Binh";
		case 98: return "Quyen Vuong";
		#endif
	}
	return "unknown"; // 未知职业
}

// ====================== 模块5：字体初始化配置【全局字体核心】 ======================
// 功能：从Config.ini的[FontConfig]节点读取字体配置，初始化全局字体句柄
// 作用：所有游戏内的文本渲染（UI文字、提示框、背包物品名）都用这个字体，支持配置文件修改字体，无需改代码
void SPK_ToolKitMain::WindowFont()
{
	char FontFace[100];        // 字体名称（如Tahoma、微软雅黑、宋体）
	char FontWeightStr[10];    // 字体粗细（0=细体，1=粗体）
	char FontHeightStr[50];    // 字体高度
	char FontAliasStr[50];     // 字体抗锯齿级别

	// 从Config.ini读取字体配置，默认值：Tahoma字体、粗体、13号字、抗锯齿3级
	GetPrivateProfileStringA("FontConfig", "FontName", "Tahoma", FontFace, sizeof(FontFace), "./Config.ini");
	GetPrivateProfileStringA("FontConfig", "FontWeight", "1", FontWeightStr, sizeof(FontWeightStr), "./Config.ini");
	GetPrivateProfileStringA("FontConfig", "FontHeight", "13", FontHeightStr, sizeof(FontHeightStr), "./Config.ini");
	GetPrivateProfileStringA("FontConfig", "FontAlias", "3", FontAliasStr, sizeof(FontAliasStr), "./Config.ini");

	// 转换配置值为整型
	int FontWeight = atoi(FontWeightStr);
	this->FontHeightRead = atoi(FontHeightStr);
	int FontAliasRead = atoi(FontAliasStr);
	// 字体粗细映射：0=细体(100)，1=粗体(700)，Windows字体API的固定值
	int FontW = (FontWeight == 0) ? 100 : 700;

	// 根据分辨率适配字体高度，低分辨率用小字体，高分辨率用配置文件的字体高度
	switch (m_Resolution)
	{
		case 0: FontHeight = 11; FontW = 100; break;
		case 1: FontHeight = 11; FontW = 100; break;
		case 2: FontHeight = 12; FontW = 100; break;
		case 3: FontHeight = this->FontHeightRead; break;
		case 4: FontHeight = this->FontHeightRead; break;
		case 5: FontHeight = this->FontHeightRead; break;
		case 6: FontHeight = this->FontHeightRead; break;
		case 7: FontHeight = this->FontHeightRead; break;
		case 8: FontHeight = this->FontHeightRead; break;
		case 9: FontHeight = this->FontHeightRead; break;
		case 10: FontHeight = this->FontHeightRead; break;
	}
	// 创建全局字体句柄：普通字体 + 大号字体（使用GB2312_CHARSET支持中文ANSI）
	g_hFont = CreateFont(FontHeight, 0, 0, 0, FontW, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, FontAliasRead, DEFAULT_PITCH | FF_DONTCARE, FontFace);
	g_hFontBig = CreateFont(20, 0, 0, 0, FontW, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, FontAliasRead, DEFAULT_PITCH | FF_DONTCARE, FontFace);

	// 输出调试日志到控制台：分辨率、字体高度、屏幕尺寸、分辨率缩放比例
	g_ConsoleDebug->Write(MCD_NORMAL, "Resolution: %d / FontHeight:%d", m_Resolution, FontHeight);
	g_ConsoleDebug->Write(MCD_NORMAL, "Screen: %dx%d", WindowWidth, WindowHeight);
	g_ConsoleDebug->Write(MCD_NORMAL, "WideRate X:%.2f / WideRate Y:%.2f", gPosWide.x_fScreenRate_x, gPosWide.x_fScreenRate_y);
}

// ====================== 模块6：数字格式化工具【千分位分隔】 ======================
// 功能：将超大数字（金币/战力/属性值）格式化为带千分位逗号的字符串，比如 1000000 → 1,000,000
// 入参：Number → 要格式化的数字（SPK是自定义的整型别名，等价于DWORD）
// 返回值：格式化后的字符串，视觉友好，奇迹MU所有数字显示都用这个函数
char* SPK_ToolKitMain::QN(SPK Number)
{
	if (Number == 0)
	{
		return strdup("0"); // 数字为0，直接返回"0"
	}

	char OutPut[40];
	char Temp[40];

	int Index = 0;
	int CommaCounter = 0;

	// 反转数字，拼接千分位逗号
	while (Number > 0)
	{
		Temp[Index++] = '0' + (int)(Number % 10); // 取最后一位数字
		Number /= 10;                             // 去掉最后一位

		// 每3位加一个逗号，最后一位不加
		if (++CommaCounter == 3 && Number > 0)
		{
			Temp[Index++] = ',';
			CommaCounter = 0;
		}
	}

	// 反转回来，得到正确的格式
	int OutPutIndex = 0;
	while (Index > 0)
	{
		OutPut[OutPutIndex++] = Temp[--Index];
	}
	OutPut[OutPutIndex] = '\0'; // 字符串结束符

	return strdup(OutPut); // 分配内存并返回字符串
}

// ====================== 模块7：核心文本渲染【普通字体】 ======================
// 功能：绘制普通大小的文本，支持换行、自定义颜色/背景色、对齐方式、格式化文本（如%d/%s）
// 入参：PosX/PosY=坐标，color=文字颜色，bkcolor=背景色，Width/Height=宽高，Align=对齐方式，Text=文本内容+格式化参数
// 核心：游戏内99%的文本都是用这个函数绘制（背包、技能、提示框、UI文字）
void SPK_ToolKitMain::ThisFont(int PosX, int PosY, DWORD color, DWORD bkcolor, int Width, int Height, BYTE Align, LPCTSTR Text, ...)
{
	char Buff[2048];
	int BuffLen = sizeof(Buff) - 1;
	ZeroMemory(Buff, BuffLen);

	va_list argList;
	va_start(argList, Text);
	int Len = vsprintf_s(Buff, BuffLen, Text, argList);
	va_end(argList);

	if (Len <= 0) return;

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetBgColor(
		(bkcolor >> 24) & 0xFF,
		(bkcolor >> 16) & 0xFF,
		(bkcolor >> 8) & 0xFF,
		bkcolor & 0xFF
	);

	char* Context = NULL;
	char* Line = strtok_s(Buff, "\n", &Context);

	while (Line != NULL)
	{
		g_pRenderText->SetTextColor(0, 0, 0, 128);
		g_pRenderText->RenderText(PosX + 2, PosY + 2, Line, Width, Height, Align);
		
		g_pRenderText->SetTextColor(
			(color >> 24) & 0xFF,
			(color >> 16) & 0xFF,
			(color >> 8) & 0xFF,
			color & 0xFF
		);
		g_pRenderText->RenderText(PosX, PosY, Line, Width, Height, Align);
		
		PosY += Height + 2;
		Line = strtok_s(NULL, "\n", &Context);
	}
}

// ====================== 模块7：核心文本渲染【大号字体】 ======================
// 功能：和ThisFont完全一致，唯一区别是使用大号字体g_hFontBig
// 适用场景：标题、重要提示、大按钮文字、角色名等需要醒目显示的文本
void SPK_ToolKitMain::ThisFontBig(int PosX, int PosY, DWORD color, DWORD bkcolor, int Width, int Height, BYTE Align, LPCTSTR Text, ...)
{
	char Buff[2048];
	int BuffLen = sizeof(Buff) - 1;
	ZeroMemory(Buff, BuffLen);

	va_list argList;
	va_start(argList, Text);
	int Len = vsprintf_s(Buff, BuffLen, Text, argList);
	va_end(argList);

	if (Len <= 0) return;

	g_pRenderText->SetFont(g_hFontBig);
	g_pRenderText->SetBgColor(
		(bkcolor >> 24) & 0xFF,
		(bkcolor >> 16) & 0xFF,
		(bkcolor >> 8) & 0xFF,
		bkcolor & 0xFF
	);

	char* Context = NULL;
	char* Line = strtok_s(Buff, "\n", &Context);

	while (Line != NULL)
	{
		g_pRenderText->SetTextColor(0, 0, 0, 128);
		g_pRenderText->RenderText(PosX + 2, PosY + 2, Line, Width, Height, Align);
		
		g_pRenderText->SetTextColor(
			(color >> 24) & 0xFF,
			(color >> 16) & 0xFF,
			(color >> 8) & 0xFF,
			color & 0xFF
		);
		g_pRenderText->RenderText(PosX, PosY, Line, Width, Height, Align);
		
		PosY += Height + 2;
		Line = strtok_s(NULL, "\n", &Context);
	}
	g_pRenderText->SetFont(g_hFont);
}

// ====================== 模块8：鼠标交互【鼠标碰撞检测】 ======================
// 功能：检测鼠标是否在指定的矩形区域内（X,Y到X+h,Y+w）
// 入参：x,y=矩形左上角坐标，h=宽，w=高
// 返回值：bool → true=鼠标在区域内，false=不在
// 核心作用：所有UI按钮/面板的点击、悬浮判定都用这个函数（比如点击背包格子、技能按钮）
bool SPK_ToolKitMain::IsWorkZone(float x, float y, float h, float w)
{
	return MouseX >= x && MouseX <= x + h && MouseY >= y && MouseY <= y + w;
}

// ====================== 模块9：网络发包【通用指令发包】 ======================
// 功能：封装通用的网络指令包，发送到服务端
// 入参：Folder=主指令号，Data=子指令号 → 奇迹MU的网络指令是双字节标识（主+子）
// 核心：客户端→服务端的指令都用这个函数发送，比如切换地图、使用物品、释放技能
void SPK_ToolKitMain::KeySendState(BYTE Folder, BYTE Data)
{
	PMSG_TICKET_SEND pMsg{};
	pMsg.header.set(Folder, Data, sizeof(pMsg)); // 设置指令头：主/子指令+包大小
	DataSend((BYTE*)&pMsg, pMsg.header.size);   // 发送数据包到服务端
}

// ====================== 模块9：网络发包【交易类型切换】 ======================
// 功能：发送交易类型切换指令到服务端（如开启/关闭交易、摆摊）
void SPK_ToolKitMain::KeySendStateStore(int Type)
{
	SendOffTradeType pMsg{};
	pMsg.header.set(0xF3, 0xEB, sizeof(pMsg)); // 固定指令号 F3 EB
	pMsg.Type = Type;                           // 交易类型：0=关闭，1=开启
	DataSend((BYTE*)&pMsg, pMsg.header.size);
}

// ====================== 模块9：网络发包【充值相关指令】 ======================
// 以下4个函数是越南服定制的【充值/点数】相关发包，分别是：充值请求、查看充值、打开充值面板
void SPK_ToolKitMain::KeySendStateMocNap(DWORD c)
{
	SendRequestAction pMsg{};
	pMsg.header.set(0xD3, 0x9A, sizeof(pMsg));
	pMsg.Action = g_dataclient.DanhSachMocNap[c].IndexMocNap;
	DataSend((BYTE*)&pMsg, pMsg.header.size);
}
void SPK_ToolKitMain::KeySendStateViewMocNap(DWORD c)
{
	IsToolKit.ViewIndex = c;
	SendRequestAction pMsg{};
	pMsg.header.set(0xD3, 0x9B, sizeof(pMsg));
	pMsg.Action = g_dataclient.DanhSachMocNap[c].IndexMocNap;
	DataSend((BYTE*)&pMsg, pMsg.header.size);
}
void SPK_ToolKitMain::KeySendStateOpenMocNap(DWORD c)
{
	SendRequestAction pMsg{};
	pMsg.header.set(0xD3, 0x9C, sizeof(pMsg));
	pMsg.Action = 1;
	DataSend((LPBYTE)&pMsg, pMsg.header.size);
}

// ====================== 模块10：角色职业校验 ======================
// 功能：校验当前角色是否为指定的职业类型，返回1=是，0=否
// 作用：用于职业专属功能的判定（比如法师专属技能、剑士专属装备）
int SPK_ToolKitMain::CheckClass()
{
	return (Hero->Class == 0 || Hero->Class == 1 || Hero->Class == 2 ||
		Hero->Class == 3 || Hero->Class == 4 || Hero->Class == 5 ||
		Hero->Class == 6 || Hero->Class == 8 || Hero->Class == 9 ||
		Hero->Class == 10 || Hero->Class == 13);
}

// ====================== 模块11：3D物品渲染【核心】 ======================
// 功能：在指定坐标绘制3D物品模型（如背包里的装备、地上的物品）
// 入参：x,y=坐标，a/b=物品模型编号
// 核心：奇迹MU的物品是3D模型，这个函数是3D物品的绘制入口，所有物品显示都依赖它
void SPK_ToolKitMain::RenderItemKit(int x, int y, int a, int b)
{
	EndBitmap(); // 关闭2D位图渲染，切换到3D模式

	// OpenGL矩阵切换：投影矩阵+模型视图矩阵，3D渲染的固定流程
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport2(0, 0, WindowWidth, WindowHeight);
	gluPerspective2(1.f, (float)(WindowWidth) / (float)(WindowHeight), RENDER_ITEMVIEW_NEAR, RENDER_ITEMVIEW_FAR);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	GetOpenGLMatrix(CameraMatrix);
	EnableDepthTest();  // 开启深度检测，3D模型不穿透
	EnableDepthMask();

	// 绘制3D物品模型
	RenderItem3D(x, y, 20, 20, a, b, 0, 0, false);

	UpdateMousePositionn(); // 更新鼠标位置

	// 恢复矩阵，切换回2D模式
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	BeginBitmap(); // 开启2D位图渲染
}

// ====================== 模块12：UI渲染【带文本的悬浮提示框】 ======================
// 功能：绘制带边框、背景、文本的悬浮提示框（如鼠标悬浮在装备上的属性提示、按钮提示）
// 支持显示整数/浮点数值，是游戏内所有提示框的核心绘制函数
void SPK_ToolKitMain::RenderToolTipExt(float x, float y, float w, float h, int tHi, const char *text, int Enable, int Val, float fVal)
{
	EnableAlphaTest(); // 开启透明测试，支持半透明背景

	// 绘制黑色边框：上、左、右、下四条线
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	RenderColor(x - 1, y - 1, w + 1, 1);         
	RenderColor(x - 1, y - 1, 1, h + 1);         
	RenderColor(x - 1 + w + 1, y - 1, 1, h + 1); 
	RenderColor(x - 1, y - 1 + h + 1, w + 2, 1); 

	// 绘制半透明黑色背景（0.8透明度）
	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	RenderColor(x, y, w, h);

	// 绘制提示文本
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	g_pRenderText->SetBgColor(50, 0, 0, 255);
	g_pRenderText->SetTextColor(255, 255, 255, 255);
	g_pRenderText->RenderText(x, y - 1, text, w, tHi, 3);

	// 显示数值：Enable=1显示整数，Enable=2显示浮点型（保留2位小数）
	if (Enable == 1)
	{
		char szMessage[128];
		sprintf(szMessage, "%d", Val);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->SetTextColor(255, 255, 255, 255);
		g_pRenderText->RenderText(x, y + h * 0.5f, szMessage, w, tHi, 3);
	}
	else if (Enable == 2)
	{
		char szMessage[128];
		sprintf(szMessage, "%.2f", fVal);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->SetTextColor(255, 255, 255, 255);
		g_pRenderText->RenderText(x, y + h * 0.5f, szMessage, w, tHi, 3);
	}
}

// ====================== 模块12：UI渲染【纯空白提示框】 ======================
// 功能：绘制只有边框和背景的空白提示框，无文本，用于自定义内容填充
void SPK_ToolKitMain::RenderToolTip(float x, float y, float w, float h)
{
	EnableAlphaTest();
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	RenderColor(x - 1, y - 1, w + 1, 1);
	RenderColor(x - 1, y - 1, 1, h + 1);
	RenderColor(x - 1 + w + 1, y - 1, 1, h + 1);
	RenderColor(x - 1, y - 1 + h + 1, w + 2, 1);
	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	RenderColor(x, y, w, h);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
}

// ====================== 模块13：UI渲染【物品格子面板】 ======================
// 功能：绘制网格状的物品格子（背包、仓库、商城的格子面板）
// 入参：X/Y=起始坐标，Doc=行数，Ngang=列数，Use=是否可用
// 核心：所有格子类UI的绘制入口，比如背包是6行10列，仓库是8行10列
void SPK_ToolKitMain::RenderItemSlot(int X, int Y, int Doc, int Ngang, int Use)
{
	float v6;
	float v7;
	GLfloat red;
	GLfloat green;
	int j;
	int i;

	// 双层循环绘制行列格子
	for (i = 0; i < Doc; ++i)
	{
		for (j = 0; j < Ngang; ++j)
		{
			EnableAlphaTest();
			glEnable(GL_ALPHA_TEST);
			glColor4f(0.3, 0.3, 0.3, 0.60000002); // 格子半透明背景色
			v7 = (float)(Y + 20 * i);
			v6 = (float)(X + 20 * j);
			RenderColor(LODWORD(v6), LODWORD(v7), 20.0, 20.0, 0.0, 0);
			glEnable(GL_TEXTURE_2D);
			glColor3f(1.0, 1.0, 1.0);
			green = (float)(Y + 20 * i);
			red = (float)(X + 20 * j);
			// 绘制格子边框纹理（新背包UI的格子样式）
			SEASON3B::RenderImage(BITMAP_INTERFACE_NEW_INVENTORY_BASE_BEGIN, LODWORD(red), LODWORD(green), 21.0, 21.0);
			glDisable(GL_BLEND);		
			glEnable(GL_ALPHA_TEST);
			DisableAlphaBlend();
			glColor3f(1.f, 1.f, 1.f);
		}
	}
}

// ====================== 模块14：公益服核心定制【自动发币按钮】【重中之重】 ======================
// 全局变量：按钮状态+冷却时间
DWORD lastToggleTime = 0;
bool isHoverState = true;
// 功能：绘制【自动发币按钮】+ 点击逻辑，公益服核心功能！点击按钮自动发送金币到角色身上
// 入参：ID=按钮纹理ID，Time=冷却时间(秒)，x/y/w/h=按钮坐标和大小
// 核心指令：/wc 角色名 货币类型 数量 → 奇迹MU公益服的金币发送指令，服务端专属解析
void SPK_ToolKitMain::CreateRenderButton(int ID, int Time, float x, float y, float w, float h)
{
	static DWORD lastSendTime = 0;
	char szCmd[64];

	int MaxCoinSend = 1000000000; // 每次发送的金币数量：10亿【可修改这个值调发币量】
	// 校验当前金币是否小于发送额度，防止溢出
	if (pGetCoin.ThisCoin[0] <= MaxCoinSend / 100)
	{
		// 鼠标悬浮在按钮上
		if (SEASON3B::CheckMouseIn((int)x, (int)y, (int)w, (int)h))
		{
			// 绘制按钮高亮纹理
			RenderBitmap(ID, x, y, w, h, 0.f, (64.f / 128.f) * 1, 1.0, 64.f / 128.f, 1, 1, 0);

			// 鼠标左键点击按钮
			if (SEASON3B::IsPress(VK_LBUTTON))
			{
				DWORD currentTime = GetTickCount();
				// 冷却时间判定：防止连续点击刷屏
				if (currentTime - lastSendTime < (Time * 1000))
				{
					g_pChatListBox->AddText("", "Wait please...", SEASON3B::TYPE_SYSTEM_MESSAGE);
					return;
				}
				lastSendTime = currentTime;

				// 发送5种货币类型的金币指令（ThisCoin[0-4]对应不同货币：金币、钻石、积分等）
				if (pGetCoin.ThisCoin[0] <= MaxCoinSend) sprintf_s(szCmd, "/wc %s %d %d", Hero->ID, 1, MaxCoinSend); SendChat(szCmd);
				if (pGetCoin.ThisCoin[1] <= MaxCoinSend) sprintf_s(szCmd, "/wc %s %d %d", Hero->ID, 2, MaxCoinSend); SendChat(szCmd);
				if (pGetCoin.ThisCoin[2] <= MaxCoinSend) sprintf_s(szCmd, "/wc %s %d %d", Hero->ID, 3, MaxCoinSend); SendChat(szCmd);
				if (pGetCoin.ThisCoin[3] <= MaxCoinSend) sprintf_s(szCmd, "/wc %s %d %d", Hero->ID, 4, MaxCoinSend); SendChat(szCmd);
				if (pGetCoin.ThisCoin[4] <= MaxCoinSend) sprintf_s(szCmd, "/wc %s %d %d", Hero->ID, 5, MaxCoinSend); SendChat(szCmd);

				return;
			}
		}
		else
		{
			// 鼠标不在按钮上，按钮呼吸灯效果：500ms切换一次纹理状态，视觉友好
			DWORD currentTime = GetTickCount();
			if (currentTime - lastToggleTime >= 500)
			{
				isHoverState = !isHoverState;
				lastToggleTime = currentTime;
			}
			if (isHoverState)
			{
				RenderBitmap(ID, x, y, w, h, 0.f, (64.f / 128.f) * 1, 1.0, 64.f / 128.f, 1, 1, 0);
			}
			else
			{
				RenderBitmap(ID, x, y, w, h, 0.f, (64.f / 128.f) * 0, 1.0, 64.f / 128.f, 1, 1, 0);
			}
		}
	}
}

// ====================== 模块15：安全校验【文件防篡改 CRC32校验】 ======================
// 功能：计算字符串的CRC32校验值，用于文件完整性校验
unsigned int CRC32(const char* data, size_t length)
{
	unsigned int crc = 0xFFFFFFFF;
	unsigned int table[256];

	for (unsigned int i = 0; i < 256; ++i)
	{
		unsigned int crc_value = i;
		for (unsigned int j = 8; j > 0; --j)
		{
			if (crc_value & 1)
				crc_value = (crc_value >> 1) ^ 0xEDB88320;
			else
				crc_value >>= 1;
		}
		table[i] = crc_value;
	}

	for (size_t i = 0; i < length; ++i)
	{
		unsigned char byte = data[i];
		crc = (crc >> 8) ^ table[(crc & 0xFF) ^ byte];
	}

	return ~crc;
}

// 功能：校验指定文件的CRC32值是否和预期一致，防止文件被篡改（如修改客户端、开挂）
bool CheckFileCRC(const std::string& filename, unsigned int expectedCRC)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file) return false;

	std::string fileData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	if (fileData.empty()) return false;

	unsigned int calculatedCRC = CRC32(fileData.c_str(), fileData.size());
	return calculatedCRC == expectedCRC;
}

// ====================== 模块15：安全校验【异或加密/解密】 ======================
// 功能：对字符串进行简单的异或加解密，密钥固定为key，用于加密提示信息，防止被轻易破解
std::string xorEncryptDecrypt(const std::string& data, char key) {
	std::string output = data;
	for (size_t i = 0; i < output.size(); ++i) {
		output[i] ^= key; 
	}
	return output;
}

// ====================== 模块15：安全校验【核心防篡改入口】 ======================
// 功能：客户端启动时的核心校验函数，校验Player.bmd文件的CRC32值
// 作用：如果文件被篡改（如修改角色数据、开挂），则弹出加密提示框并强制退出游戏
int SPK_ToolKitMain::BmdSPK()
{
	std::string filePath = "\x44\x61\x74\x61\x2F\x50\x6C\x61\x79\x65\x72\x2F\x50\x6C\x61\x79\x65\x72\x2E\x62\x6D\x64"; // 加密的文件路径：Data/Player/Player.bmd
	unsigned int expectedCRC = 0x7D56FCEB; // 预期的CRC值

	if (!CheckFileCRC(filePath, expectedCRC))
	{
		std::string encodedMessage = "\x18\x52\x5D\x71\x52\x75\x66\x55\x68\x56\x3A\x2C\x66\x57\x51\x1A\x47\x6A\x52\x50\x6E\x2C\x69\x5D"; // 加密的提示信息
		std::string decodedMessage = xorEncryptDecrypt(encodedMessage, 0xAA); // 密钥0xAA解密

		MessageBox(NULL, decodedMessage.c_str(), "ACuoi", MB_OK | MB_ICONERROR); // 弹出错误提示
		ExitProcess(1); // 强制退出游戏
	}

	return 0;
}

// ====================== 模块16：数值修正工具函数 ======================
// 功能：修正伤害值，防止数值溢出（奇迹MU的伤害值有上限255）
WORD AdjustedDamage(WORD value)
{
	return (value > 255) ? (255 + (value % 256)) : value;
}
// 功能：安全获取物品索引，防止索引越界导致崩溃
int SafeGetItem(int index)
{
	return CHECK_ITEM(index);
}
// 功能：校验密码字符是否合法（只允许ASCII可见字符）
bool IsValidPasswordChar(const char* str)
{
	for (int i = 0; str[i] != '\0'; ++i)
	{
		unsigned char c = (unsigned char)str[i];
		if (c < 0x20 || c > 0x7E) return false;
	}
	return true;
}

// ====================== 模块17：特效渲染【炫酷混合特效】 ======================
// 功能：绘制随机的闪光/粒子特效，用于按钮高亮、装备强化、技能释放等炫酷视觉效果
void SPK_ToolKitMain::RenderMixEffect(float x, float y, int w, int h)
{
	EnableAlphaBlend();

	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			float fx = x + j * 20 + (rand() % 20);
			float fy = y + i * 20 + (rand() % 20);

			glColor3f((float)(rand() % 6 + 6) * 0.1f, (float)(rand() % 4 + 4) * 0.1f, 0.2f);
			float Rotate = (float)((int)(WorldTime) % 100) * 20.f;
			float Scale = 5.f + (rand() % 10);

			// 绘制多种闪光纹理，叠加旋转和缩放，形成炫酷特效
			RenderBitmapRotate(BITMAP_SHINY, fx, fy, Scale, Scale, 0);
			RenderBitmapRotate(BITMAP_SHINY, fx, fy, Scale, Scale, Rotate);
			RenderBitmapRotate(BITMAP_SHINY + 1, fx, fy, Scale * 3.f, Scale * 3.f, Rotate);
			RenderBitmapRotate(BITMAP_LIGHT, fx, fy, Scale * 6.f, Scale * 6.f, 0);
		}
	}

	DisableAlphaBlend();
}

// ====================== 模块18：怪物过滤【指定怪物编号校验】 ======================
// 功能：校验怪物编号是否为指定的怪物类型，用于怪物过滤、挂机、BOSS提示等
bool SPK_ToolKitMain::GetIndexMonster(int index)
{
	switch (index)
	{
		case 750: return true;
		case 751: return true;
		case 752: return true;
		case 753: return true;
		case 754: return true;
		case 755: return true;
		case 756: return true;
	}
	return false;
}

// ====================== 模块19：金币颜色值【根据金币数量变色】 ======================
// 功能：根据金币数量返回对应的颜色值，数量越多颜色越鲜艳（如100万以下白色，100万以上黄色，700万以上红色）
DWORD GetGoldValue(DWORD Gold)
{
	if (Gold <= 99999) return 0xFFFFFFFF;        // 白色
	else if (Gold <= 999999) return 0x1DCC2EFF;  // 浅黄色
	else if (Gold < 7000000) return 0xFF6F00FF;  // 金黄色
	else return 0xFF0000FF;                      // 红色
}

// ====================== 模块20：武器过滤【指定武器类型校验】 ======================
// 功能：校验武器类型是否为指定的类别，用于武器过滤、职业专属武器判定
bool IsFilteredWeapon(int WeaponType)
{
	if (WeaponType >= MODEL_SWORD && WeaponType <= MODEL_SHIELD + 512) return true;
	return false;
}

// ====================== 模块21：异或加密【双字节异或】 ======================
// 功能：对字符串进行双字节异或加密，用于更高级的文本加密
std::string EncryptXOR(const std::string& input)
{
	std::string output = input;
	for (size_t i = 0; i + 1 < output.size(); i += 2)
	{
		output[i] ^= 0xA;
		output[i + 1] ^= 0xF;
	}
	return output;
}