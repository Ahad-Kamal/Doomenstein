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
}

//-----------------------------------------------------------------------------------------------
void Weapon::Fire( Actor* owner )
{
	if( m_definition->GetType() == WEAPON_TYPE_RAY )
	{
		Vec3 startPosition = owner->m_position;
		startPosition.z = owner->m_definition->GetCameraView().m_eyeHeight;
		Vec3 direction = owner->GetModelToWorldTransform().GetIBasis3D();
		Vec3 endPosition = startPosition + ( direction * 10.f );
		DebugAddWorldCylinder( startPosition, endPosition, 0.01f, 10.f, Rgba8::WHITE, Rgba8::WHITE, DebugRenderMode::X_RAY );

		WeaponRaycastResult raycast = owner->m_map->WeaponRaycastAll( startPosition, direction, 10.f, owner );

		if( raycast.m_didImpact )
		{
			/*DebugAddWorldSphere( raycast.m_impactPos, 0.06f, 10.f );
			Vec3 arrowStart = raycast.m_impactPos;
			Vec3 arrowEnd = raycast.m_impactPos + ( raycast.m_impactNormal * 0.3f );
			DebugAddWorldArrow( arrowStart, arrowEnd, 0.03f, 10.f, Rgba8::BLUE, Rgba8::BLUE );*/

			if( raycast.m_impactedActor )
			{
				FloatRange damageRange = m_definition->GetRayWeaponInfo().m_rayDamage;
				float calculatedDamage = RangeMapClamped( raycast.m_implactDist, 0.f, raycast.m_rayMaxLength, damageRange.m_min, damageRange.m_max );

				raycast.m_impactedActor->Damage( static_cast<int>( roundf( calculatedDamage ) ) );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
Vec3 Weapon::GetRandomDirectionInCone() const
{
	return Vec3();
}

