#pragma once

#include <Geode/Geode.hpp>

// some of these aren't in bindings
#if GEODE_COMP_GD_VERSION == 22081
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
