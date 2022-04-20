#pragma once

class KdgameSystem
{
public:
	KdgameSystem();
	~KdgameSystem();

	void Init();
	void Update();
	void Draw();

private:
	KdSquarePolygon poly;
	KdCamera* Shatter;
	void Release();
};