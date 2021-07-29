#pragma once
#include <string>

#include "imgui/imgui.h"



namespace imogui
{
	enum class Overlay
	{
		STEAM,
		ORIGIN,
		OVERWOLF,
		OPENBROADCAST,
		NVIDIASHARE,
		MSIAFTERBURNER,
		DISCORD
	};

	enum class Renderapi
	{
		OPENGL,
		DIRECTX9,
		DIRECTX11
	};

	class Renderer
	{
	private:
		static Renderer* _instance;

		int width_ = 0;
		int height_ = 0;

		Renderer();
		~Renderer();

	public:
		static Renderer& Get();

		bool inFrame;

		int GetWidth();
		int GetHeight();

		void SetWidth(int width);
		void SetHeight(int height);

		void BeginScene();
		void EndScene();

		void RenderText(const std::string& text, const ImVec2& position, float size, uint32_t color);
		void RenderLine(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness = 1.0f);
		void RenderPolygon(const ImVec2* points, int pointCount, uint32_t color, float thickness = 1.0f);
		void RenderCircle(const ImVec2& position, float radius, uint32_t color, float thickness = 1.0f, uint32_t segments = 16);
		void RenderCircleFilled(const ImVec2& position, float radius, uint32_t color, uint32_t segments = 16);
		void RenderRect(const ImVec2& from, const ImVec2& to, uint32_t color, float rounding = 0.0f, uint32_t roundingCornersFlags = ImDrawCornerFlags_All, float thickness = 1.0f);
		void RenderRect(const ImVec2& one, const ImVec2& two, const ImVec2& three, const ImVec2& four, uint32_t color, float thickness = 1.0f);
		void RenderRectFilled(const ImVec2& from, const ImVec2& to, uint32_t color, float rounding = 0.0f, uint32_t roundingCornersFlags = ImDrawCornerFlags_All);
	};
}