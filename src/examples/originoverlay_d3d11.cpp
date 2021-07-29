#include <Windows.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../Renderer.h"
#include "../InputHandler.h"
#include "../Originoverlay.h"

struct bouncyball
{
	int radius;
	ImVec2 pos;
	ImVec2 velocity;

	bouncyball()
	{
		radius = rand() % 32;
		pos.x = rand() % 200;
		pos.y = rand() % 100;
		velocity.x = rand() % 10;
		velocity.y = rand() % 10;
	}
};

const int bouncyBallCount = 200;
bouncyball bouncyballs[bouncyBallCount];

/**
*	Draw anything using Renderer and IMGUI
*/
void OnDraw(imogui::Renderer& renderer)
{
	const int width = renderer.GetWidth();
	const int height = renderer.GetHeight();

	static ImU32 bouncyballColor = 0xc000ffaa;
	static float color[3] = { 0.3f, 0, 0.9f };

	for (int i = 0; i < bouncyBallCount; i++)
	{
		bouncyballs[i].pos.x += bouncyballs[i].velocity.x;
		bouncyballs[i].pos.y += bouncyballs[i].velocity.y;

		if (bouncyballs[i].pos.x - bouncyballs[i].radius <= 0)
		{
			bouncyballs[i].velocity.x = abs(bouncyballs[i].velocity.x);
		}
		if (bouncyballs[i].pos.x + bouncyballs[i].radius >= width)
		{
			bouncyballs[i].velocity.x = abs(bouncyballs[i].velocity.x) * -1;
		}
		if (bouncyballs[i].pos.y - bouncyballs[i].radius <= 0)
		{
			bouncyballs[i].velocity.y = abs(bouncyballs[i].velocity.y);
		}
		if (bouncyballs[i].pos.y + bouncyballs[i].radius >= height)
		{
			bouncyballs[i].velocity.y = abs(bouncyballs[i].velocity.y) * -1;
		}

		renderer.RenderCircle(bouncyballs[i].pos, bouncyballs[i].radius, bouncyballColor, i % 8, 32);
	}

	ImGui::SetNextWindowBgAlpha(0.8f);
	static bool openOverlay = true;
	if (ImGui::Begin("IMOGUI - Originoverlay DirectX11 Hook", &openOverlay))
	{
		ImGui::Text("Intermediate Mode Overlay GUI");
		ImGui::ColorPicker3("Bouncyball Color", color);
		bouncyballColor = ImGui::ColorConvertFloat4ToU32(ImVec4(color[2], color[1], color[0], 1.f));
	}
	ImGui::End();

	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::ShowDemoWindow();
}

DWORD __stdcall Initialise(LPVOID hModule)
{
	imogui::Originoverlay overlay;
	overlay.Hook(imogui::Renderapi::DIRECTX11, OnDraw);

	while (true)
	{
		if (GetAsyncKeyState(VK_F2) & 0x1)
		{
			overlay.Unhook();
			break;
		}
		Sleep(1);
	}

	Sleep(100);

	FreeLibraryAndExitThread((HMODULE)hModule, 0);
	return 0;
}

BOOL __stdcall DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, Initialise, hModule, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		break;
	default:
		break;
	}
	return TRUE;
}