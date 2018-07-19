/*
    Copyright 2018 Samuel Siltanen
    Action.hpp
*/

#pragma once

#include "../Types.hpp"

namespace input
{
	enum class ActionType
	{
		MoveForward,
		MoveBackward,
		StrafeLeft,
		StrafeRight,
		Select,
		Rotate
	};

	struct Action
	{
		ActionType	type;
		void*		data;

		Action(ActionType type, void* data = nullptr) :
			type(type),
			data(data)
		{}
	};

	class ActionListener
	{
	public:
		virtual void onAction(const Action& action) = 0;
	};
}
