#pragma once

#include <Geode/Geode.hpp>

#if GEODE_COMP_GD_VERSION == 22091
	// thanks to `talindrusk` for finding these while I was in my lazy era
	#if defined(GEODE_IS_ARM_MAC)
		constexpr std::uintptr_t AppController_applicationDidFinishLaunching_addr = 0x32f4;

		constexpr std::uintptr_t SteamManager_sharedState_addr = 0x5f33d0; // used in applicationDidFinishLaunching
		constexpr std::uintptr_t SteamManager_setupSteam_addr = 0x5f3620;
	#elif defined(GEODE_IS_INTEL_MAC)
		constexpr std::uintptr_t AppController_applicationDidFinishLaunching_addr = 0x3720;

		constexpr std::uintptr_t SteamManager_sharedState_addr = 0x6de2f0;
		constexpr std::uintptr_t SteamManager_setupSteam_addr = 0x6de550;
	#endif
#elif GEODE_COMP_GD_VERSION == 22081
	#if defined(GEODE_IS_ARM_MAC)
		constexpr std::uintptr_t AppController_applicationDidFinishLaunching_addr = 0x6914;

		constexpr std::uintptr_t SteamManager_sharedState_addr = 0x52d2cc; // used in applicationDidFinishLaunching
		constexpr std::uintptr_t SteamManager_setupSteam_addr = 0x52d51c;
	#elif defined(GEODE_IS_INTEL_MAC)
		constexpr std::uintptr_t AppController_applicationDidFinishLaunching_addr = 0x7470;

		constexpr std::uintptr_t SteamManager_sharedState_addr = 0x606910;
		constexpr std::uintptr_t SteamManager_setupSteam_addr = 0x606b70;
	#endif
#endif

struct SteamManager : public cocos2d::CCNode {
	static SteamManager* sharedState() {
		return reinterpret_cast<SteamManager*(*)()>(geode::base::get() + SteamManager_sharedState_addr)();
	}

	bool setupSteam() {
		return reinterpret_cast<bool(*)(SteamManager*)>(geode::base::get() + SteamManager_setupSteam_addr)(this);
	}
};
