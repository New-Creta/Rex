#pragma once

#include "rex_engine/input/key_code.h"
#include "rex_engine/input/mouse_button.h"

#include "rex_std/functional.h"

namespace rex
{
	// Specifies what kind of input was pressed
	enum class InputActionType
	{
		Key,
		Mouse,
	};

	struct InputAction
	{
		InputActionType type;
		union data
		{
			KeyCode key_code;
			MouseButton mouse_button;
		} data;
	};

	inline bool operator==(const InputAction& lhs, const InputAction& rhs)
	{
		return lhs.type == rhs.type && lhs.data.key_code == rhs.data.key_code;
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
			hash = rsl::hash_combine(hash, rsl::comp_hash(action.type));
			hash = rsl::hash_combine(hash, rsl::comp_hash(action.data.key_code));

			return hash;
		}
	};
}