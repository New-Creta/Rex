#pragma once

#include "rex_engine/input/key_code.h"
#include "rex_engine/input/mouse_button.h"

#include "rex_std/functional.h"

namespace rex
{
	// Specifies what kind of input was pressed
	// This is mainly targeting PC input for now but
	// can be changed in the future
	enum class InputDeviceType
	{
		Key,
		Mouse,
	};
	// Pressed is not here as thaat's determined by the tick count of a key state
	enum class InputActionType
	{
		Down,
		Up,
	};

	// This is all the data you need to determine what kind of input happened
	struct InputAction
	{
		InputDeviceType device;
		InputActionType type;
		union data
		{
			KeyCode key_code;
			MouseButton mouse_button;
		} data;
	};

	inline bool operator==(const InputAction& lhs, const InputAction& rhs)
	{
		return 
			lhs.device == rhs.device &&
			lhs.type == rhs.type && 
			lhs.data.key_code == rhs.data.key_code
			;
	}
	inline bool operator!=(const InputAction& lhs, const InputAction& rhs)
	{
		return !(lhs == rhs);
	}
}

namespace rsl
{
	template <>
	struct hash<rex::InputAction>
	{
		rsl::hash_result operator()(const rex::InputAction& action) const
		{
			rsl::hash_result hash{};
			hash = rsl::hash_combine(hash, rsl::comp_hash(action.device));
			hash = rsl::hash_combine(hash, rsl::comp_hash(action.type));
			hash = rsl::hash_combine(hash, rsl::comp_hash(action.data.key_code));

			return hash;
		}
	};
}