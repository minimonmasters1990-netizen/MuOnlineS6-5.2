#include "stdafx.h"
#include "CGMProtect.h"

CGMProtect::CGMProtect()
{
}

CGMProtect::~CGMProtect()
{
	this->Release();
}

CGMProtect* CGMProtect::Instance()
{
	static CGMProtect sInstance;
	return &sInstance;
}

void CGMProtect::Release()
{
	CustomImagen.clear();
}
