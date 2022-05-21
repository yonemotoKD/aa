#include"collision.h"

Rect Collision::SetPlayRect(Math::Vector2* a_pos, float a_radius)
{
	Rect rect;
	rect.Up = a_pos->y + a_radius ;
	rect.Down = a_pos->y - a_radius;
	rect.Right = a_pos->x + a_radius ;
	rect.Left = a_pos->x - a_radius ;

	return rect;
}

Rect Collision::SetRect(Math::Vector2* a_pos, float a_radius)
{
	Rect rect;
	rect.Up = a_pos->y + a_radius;
	rect.Down = a_pos->y - a_radius;
	rect.Right = a_pos->x + a_radius;
	rect.Left = a_pos->x - a_radius;

	return rect;
}

Rect Collision::SetRect(Math::Vector2 a_pos, float a_radius)
{
	Rect rect;
	rect.Up = a_pos.y + a_radius;
	rect.Down = a_pos.y - a_radius;
	rect.Right = a_pos.x + a_radius;
	rect.Left = a_pos.x - a_radius;

	return rect;
}

Rect Collision::SetFutureRect(Rect a_rect, Math::Vector2* a_move)
{
	Rect futureRect;

	futureRect.Up = a_rect.Up + a_move->y;
	futureRect.Down = a_rect.Down + a_move->y;
	futureRect.Right = a_rect.Right + a_move->x;
	futureRect.Left = a_rect.Left + a_move->x;

	return futureRect;
}
