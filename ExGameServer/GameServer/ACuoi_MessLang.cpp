#include "stdafx.h"
#include "ACuoi_MessLang.h"
#include "CommandManager.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "ItemManager.h"

#include "Notice.h"
#include "Util.h"

cMessageNew gMessageNew;

void cMessageNew::LoadConfig(char* FilePath)
{
    pugi::xml_document file;
    // 确保XML解析器正确处理UTF-8
    pugi::xml_parse_result res = file.load_file(FilePath, pugi::parse_default, pugi::encoding_utf8);
    if (res.status != pugi::status_ok)
    {
        ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
        return;
    }
    pugi::xml_node IsBankConfig = file.child("ThangCuoi");

    pugi::xml_node Message = IsBankConfig.child("Message");
    for (pugi::xml_node msg = Message.child("Msg"); msg; msg = msg.next_sibling())
    {
        MESSAGE_NEW info;
        info.Index = msg.attribute("Index").as_int();
        // 确保strcpy_s正确复制UTF-8字符串
        strcpy_s(info.Message, msg.attribute("Text").as_string());
        this->m_MessageInfoBP.insert(std::pair<int, MESSAGE_NEW>(info.Index, info));
    }
}
char* cMessageNew::GetMessage(int index)
{
	std::map<int, MESSAGE_NEW>::iterator it = this->m_MessageInfoBP.find(index);
	if (it == this->m_MessageInfoBP.end())
	{
		char Error[256];
		wsprintf(Error, "Could not find message %d!", index);
		return Error;
	}
	else
	{
		return it->second.Message;
	}
}
