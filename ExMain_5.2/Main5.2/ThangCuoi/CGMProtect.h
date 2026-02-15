#pragma once

class CGMProtect
{
public:
	CGMProtect();
	virtual~CGMProtect();
	static CGMProtect* Instance();

	void Release();

protected:

public:
	//------------------------------------
};

#define GMProtect				(CGMProtect::Instance())
