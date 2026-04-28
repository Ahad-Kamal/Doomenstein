#include "Game/Gameplay/Weapon.hpp"
#include "Game/Gameplay/WeaponDefinition.hpp"
#include "Game/Gameplay/ActorDefinition.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <cmath>


//-----------------------------------------------------------------------------------------------
Weapon::Weapon( WeaponDefinition* definition )
	: m_definition( definition )
{
	if( m_definition->GetType() == WEAPON_TYPE_PROJECTILE )
	{
		std::string projectileName = m_definition->GetProjectileWeaponInfo().m_projectileActor;
		for( unsigned int actorDefIndex = 0; actorDefIndex < ActorDefinition::s_actorDefs.size(); actorDefIndex++ )
		{
			if( projectileName == ActorDefinition::s_actorDefs[ actorDefIndex ].GetName() )
			{
				m_projectileDefinition = &ActorDefinition::s_actorDefs[ actorDefIndex ];
			}
		}
	}

	m_cooldownTimer = new Timer( m_definition->GetRefireTime(), g_game->m_gameClock );
	m_cooldownTimer->Start();
	m_cooldownTimer->ElaspePeriod();
}

//-----------------------------------------------------------------------------------------------
void Weapon::Fire( Actor* owner )
{
	if( !m_cooldownTimer->DecrementPeriodIfElapsed() )
	{
		return;
	}

	WeaponType weaponType = m_definition->GetType();
	if( weaponType == WEAPON_TYPE_RAY )
	{
		Vec3 startPosition = owner->m_position;
		startPosition.x += owner->m_cosmeticRadius * CosDegrees( owner->m_orientation.m_yawDegrees ) * 0.8f;
		startPosition.y += owner->m_cosmeticRadius * SinDegrees( owner->m_orientation.m_yawDegrees ) * 0.8f;
		startPosition.z = owner->m_definition->GetCameraView().m_eyeHeight * 0.9f;
		Vec3 direction = owner->GetModelToWorldTransform().GetIBasis3D();

		ActorRaycastResult raycast = owner->m_map->ActorRaycastAll( startPosition, direction, m_definition->GetRayWeaponInfo().m_rayRange, owner );

		RayWeapon rayDef = m_definition->GetRayWeaponInfo();
		for( int rayCount = 0; rayCount < rayDef.m_rayCount; rayCount++ )
		{
			if( raycast.m_didImpact )
			{
				DebugAddWorldCylinder( startPosition, raycast.m_impactPos, 0.01f, 1.f, Rgba8( 0, 0, 150 ), Rgba8( 0, 0, 150 ), DebugRenderMode::X_RAY );

				if( raycast.m_impactedActor )
				{
					FloatRange damageRange = rayDef.m_rayDamage;
					float calculatedDamage = g_rng->RollRandomFloatInRange( damageRange.m_min, damageRange.m_max );

					raycast.m_impactedActor->Damage( static_cast<int>( roundf( calculatedDamage ) ), owner->m_actorHandle );
				}
			}
			else
			{
				Vec3 endPosition = startPosition + ( direction * 10.f );
				DebugAddWorldCylinder( startPosition, endPosition, 0.01f, 10.f, Rgba8( 0, 0, 150 ), Rgba8( 0, 0, 150 ), DebugRenderMode::X_RAY );
			}
		}
	}

	if( weaponType == WEAPON_TYPE_PROJECTILE )
	{
		EulerAngles projectileDirection = owner->m_orientation;
		ProjectileWeapon projectileDef = m_definition->GetProjectileWeaponInfo();
		float cone = projectileDef.m_projectileCone;

		for( int projectileCount = 0; projectileCount < projectileDef.m_projectileCount; projectileCount++ )
		{
			projectileDirection = GetRandomDirectionInCone( projectileDirection, cone, cone );

			Vec3 projectileSpawnPosition = Vec3( owner->m_position.x, owner->m_position.y, owner->m_position.z + owner->m_definition->GetCameraView().m_eyeHeight * 0.85f );
			projectileSpawnPosition.x += owner->m_cosmeticRadius * CosDegrees( owner->m_orientation.m_yawDegrees ) * 0.8f;
			projectileSpawnPosition.y += owner->m_cosmeticRadius * SinDegrees( owner->m_orientation.m_yawDegrees ) * 0.8f;
			Actor* newProjectile = owner->m_map->SpawnActor( m_projectileDefinition->GetName(), projectileSpawnPosition, EulerAngles() );
			newProjectile->m_owner = owner;
			newProjectile->AddImpulse( projectileDirection.GetForwardDir_IFwd_JLeft_KUp() * projectileDef.m_projectileSpeed );
		}
	}

	if( weaponType == WEAPON_TYPE_MELEE )
	{
		// Note this wont work if the owner does not have an ai controller
		ActorHandle targetActorHandle = owner->m_map->GetClosestVisibleEnemy( owner );

		if( targetActorHandle == ActorHandle::INVALID )
		{
			return;
		}

		Actor* targetActor = owner->m_map->GetActorByHandle( targetActorHandle );

		MeleeWeapon meleeDef = m_definition->GetMeleeWeaponInfo();
		if( IsPointInsideOrientedSector2D( Vec2( targetActor->m_position ), Vec2( owner->m_position ), owner->m_orientation.m_yawDegrees, meleeDef.m_meleeArc, meleeDef.m_meleeRange ) )
		{
			FloatRange damageRange = meleeDef.m_meleeDamage;

			float calculatedDamage = g_rng->RollRandomFloatInRange( damageRange.m_min, damageRange.m_max );
			targetActor->Damage( static_cast<int>( roundf( calculatedDamage ) ), owner->m_actorHandle );
		}
	}
}

//-----------------------------------------------------------------------------------------------
EulerAngles Weapon::GetRandomDirectionInCone( EulerAngles const& orientation, float yawOffset, float pitchOffset ) const
{
	float randomYawOffset = g_rng->RollRandomFloatInRange( -yawOffset, yawOffset );
	float randomPitchOffset = g_rng->RollRandomFloatInRange( -pitchOffset, pitchOffset );

	return EulerAngles( orientation.m_yawDegrees + randomYawOffset, orientation.m_pitchDegrees + randomPitchOffset, orientation.m_rollDegrees );
}