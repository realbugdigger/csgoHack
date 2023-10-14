#pragma once
#include <cstdint>

namespace memory {
	// given the interface pointer, calls the function at index provided
	template <typename Return, typename ... Args>
	constexpr Return Call(void* _this, const std::uintptr_t index, Args ... args) noexcept {

		using Function = Return(__thiscall*)(void*, decltype(args) ...);
		return (*static_cast<Function**>(_this))[index](_this, args...);
	}

	// given the interface pointer, returns the function at index provided
	constexpr void* Get(void* _this, const std::uintptr_t index) noexcept {

		return (*static_cast<void***>(_this))[index];
	}

}