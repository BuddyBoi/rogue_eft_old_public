#pragma once
#include "include.hpp"
#include "unity.hpp"
#include "eft.hpp"

using namespace framework;

class c_player
{
public:
	uint64_t entity;
	int i_team;
	float f_health;
	float f_distance;
	Vector3D position;
	Vector2D position_screen;

	// 0 = player
	// 1 = player_scav
	// 2 = scav
	int get_team()
	{
		if ( !this->entity )
			return -1;

		Player player = driver::read<Player>( this->entity );

		PlayerProfile player_profile = driver::read<PlayerProfile>( (uintptr_t)player.m_pPlayerProfile );
		PlayerInfo player_info = driver::read<PlayerInfo>( (uintptr_t)player_profile.m_PlayerInfo );
				
		int32_t creation_date = player_info.CreationDate;

		//bot
		if ( !creation_date )
			return 2;

		//player
		else
		{
			if ( player_info.Side == 4 )
				return 1;
			else 
				return 0;
		}
	}

	float get_health()
	{
		if ( !this->entity )
			return 0;

		Player local_player = driver::read<Player>( this->entity );

		HealthController player_health_controller = driver::read<HealthController>( (uintptr_t)local_player.m_pHealthController );
		HealthBody player_health_body = driver::read<HealthBody>( (uintptr_t)player_health_controller.m_pHealthBody );
		BodyController player_body_controller = driver::read<BodyController>( (uintptr_t)player_health_body.m_pBodyController );

		float f_health = 0;

		// Calculate HP
		for ( int limb = 0; limb < NUM_BODY_PARTS; limb++ )
		{
			HealthContainer bodypart = driver::read<HealthContainer>( (uintptr_t)player_body_controller.m_bodyParts[limb].m_pBodyPart );
			Health bodypartHealth = driver::read<Health>( (uintptr_t)bodypart.m_pHealth );
			f_health += bodypartHealth.Health;
		}

		return f_health;
	}

	bool get_destroyed()
	{
		bool b_destroyed = driver::read<bool>( this->entity + 0x798 );
		if ( b_destroyed )
			return true;

		bool b_dead = driver::read<bool>( this->entity + 0x6E0 );
		if ( b_dead )
			return true;

		//float f_health = get_health();
		//if ( f_health < 1.0f )
			//return true;
		
		return false;
	}

	void infinite_stamina()
	{
		if ( !vars::config::b_infinite_stamina )
			return;

		if ( !this->entity )
			return;

		if ( this->get_destroyed() )
			return;

		uint64_t physical = driver::read<uint64_t>( this->entity + 0x500 );
		if ( !physical )
		{
			lg( "failed to find physical playerclass" );
			return;
		}

		uint64_t stamina = driver::read<uint64_t>( physical + 0x38 );
		if ( !stamina )
		{
			lg( "failed to find stamina" );
			return;
		}

		uint64_t oxygen = driver::read <uint64_t>( physical + 0x48 );
		if ( !oxygen )
		{
			lg( "failed to find oxygen" );
			return;
		}

		uint64_t stamina_addr = (driver::read<uint64_t>( physical + 0x38 ) + 0x48);
		uint64_t hands_stamina_addr = (driver::read<uint64_t>( physical + 0x40 ) + 0x48);
		uint64_t oxygen_addr = (driver::read<uint64_t>( physical + 0x48 ) + 0x48);

		//Read vars - 0x48 = current value
		float f_stamina = driver::read<float>( stamina_addr );
		float f_stamina_hands = driver::read<float>( hands_stamina_addr );
		float f_oxygen = driver::read<float>( oxygen_addr );

		if( f_stamina < 50.0f )
			driver::write<float>( stamina_addr, 100.0f );

		if( hands_stamina_addr < 35.0f )
			driver::write<float>( hands_stamina_addr, 70.0f );

		if( f_oxygen < 150.0f )
			driver::write<float>( oxygen_addr, 300.0f );
	}

	void set_skill_level( uint64_t p_skill, float f_value )
	{
		uint64_t profile = driver::read<uint64_t>( this->entity + 0x4F0 );
		if ( !profile )
		{
			lg( "failed to find player profile" );
			return;
		}

		uint64_t skills = driver::read<uint64_t>( profile + 0x60 );
		if ( !skills )
		{
			lg( "failed to find skills" );
			return;
		}

		uint64_t selected_skill = driver::read<uint64_t>( skills + p_skill );
		if ( !selected_skill )
		{
			lg( "bd_skill_change failed" );
			return;
		}

		driver::write<float>( selected_skill + 0x30, f_value );
	}

	void instant_search()
	{
		set_skill_level( 0x160, 50.0f );
	}

	void instant_examine()
	{
		set_skill_level( 0x168, 50.0f );
	}

	void grenade_longthrow()
	{
		//StrengthBuffThrowDistance
		set_skill_level( 0x70, 50.0f );
	}

	void fast_magdrills()
	{
		//Mag load speed - MagDrillsLoadSpeed
		set_skill_level( 0x180, 50.0f );

		//mag unload speed - MagDrillsUnloadSpeed
		set_skill_level( 0x188, 50.0f );
	}

	void recoil_remove()
	{
		if ( !vars::config::b_remove_recoil )
			return;

		if ( !this->entity )
			return;

		if ( this->get_destroyed() )
			return;

		Player localplayer = driver::read<Player>( this->entity );

		ProceduralWeaponAnimation weaponAnimation = driver::read<ProceduralWeaponAnimation>( (uintptr_t)localplayer.m_pProceduralWeaponAnimation );

		//HandsController handsController = driver::read<HandsController>( (uintptr_t)localplayer.m_pHandsController );
		//WeaponItem weaponItem = driver::read<WeaponItem>( (uintptr_t)handsController.m_pWeaponItem );
		//ItemTemplate itemTemplate = driver::read<ItemTemplate>( (uintptr_t) weaponItem.m_BackingField);
		//itemTemplate.m_allowFeed = false;
		//itemTemplate.m_allowJam = false;
		//itemTemplate.m_allowMisfire = false;
		//itemTemplate.m_allowSlide = false;
		//process->Write<ItemTemplate>( (uintptr_t) weaponItem.m_BackingField, itemTemplate );

		//Logger::Log("proceduralWeaponAnim (%p)\n", localplayer.m_pProceduralWeaponAnimation);
		ShotEffector shotEffector = driver::read<ShotEffector>( (uintptr_t)weaponAnimation.m_pShooting );
		shotEffector.recoilHori = 0.0f;
		shotEffector.recoilVert = 0.0f;
		driver::write<ShotEffector>( (uintptr_t)weaponAnimation.m_pShooting, shotEffector );

		BreathEffector breathEffector = driver::read<BreathEffector>( (uintptr_t)weaponAnimation.m_pBreath );
		breathEffector.Intensity = 0.0f;
		driver::write<BreathEffector>( (uintptr_t)weaponAnimation.m_pBreath, breathEffector );

		weaponAnimation.mask = 1;
		driver::write<ProceduralWeaponAnimation>( (uintptr_t)localplayer.m_pProceduralWeaponAnimation, weaponAnimation );

		//FirearmController fire = driver::read<FirearmController>( (uintptr_t)weaponAnimation.m_pFiring );

		//
		/*Anti-Jam- Broken As of July 13th
		WeaponItem weaponItem = driver::read<WeaponItem>( (uintptr_t)fire.m_pItem );

		WeaponTemplate weaponTemplate = driver::read<WeaponTemplate>( (uintptr_t)weaponItem.m_BackingField );

		if ( weaponTemplate.m_allowFeed )
		{
			weaponTemplate.m_allowSlide = false;
			weaponTemplate.m_allowMisfire = false;
			weaponTemplate.m_allowJam = false;
			weaponTemplate.m_allowFeed = false;
			driver::write<WeaponTemplate>( (uintptr_t)weaponItem.m_BackingField, weaponTemplate );
		}*/

		//fire.m_hipInaccuracy = 0.0f;
		//fire.m_weaponLn = 0.1f; // buggy
		//driver::write<FirearmController>( (uintptr_t)weaponAnimation.m_pFiring, fire );
		//process->Write<int32_t>( (uintptr_t)localplayer.m_pProceduralWeaponAnimation + offsetof(ProceduralWeaponAnimation, Mask), 1 );

		//process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, floater1), 1.0f );
		//process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, floater2), 0.0f ); // spread
		//process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, floater3), 1.0f );
		//fire.bulletSpeedMultiplier = 5.0f;
		//process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, bulletSpeedMultiplier), 4.9f );
	}

	void recoil_remove_animation()
	{
		auto procedural_weapon_animation = driver::read<uintptr_t>( entity + 0x198 );
		if ( procedural_weapon_animation )
		{
			driver::write<int>( procedural_weapon_animation + 0x100, 0 ); // mask
		}
	}

	c_player( uint64_t address )
	{
		entity = address;
	}
};