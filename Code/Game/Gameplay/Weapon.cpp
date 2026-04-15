#include "Game/Gameplay/Weapon.hpp"
#include "Game/Gameplay/WeaponDefinition.hpp"
#include "Game/Gameplay/ActorDefinition.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cmath>


//-----------------------------------------------------------------------------------------------
Weapon::Weapon( WeaponDefinition* definition )
	: m_definition( definition )
{
	if( m_definition->GetType() == WEAPON_TYPE_PROJECTILE )
	{
		std::string projectileName = m_definition->GetProjectileWeaponInfo().m_projectileActor;
		for( unsigned int actorDefIndex = 0; actorDefIndex < ActorDefinition::s_actorDefs.size(); actorDefIndex )
		{
			if( projectileName == ActorDefinition::s_actorDefs[ actorDefIndex ].GetName() )
			{
				m_projectileDefinition = &ActorDefinition::s_actorDefs[ actorDefIndex ];
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Weapon::Fire( Actor* owner )
{
	if( m_definition->GetType() == WEAPON_TYPE_RAY )
	{
		Vec3 startPosition = owner->m_position;
		startPosition.x += owner->m_cosmeticRadius * CosDegrees( owner->m_orientation.m_yawDegrees ) * 0.8f;
		startPosition.y += owner->m_cosmeticRadius * SinDegrees( owner->m_orientation.m_yawDegrees ) * 0.8f;
		startPosition.z = owner->m_definition->GetCameraView().m_eyeHeight * 0.9f;
		Vec3 direction = owner->GetModelToWorldTransform().GetIBasis3D();

		WeaponRaycastResult raycast = owner->m_map->WeaponRaycastAll( startPosition, direction, m_definition->GetRayWeaponInfo().m_rayRange, owner );

		if( raycast.m_didImpact )
		{
			DebugAddWorldCylinder( startPosition, raycast.m_impactPos, 0.01f, 10.f, Rgba8( 0, 0, 150 ), Rgba8( 0, 0, 150 ), DebugRenderMode::X_RAY );

			if( raycast.m_impactedActor )
			{
				FloatRange damageRange = m_definition->GetRayWeaponInfo().m_rayDamage;
				float calculatedDamage = RangeMapClamped( raycast.m_implactDist, 0.f, raycast.m_rayMaxLength, damageRange.m_min, damageRange.m_max );

				raycast.m_impactedActor->Damage( static_cast<int>( roundf( calculatedDamage ) ) );
			}
		}
		else
		{
			Vec3 endPosition = startPosition + ( direction * 10.f );
			DebugAddWorldCylinder( startPosition, endPosition, 0.01f, 10.f, Rgba8( 0, 0, 150 ), Rgba8( 0, 0, 150 ), DebugRenderMode::X_RAY );
		}
	}
}

//-----------------------------------------------------------------------------------------------
Vec3 Weapon::GetRandomDirectionInCone() const
{
	return Vec3();
}

//if( m_definition->GetType() == WEAPON_TYPE_RAY )
//{
//	Vec3 startPosition = owner->m_position;
//	startPosition.z = owner->m_definition->GetCameraView().m_eyeHeight * 0.5f;
//	Vec3 direction = owner->GetModelToWorldTransform().GetIBasis3D();
//
//	WeaponRaycastResult raycast = owner->m_map->WeaponRaycastAll( startPosition, direction, m_definition->GetRayWeaponInfo().m_rayRange, owner );
//
//	if( raycast.m_didImpact )
//	{
//		DebugAddWorldCylinder( startPosition, raycast.m_impactPos, 0.01f, 10.f, Rgba8( 0, 0, 150 ), Rgba8( 0, 0, 150 ), DebugRenderMode::X_RAY );
//
//		if( raycast.m_impactedActor )
//		{
//			FloatRange damageRange = m_definition->GetRayWeaponInfo().m_rayDamage;
//			float calculatedDamage = RangeMapClamped( raycast.m_implactDist, 0.f, raycast.m_rayMaxLength, damageRange.m_min, damageRange.m_max );
//
//			raycast.m_impactedActor->Damage( static_cast<int>( roundf( calculatedDamage ) ) );
//		}
//	}
//	else
//	{
//		Vec3 endPosition = startPosition + ( direction * 10.f );
//		DebugAddWorldCylinder( startPosition, endPosition, 0.01f, 10.f, Rgba8( 0, 0, 150 ), Rgba8( 0, 0, 150 ), DebugRenderMode::X_RAY );
//	}
//}