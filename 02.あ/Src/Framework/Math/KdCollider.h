#pragma once

class KdGameObject;
class CollisionShape;

class KdCollider
{
public:

	// 衝突タイプ
	enum Type
	{
		TypeGround		= 1 << 0,
		TypeBump		= 1 << 1,
		TypeAttack		= 1 << 2,
		TypeAttackLine	= 1 << 3,
	};

	struct RayInfo
	{
		RayInfo() {}
		RayInfo(const Math::Vector3& pos, const Math::Vector3& dir, float range) 
			: m_pos(pos), m_dir(dir),m_range(range)
		{
			m_dir.Normalize();
		}

		Math::Vector3 m_pos;		// レイの発射位置
		Math::Vector3 m_dir;		// レイの方向
		float m_range = 0;			// 判定限界距離
	};

	struct CollisionResult
	{
		bool m_hit = false;				// 衝突したかどうか
		Math::Vector3 m_hitPos;			// 衝突した座標
		Math::Vector3 m_hitDir;			// 対象との間の方向
		float m_overlapDistance = 0.0f; // 重なり量
	};

	KdCollider(KdGameObject& rOwner):m_owner(rOwner) {}
	
	~KdCollider() {}

	void AddCollisionShape(std::shared_ptr<CollisionShape> spShape) 
	{
		if (!spShape) { return; }

		m_collisions.push_back(spShape); 
	}

	bool Intersects(Type checkType, const DirectX::BoundingSphere& target, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(Type checkType, const KdCollider::RayInfo& target, std::list<KdCollider::CollisionResult>* pResults);

private:
	std::list<std::shared_ptr<CollisionShape>> m_collisions;

	KdGameObject& m_owner;
};

class CollisionShape
{
public:

	CollisionShape(UINT type) { m_type = type; }

	virtual ~CollisionShape() {}

	UINT GetType() const { return m_type; }

	virtual bool Intersects(const DirectX::BoundingSphere& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) = 0;
	virtual bool Intersects(const KdCollider::RayInfo& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) = 0;

private:
	// 何用の当たり判定か
	UINT m_type = 0;
};

 // コライダー：球形状
class SphereCollision : public CollisionShape
{
public:
	SphereCollision(const DirectX::BoundingSphere& sphere, UINT type) :
		CollisionShape(type), m_shape(sphere) {}
	SphereCollision(const Math::Vector3& localPos, float radius, UINT type) :
		CollisionShape(type) { m_shape.Center = localPos; m_shape.Radius = radius; }

	virtual ~SphereCollision() {}

	bool Intersects(const DirectX::BoundingSphere& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) override;
	bool Intersects(const KdCollider::RayInfo& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) override;

private:
	DirectX::BoundingSphere m_shape;
};

// コライダー：モデル形状
class ModelCollision : public CollisionShape
{
public:
	ModelCollision(const std::shared_ptr<KdModelWork>& model, UINT type) :
		CollisionShape(type), m_shape(model) {}

	virtual ~ModelCollision() { m_shape = nullptr; }

	bool Intersects(const DirectX::BoundingSphere& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) override;
	bool Intersects(const KdCollider::RayInfo& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) override;

private:
	std::shared_ptr<KdModelWork> m_shape = nullptr;
};