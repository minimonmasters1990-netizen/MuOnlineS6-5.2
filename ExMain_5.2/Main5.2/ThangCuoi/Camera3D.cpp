#include "stdafx.h"
#include "Camera3D.h"
#include "ZzzLodTerrain.h"
#include "NewUISystem.h"
#include "MapManager.h"
CCamera gCamera;

CCamera::CCamera()
{
	LastPress = GetTickCount();
	m_CursorX = MouseX;
	m_CursorY = MouseY;
	CameraZoom = 0.f;
	AngleY3D = 0.f;
	AngleZ3D = 0.f;
}

CCamera::~CCamera() = default;

void CCamera::Toggle()
{
	m_CameraOnOff ^= 1;
}

void CCamera::Restore()
{
	CameraZoom = 0;
	CameraAngle[2] = -50.f;
	AngleY3D = 0.f;
	AngleZ3D = 0.f;
}

void CCamera::Update()
{
	if (SceneFlag != MAIN_SCENE)
		return;

	if (HIBYTE(GetAsyncKeyState(VK_F10)) == 128)
	{
		auto now = GetTickCount();
		if ((now - LastPress) > 2000)
		{
			LastPress = now;
			Toggle();
		}
		return;
	}
	//ACuoi Cấm Map F10 + F11
	//if ((gMapManager.WorldActive == 0))
	//{
	//	return;
	//}

	if (!m_CameraOnOff)
		return;

	if (HIBYTE(GetAsyncKeyState(VK_F11)) == 128)
	{
		Restore();
		return;
	}
	// Xoay bằng chuột giữa
	if (HIBYTE(GetAsyncKeyState(VK_MBUTTON)) == 128)
	{
		// Tính hệ số靈敏度 dựa trên CameraZoom
		// Khi zoom gần (CameraZoom nhỏ), giảm靈敏度 để giữ camera ổn định
		float sensitivity = 1.0f - (CameraZoom / -40.0f) * 0.7f;
		if (sensitivity < 0.3f) sensitivity = 0.3f;
		if (sensitivity > 1.0f) sensitivity = 1.0f;

		// Xử lý di chuyển ngang (chỉ ảnh hưởng đến góc水平)
		if (m_CursorX < MouseX) CameraAngle[2] += 1.2 * sensitivity;
		if (m_CursorX > MouseX) CameraAngle[2] -= 1.2 * sensitivity;

		// Xử lý di chuyển dọc với ngưỡng để tránh nhiễu
		const int MOUSE_VERTICAL_THRESHOLD = 2;
		if (abs(m_CursorY - MouseY) > MOUSE_VERTICAL_THRESHOLD)
		{
			if (m_CursorY < MouseY && AngleY3D < 20) // mở rộng ngẩng lên
			{
				AngleY3D += 1.3f;
				AngleZ3D += 15.f;
			}
			else if (m_CursorY > MouseY && AngleY3D > -45) // mở rộng cúi xuống
			{
				AngleY3D -= 1.3f;
				AngleZ3D -= 24.f;
			}
		}
	}
	// Xoay bằng phím mũi tên ngang
	{
		// Tính hệ số靈敏度 dựa trên CameraZoom
		float sensitivity = 1.0f - (CameraZoom / -40.0f) * 0.7f;
		if (sensitivity < 0.3f) sensitivity = 0.3f;
		if (sensitivity > 1.0f) sensitivity = 1.0f;

		if (HIBYTE(GetAsyncKeyState(VK_RIGHT)) == 128) CameraAngle[2] -= 1.5 * sensitivity;
		if (HIBYTE(GetAsyncKeyState(VK_LEFT)) == 128) CameraAngle[2] += 1.5 * sensitivity;
	}

	// Ngẩng/cúi bằng phím mũi tên dọc
	if (HIBYTE(GetAsyncKeyState(VK_UP)) == 128 && AngleY3D < 20)
	{
		AngleY3D += 2.0f;
		AngleZ3D += 30.f;
	}
	if (HIBYTE(GetAsyncKeyState(VK_DOWN)) == 128 && AngleY3D > -30)
	{
		AngleY3D -= 2.0f;
		AngleZ3D -= 30.f;
	}
	
	if (MouseWheel != 0)
	{
		if (MouseWheel > 0 && CameraZoom > -40) // zoom gần
		{
			CameraZoom -= 3;
			if (AngleY3D > -40)
			{
				AngleY3D -= 1.5f;
				AngleZ3D -= 30.f;
			}
		}
		else if (MouseWheel < 0 && CameraZoom < 12)  // zoom xa
		{
			CameraZoom += 3;
			if (AngleY3D < 20)
			{
				AngleY3D += 1.5f;
				AngleZ3D += 30.f;
			}
		}
		MouseWheel = 0;
	}
	
	// Giới hạn范围AngleZ3D để tránh camera vào dưới đất
	const float MIN_ANGLE_Z3D = -1000.0f;
	const float MAX_ANGLE_Z3D = 500.0f;
	if (AngleZ3D < MIN_ANGLE_Z3D) AngleZ3D = MIN_ANGLE_Z3D;
	if (AngleZ3D > MAX_ANGLE_Z3D) AngleZ3D = MAX_ANGLE_Z3D;

	m_CursorX = MouseX;
	m_CursorY = MouseY;
}
