#pragma once
struct Rect
{
	float Up;
	float Down;
	float Right;
	float Left;
};
namespace Collision
{
	Rect SetPlayRect(Math::Vector2* a_pos, float a_radius);

	Rect SetRect(Math::Vector2* a_pos, float a_radius);

	Rect SetRect(Math::Vector2 a_pos, float a_radius);

	Rect SetFutureRect(Rect a_rect, Math::Vector2* a_move);

};
