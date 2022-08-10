#pragma once
namespace VALROM {
	inline uintptr_t decryptWorld(uintptr_t base_address) {
		auto key = Read<uintptr_t>(base_address + offsets::uworld_key);
#pragma pack(push, 1)
		struct State
		{
			uint64_t Keys[7];
		};
#pragma pack(pop)
		auto state = Read<State>(base_address + offsets::uworld_state);
		auto decrypt = decrypt_uworld(key, (uintptr_t*)&state);
		return Read<uintptr_t>(decrypt);
	}
}