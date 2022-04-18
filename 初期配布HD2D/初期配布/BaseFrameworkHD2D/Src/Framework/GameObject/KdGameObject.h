#pragma once

class KdGameObjectFactory;

class KdGameObject : public std::enable_shared_from_this<KdGameObject>
{
public:
	KdGameObject() {}
	virtual ~KdGameObject() { Release(); }

	virtual void Init() {}
	virtual void RegisterCreateFunction(KdGameObjectFactory& factory);

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}

	virtual void GenerateDepthMapFromLight() {}
	virtual void DrawOpeque() {}
	virtual void DrawTranslucent() {}
	virtual void DrawBright() {}
	virtual void DrawSprite() {}

	virtual void SetAsset(const std::string& fileName) {}

	virtual void SetPos(const Math::Vector3& pos) { m_mWorld.Translation(pos); }
	virtual Math::Vector3 GetPos() const { return m_mWorld.Translation(); }

	virtual void SetScale(const Math::Vector3& scale);
	virtual Math::Vector3 GetScale() const;

	const Math::Matrix& GetMatrix() const { return m_mWorld; }
	const std::unique_ptr<KdCollider>& GetCollider() const { return m_pCollider; }

	bool IsAlive() { return m_isAlive; }

protected:

	void Release() {}

	Math::Matrix	m_mWorld;

	std::unique_ptr<KdCollider> m_pCollider;

	bool m_isAlive = true;
};