#include "KdGameObject.h"

void KdGameObject::RegisterCreateFunction(KdGameObjectFactory& factory)
{
    factory.RegisterCreateFunction("GameObject", [factory]() mutable { return factory.CreateGameObject<KdGameObject>(); });
}

void KdGameObject::SetScale(const Math::Vector3& scale)
{
    Math::Vector3 vecX = m_mWorld.Right();
    Math::Vector3 vecY = m_mWorld.Up();
    Math::Vector3 vecZ = m_mWorld.Backward(); 
    
    vecX.Normalize();
    vecY.Normalize();
    vecZ.Normalize();

    m_mWorld.Right(vecX * scale.x);
    m_mWorld.Up(vecY * scale.y);
    m_mWorld.Backward(vecZ * scale.z);
}

Math::Vector3 KdGameObject::GetScale() const
{
    return Math::Vector3(m_mWorld.Right().Length(), m_mWorld.Up().Length(), m_mWorld.Backward().Length());
}
