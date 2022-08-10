#pragma once
namespace VALROM {
	class Config {
	public:
		int g_width{};
		int g_height{};
		float g_AimSmooth = 0.f;
		float g_AimFov = 0.f;
		ImU32 g_esp_color = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 0.4F, 1));
		ImU32 g_color_white = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1));
		// Cheat toggle values
		bool g_overlay_visible{ false };
		bool g_esp_enabled{ true };
		bool g_Aimbot_enable{ true };
		bool g_esp_dormantcheck{ false };
		bool g_headesp{ true };
		bool g_boneesp{ true };
		bool g_AmmoCount{ true };
		bool g_boxesp{ true };
		bool g_Aimbot{ true };
		bool g_DrawFov{ true };
	};

	inline auto g_Config = std::make_unique<Config>();
}