#include "include.hpp"
#include "overlay.h"
#include "c_player.hpp"
#include "eft.hpp"
#include "unity.hpp"

#include <codecvt>

class rect
{
public:
	float x, y, w, h;
};

namespace Tarky
{
	//core values
	uint64_t base_address;
	DWORD process_id = 0;

	bool setup()
	{
		while ( !FindWindowA( "UnityWndClass", "EscapeFromTarkov" ) )
		{
			lg( "waiting for g" );
			std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
		}
		HWND _hwnd = FindWindowA( "UnityWndClass", "EscapeFromTarkov" );
		if ( !_hwnd )
			return false;

		process_id = 0;
		GetWindowThreadProcessId( _hwnd, &process_id );
		if ( !process_id )
			return false;

		lg( "Processid: " + std::to_string( process_id ) );
		driver::attach_process( (ULONG)process_id );

		uint64_t process_address = driver::get_base_address();
		lg( "Base address: " + std::to_string( process_address ) );

		base_address = driver::get_module_base( "UnityPlayer.dll" );
		if ( !base_address )
		{
			lg( "failed to get base addy" );
			return false;
		}
		lg( "UnityPlayer address: " + std::to_string( base_address ) );

		vars::b_setup = true;
		return true;
	}

	bool World2Screen( const Vector3D& world, Vector2D* screen, uint64_t u_camera_component )
	{
		if ( !u_camera_component )
			return false;

		Matrix4x4 matrix = driver::read<Matrix4x4>( u_camera_component + 0xDC );
		matrix = matrix.Transpose();
		const Vector3D translation = { matrix.m[3][0], matrix.m[3][1], matrix.m[3][2] };
		const Vector3D up = { matrix.m[1][0], matrix.m[1][1], matrix.m[1][2] };
		const Vector3D right = { matrix.m[0][0], matrix.m[0][1], matrix.m[0][2] };

		const float w = translation.DotProduct( world ) + matrix.m[3][3];

		// Not on screen
		if ( w < 0.1f )
			return false;

		float x = right.DotProduct( world ) + matrix.m[0][3];
		float y = up.DotProduct( world ) + matrix.m[1][3];

		//if ( localPlayerIsAiming && localPlayerIsOpticCamera )
		//{
			//TODO: scope scale
		//}

		float return_x = 0.0f;
		float return_y = 0.0f;

		return_x = (vars::i_screen_x / 2) * (1.f + x / w);
		return_y = (vars::i_screen_y / 2) * (1.f - y / w);

		//off-screen check
		if ( return_x < 5.0f || return_y < 5.0f || return_y >( vars::i_screen_y - 5.0f ) || return_x >( vars::i_screen_x - 5.0f ) )
			return false;

		screen->x = return_x;
		screen->y = return_y;
		return true;
	}

	uint64_t GetCamera( uint64_t gom )
	{
		uint64_t firstObjectPtr;
		uint64_t lastObjectPtr;

		const auto camera_tagged_node = driver::read<uint64_t >( gom + 0x10 );
		if ( camera_tagged_node == driver::read<uint64_t>( camera_tagged_node + 0x8 ) )
		{
			return 0;
		}

		firstObjectPtr = driver::read<uint64_t>( gom + 0x18 );
		lastObjectPtr = driver::read<uint64_t>( gom + 0x10 );

		uintptr_t itr = firstObjectPtr;

		while ( itr )
		{
			mono_object_wrapper wrapper = driver::read<mono_object_wrapper>( itr );
			mono_object object = driver::read<mono_object>( (uintptr_t)wrapper.object );
			char name[256] = { 0 };
			driver::read_buffer( (uint64_t)object.objectname, 256, name );

			if ( !strcmp( "FPS Camera", name ) )
			{
				game_object_wrapper gameobjWrapper = driver::read<game_object_wrapper>( (uintptr_t)object.pObjectClass );
				return (uintptr_t)gameobjWrapper.gameObject;
			}
			if ( itr == lastObjectPtr )
				break;

			itr = (uintptr_t)wrapper.next;
		}

		return 0;
	}

	uint64_t GetObjectPtrByName( uint64_t gom, const char* objname, bool tagged )
	{
		uint64_t firstObjectPtr;
		uint64_t lastObjectPtr;

		if ( tagged )
		{
			firstObjectPtr = driver::read<uint64_t>( gom + 0x18 );
			lastObjectPtr = driver::read<uint64_t>( gom + 0x10 );
		}
		else
		{
			firstObjectPtr = driver::read<uint64_t>( gom + 0x28 );
			lastObjectPtr = driver::read<uint64_t>( gom + 0x20 );
		}

		uintptr_t itr = firstObjectPtr;

		while ( itr )
		{
			mono_object_wrapper wrapper = driver::read<mono_object_wrapper>( itr );
			mono_object object = driver::read<mono_object>( (uintptr_t)wrapper.object );
			char name[256] = { 0 };
			driver::read_buffer( (uint64_t)object.objectname, 256, name );

			if ( !strcmp( objname, name ) )
			{
				game_object_wrapper gameobjWrapper = driver::read<game_object_wrapper>( (uintptr_t)object.pObjectClass );
				return (uintptr_t)gameobjWrapper.gameObject;
			}
			if ( itr == lastObjectPtr )
				break;

			itr = (uintptr_t)wrapper.next;
		}

		return 0;
	}

	uint64_t GetWorldPtr( uint64_t gom )
	{
		uint64_t firstObjectPtr;
		uint64_t lastObjectPtr;

		firstObjectPtr = driver::read<uint64_t>( gom + 0x28 );
		lastObjectPtr = driver::read<uint64_t>( gom + 0x20 );

		uintptr_t itr = firstObjectPtr;

		while ( itr )
		{
			mono_object_wrapper wrapper = driver::read<mono_object_wrapper>( itr );
			mono_object object = driver::read<mono_object>( (uintptr_t)wrapper.object );
			char name[256] = { 0 };
			driver::read_buffer( (uint64_t)object.objectname, 256, name );

			if ( !strcmp( "GameWorld", name ) )
			{
				game_object_wrapper gameobjWrapper = driver::read<game_object_wrapper>( (uintptr_t)object.pObjectClass );
				GameWorldWrapper gameWorldWrapper = driver::read<GameWorldWrapper>( (uintptr_t)gameobjWrapper.gameObject );
				LOCALGAMEWORLD world = driver::read<LOCALGAMEWORLD>( (uintptr_t)gameWorldWrapper.localgameworld );
				Array playerList = driver::read<Array>( (uintptr_t)world.m_pPlayerList );
				if ( playerList.Count == 0 )
					goto next;

				return (uintptr_t)gameWorldWrapper.localgameworld;
			}
			if ( itr == lastObjectPtr )
				break;

next:
			itr = (uintptr_t)wrapper.next;
		}

		return 0;
	}

	static uint64_t global_object_manager = 0;
	static uint64_t local_game_world = 0;
	static uint64_t local_camera = 0;

	enum E_EXTRACT_STATUS
	{
		EXTRACT_CLOSED = 1,
		EXTRACT_INCOMPLETE_REQ = 2,
		EXTRACT_COUNTDOWN = 3,
		EXTRACT_OPEN = 4,
		EXTRACT_PENDING = 5,
		EXTRACT_MANUAL_ACTIVATION = 6
	};

	class c_item
	{
	public:
		uint64_t address;
		std::string name;
		Vector3D position;

		Item item;
		ItemProfile itemProfile;
		ItemBasicInformation itemBasicInformation;
		ItemLocalization itemLocalization;
		ItemCoordinates itemCoordinates;
		ItemLocationContainer itemLocationContainer;

		Vector3D get_position()
		{
			return itemLocationContainer.ItemPosition;
		}

		c_item( uint64_t in_address )
		{
			address = in_address;

			char nameBuffer[128]{};
			float distance = 0.0f;
			Vector2D item2D{};

			//get item info
			item = driver::read<Item>( address );
			itemProfile = driver::read<ItemProfile>( (uint64_t)item.m_pItemProfile );
			itemBasicInformation = driver::read<ItemBasicInformation>( (uint64_t)itemProfile.m_pItemInformation );
			itemLocalization = driver::read<ItemLocalization>( (uint64_t)itemBasicInformation.m_pItemLocalization );
			itemCoordinates = driver::read<ItemCoordinates>( (uint64_t)itemLocalization.m_pItemCoordinates );
			itemLocationContainer = driver::read<ItemLocationContainer>( (uint64_t)itemCoordinates.m_pItemLocationContainer );

			driver::read_buffer( (uint64_t)itemBasicInformation.ItemPatName, 128, nameBuffer );
			if ( !nameBuffer )
				return;

			nameBuffer[127] = '\0';
			name = std::string( nameBuffer );

			position = get_position();
		}
	};
	std::vector<c_item> v_items{};

	static c_player g_local_player = c_player( 0 );

	static bool b_world_active = false;
	bool b_get_world_active()
	{
		//not in game / entrance check
		if ( !b_world_active )
		{
			uint64_t temp_camera = GetCamera( global_object_manager );
			if ( !temp_camera )
				return false;

			std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );

			uint64_t temp_camera_delay = GetCamera( global_object_manager );
			if ( !temp_camera_delay )
				return false;

			return true;
		}

		//in game / exit check
		else
		{
			uint64_t temp_camera = GetCamera( global_object_manager );
			if ( !temp_camera )
				return false;

			uint64_t temp_game_world = GetWorldPtr( global_object_manager );
			if ( !temp_game_world )
				return false;

			if ( g_local_player.entity )
			{
				if ( g_local_player.get_destroyed() )
					return false;
			}

			return true;
		}
	}

	void world_init()
	{
		auto camera_object = GetCamera( global_object_manager );

		while ( !camera_object )
		{
			camera_object = GetCamera( global_object_manager );
		}

		local_camera = GetCamera( global_object_manager );
		local_game_world = GetWorldPtr( global_object_manager );

		while ( !local_game_world )
		{
			local_game_world = GetWorldPtr( global_object_manager );
		}
	}

	void world_deinit()
	{
		local_camera = 0;
		local_game_world = 0;
		v_items.clear();
		g_local_player = c_player( 0 );
	}

	void world_update()
	{
		bool active = b_get_world_active();

		//world state has changed
		if ( active != b_world_active )
		{
			//world is now active
			if ( active )
			{
				world_init();
			}

			//world is now deactive
			else
			{
				world_deinit();
			}
		}

		//set new cached state
		b_world_active = active;
	}

	unsigned long col_red = D3DCOLOR_ARGB( 255, 255, 0, 0 );
	unsigned long col_blue = D3DCOLOR_ARGB( 255, 0, 0, 255 );
	unsigned long col_green = D3DCOLOR_ARGB( 255, 0, 255, 0 );

	void thread_process_monitor()
	{
		while ( !vars::b_setup )
			std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

		while ( !vars::b_close )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );

			//if window gone, close cheat
			if ( !FindWindowA( "UnityWndClass", "EscapeFromTarkov" ) )
				vars::b_close = true;
		}
	}

	void thread_hotkey_monitor()
	{
		while ( !vars::b_setup )
			std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

		while ( !vars::b_close )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

			//item esp = f1
			if ( GetAsyncKeyState( VK_F1 ) )
				vars::config::b_item_esp = !vars::config::b_item_esp;

			//extract esp = F2
			if ( GetAsyncKeyState( VK_F2 ) )
				vars::config::b_exfil_esp = !vars::config::b_exfil_esp;

			//skeleton ESP = F3
			if ( GetAsyncKeyState( VK_F3 ) )
				vars::config::b_skeleton = !vars::config::b_skeleton;
		}
	}

#define radiansToDegrees 57.29578f	

	Vector3D CalculateAngle( const Vector3D& local, const Vector3D& enemy )
	{
		Vector3D delta = local - enemy;
		//delta.y -= 180.0f;
		//delta.NormalizeAngles();

		float len = delta.Length();

		Vector3D ret;
		ret.y = asinf( delta.y / len ) * radiansToDegrees;
		ret.x = -atan2f( delta.x, -delta.z ) * radiansToDegrees;

		return ret;
	}

	inline float AngleFOV( const Vector3D& viewAngle, const Vector3D& aimAngle )
	{
		Vector3D delta = viewAngle - aimAngle;
		return fabsf( delta.Length() );
	}

	void run_aimbot( uint64_t local_player, uint64_t aimbot_target )
	{
		if ( !local_player
			|| !aimbot_target )
			return;

		//get player angles
		Player localPlayer = driver::read<Player>( local_player );

		c_player cPlayer( local_player );
		if ( cPlayer.get_destroyed() )
			return;

		MovementContext localMovement = driver::read<MovementContext>( (uintptr_t)localPlayer.m_pMovementContext );

		if ( cPlayer.get_destroyed() )
			return;

		Vector3D local_angles{};
		local_angles.x = localMovement.ViewAngles.x;
		local_angles.y = localMovement.ViewAngles.y;

		Vector3D local_player_head = GetBonePosition( local_player, Bones::HumanHead );

		if ( cPlayer.get_destroyed() )
			return;

		//calculate angle
		Vector3D enemy_head_pos = GetBonePosition( aimbot_target, Bones::HumanHead );

		if ( cPlayer.get_destroyed() )
			return;

		Vector3D aim_target_angle = CalculateAngle( local_player_head, enemy_head_pos );

		if ( cPlayer.get_destroyed() )
			return;

		//set angles
		if ( GetAsyncKeyState( 0x06 ) )
		{
			localMovement.ViewAngles.x = aim_target_angle.x;
			localMovement.ViewAngles.y = aim_target_angle.y;
			driver::write<MovementContext>( (uintptr_t)localPlayer.m_pMovementContext, localMovement );
		}
	}

	static bool b_update_players = false;
	static bool b_update_items = false;

	void thread_update()
	{
		while ( !vars::b_setup )
			std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

		while ( !vars::b_close )
		{
			//update item list once per second
			std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

			if ( !b_world_active )
			{
				v_items.clear();
				b_update_items = false;
				continue;
			}

			b_update_items = true;
		}
	}

	std::string GetUnicodeString( uint64_t addr, int stringLength )
	{
		char16_t wcharTemp[64] = { '\0' };
		driver::read_buffer( addr, stringLength * 2, wcharTemp );
		std::string u8_conv = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes( wcharTemp );
		return u8_conv;
	}

	//main cheat loop in overlay thread
	void main_loop()
	{
		overlay::draw_text( "rogue.rip", 1, 1, col_red, false, true );
		overlay::draw_text( "Raid status: " + std::to_string( b_world_active ), 1, 13, col_red, false, true );
		overlay::draw_text( "Item Esp: " + std::to_string( vars::config::b_item_esp ), 1, 25, col_red, false, true );
		overlay::draw_text( "Extract Esp: " + std::to_string( vars::config::b_exfil_esp ), 1, 37, col_red, false, true );
		overlay::draw_text( "Skeleton Esp: " + std::to_string( vars::config::b_skeleton ), 1, 49, col_red, false, true );

		if ( !vars::config::b_item_esp )
			v_items.clear();

		if ( !global_object_manager )
		{
			global_object_manager = driver::read<uint64_t>( base_address + 0x17FFD28 );
			if ( !global_object_manager )
				return;
		}

		g_local_player = c_player( 0 );

		world_update();
		if ( !b_world_active )
			return;

		uint64_t aimbot_target_address = 0;
		float f_best_distance = 9999.0f;

		//player esp
		{
			LOCALGAMEWORLD game_world = driver::read<LOCALGAMEWORLD>( local_game_world );
			if ( !game_world.m_pPlayerList )
				return;

			Array player_list = driver::read<Array>( (uint64_t)game_world.m_pPlayerList );
			if ( !player_list.Count )
				return;

			if ( !player_list.m_pList )
				return;

			std::vector<c_player> v_players{};
			for ( int i = 0; i != player_list.Count; i++ )
			{
				uint64_t player_address = driver::read<uint64_t>( (uint64_t)player_list.m_pList + (0x20 + (i * 8)) );
				if ( !player_address )
					continue;

				c_player player( player_address );
				if ( !player.entity )
					continue;

				player.f_health = player.get_health();
				if ( !player.f_health )
					continue;

				v_players.push_back( player );
			}

			if ( !v_players[0].entity )
				return;

			if ( v_players[0].get_destroyed() )
				return;

			if ( !v_players[0].get_health() )
				return;

			g_local_player = c_player( v_players[0] );
			Vector3D local_player_head = GetBonePosition( g_local_player.entity, Bones::HumanHead );

			for ( c_player player : v_players )
			{
				if ( g_local_player.get_destroyed() )
					return;

				Vector3D player_position_head = GetBonePosition( player.entity, Bones::HumanHead );

				if ( g_local_player.get_destroyed() )
					return;

				Vector3D player_position_root = GetBonePosition( player.entity, Bones::HumanBase );

				if ( g_local_player.get_destroyed() )
					return;

				float f_distance = local_player_head.Distance( player_position_head );
				if ( f_distance > vars::config::f_player_distance )
					continue;

				if ( g_local_player.get_destroyed() )
					return;

				Vector2D screen_head{};
				Vector2D screen_root{};
				if ( !player_position_head.x || !player_position_head.y )
					continue;

				float f_health = player.f_health;
				if ( !f_health )
					continue;

				if ( !World2Screen( player_position_head, &screen_head, local_camera ) )
					continue;

				if ( g_local_player.get_destroyed() )
					return;

				if ( !World2Screen( player_position_root, &screen_root, local_camera ) )
					continue;

				if ( g_local_player.get_destroyed() )
					return;

				if ( !screen_head.x || !screen_head.y || !screen_root.x || !screen_root.y )
					continue;

				//get closest to crosshair
				static Vector2D v_screen{ vars::i_screen_x / 2, vars::i_screen_y / 2 };
				float f_distance_screen = v_screen.Distance2D( screen_head );

				//check if aimbot target
				if ( f_distance_screen < f_best_distance && f_distance_screen < 500.0f )
				{
					f_best_distance = f_distance_screen;
					aimbot_target_address = player.entity;
				}

				if ( g_local_player.get_destroyed() )
					return;

				//skeleton test + skeleton fov
				if ( vars::config::b_skeleton && f_distance_screen < 80.0f )
				{
					if ( g_local_player.get_destroyed() )
						return;

					std::vector<int> upper_part = { Bones::HumanNeck, Bones::HumanHead };
					std::vector<int> right_arm = { Bones::HumanNeck, Bones::HumanRUpperarm, Bones::HumanRForearm1, Bones::HumanRPalm };
					std::vector<int> left_arm = { Bones::HumanNeck, Bones::HumanLUpperarm, Bones::HumanLForearm1, Bones::HumanLPalm };
					std::vector<int> spine = { Bones::HumanNeck, Bones::HumanSpine1, Bones::HumanSpine2, Bones::HumanPelvis };

					std::vector<int> lower_right = { Bones::HumanPelvis, Bones::HumanRThigh1, Bones::HumanRCalf, Bones::HumanRFoot };
					std::vector<int> lower_left = { Bones::HumanPelvis, Bones::HumanLThigh1, Bones::HumanLCalf, Bones::HumanLFoot };

					//std::vector<int> v_bones { Bones::HumanHead, Bones::HumanNeck };

					std::vector<std::vector<int>> v_vectors{ upper_part, right_arm, left_arm, spine, lower_right, lower_left };

					for ( int i_current_list = 0; i_current_list != v_vectors.size(); i_current_list++ )
					{
						for ( int i = 0; i != v_vectors[i_current_list].size(); i++ )
						{
							if ( i == 0 )
								continue;

							int i_bone = v_vectors[i_current_list][i];
							int i_bone_last = v_vectors[i_current_list][i - 1];

							Vector3D last_bone = GetBonePosition( player.entity, i_bone );
							Vector3D next_bone = GetBonePosition( player.entity, i_bone_last );

							Vector2D last_bone_screen{};
							Vector2D next_bone_screen{};

							if ( !World2Screen( last_bone, &last_bone_screen, local_camera ) )
								continue;

							if ( !World2Screen( next_bone, &next_bone_screen, local_camera ) )
								continue;

							overlay::draw_line( last_bone_screen.x, last_bone_screen.y, next_bone_screen.x, next_bone_screen.y, col_red );
						}
					}
				}

				screen_head.y -= 2;

				float height = screen_root.y - screen_head.y;
				float width = height / 2;

				if ( g_local_player.get_destroyed() )
					continue;

				//rect esp
				overlay::draw_outlined_rect( screen_head.x - (width / 2), screen_head.y, width, height, col_red );

				//health esp
				overlay::draw_text( "Health: " + std::to_string( f_health ), screen_head.x, screen_head.y - 12, col_red, true, false );

				//get team title
				std::string s_role_title = "null";
				int i_role = player.get_team();
				if ( i_role == 0 )
					s_role_title = "player";
				else if ( i_role == 1 )
					s_role_title = "player scav";
				else if ( i_role == 2 )
					s_role_title = "scav";

				if ( g_local_player.get_destroyed() )
					continue;

				//bot esp
				overlay::draw_text( s_role_title, screen_head.x, screen_head.y - 24, col_red, true, false );
			}
		}

		world_update();
		if ( !b_world_active )
			return;

		//extract ESP
		if ( vars::config::b_exfil_esp )
		{
			uint64_t exit_controller = driver::read<uint64_t>( local_game_world + 0x18 );
			if ( !exit_controller )
				return;

			//PMC extracts
			uint64_t exit_point = driver::read<uint64_t>( exit_controller + 0x20 );
			if ( !exit_point )
				return;

			int exit_count = driver::read<int>( exit_point + 0x18 );
			if ( !exit_count )
				return;

			if ( g_local_player.get_destroyed() )
				return;

			constexpr auto BUFFER_SIZE = 24;
			uint64_t extract_buffer[BUFFER_SIZE];
			driver::read_buffer( exit_point + offsetof( EFTStructs::ListInternal, firstEntry ), sizeof( uint64_t ) * exit_count, extract_buffer );
			if ( !extract_buffer )
				return;

			for ( int i = 0; i < exit_count; i++ )
			{
				uint64_t current_extract = extract_buffer[i];
				if ( !current_extract )
					continue;

				if ( g_local_player.get_destroyed() )
					return;

				//extract status
				int i_status = driver::read<int32_t>( current_extract + 0xA8 );
				if ( i_status != 4 )
					continue;

				if ( g_local_player.get_destroyed() )
					return;

				//extract position
				uint64_t transform = driver::read_chain( current_extract, { 0x10, 0x30, 0x30, 0x8, 0x28 } );
				if ( !transform )
					continue;

				if ( g_local_player.get_destroyed() )
					return;

				//extraction screen position
				Vector3D v_extract_location = GetPosition( transform );
				Vector2D v_extract_location_screen{};

				if ( !World2Screen( v_extract_location, &v_extract_location_screen, local_camera ) )
					continue;

				if ( g_local_player.get_destroyed() )
					return;

				//extract name
				uint64_t extract_name_address = driver::read_chain( current_extract, { 0x58, 0x10 } );
				if ( !extract_name_address )
					continue;

				if ( g_local_player.get_destroyed() )
					return;

				int32_t nameLength = driver::read<int32_t>( extract_name_address + 0x10 );
				if ( !nameLength )
					continue;

				if ( g_local_player.get_destroyed() )
					return;

				char16_t wcharTemp[64] = { '\0' };
				driver::read_buffer( extract_name_address + 0x0014, nameLength * 2, wcharTemp );
				if ( !wcharTemp )
					continue;

				if ( g_local_player.get_destroyed() )
					return;

				std::string s_name = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes( wcharTemp );
				if ( !s_name.size() )
					continue;

				overlay::draw_text( s_name, v_extract_location_screen.x, v_extract_location_screen.y, col_blue, true, false );
			}
		}

		world_update();
		if ( !b_world_active )
			return;

		//item esp
		if ( vars::config::b_item_esp )
		{
			Vector3D local_player_head = GetBonePosition( g_local_player.entity, Bones::HumanHead );

			//update items

			if ( b_update_items )
			{
				LOCALGAMEWORLD game_world = driver::read<LOCALGAMEWORLD>( local_game_world );
				if ( !game_world.m_pPlayerList )
					return;

				if ( g_local_player.get_destroyed() )
					return;

				LootArray item_list = driver::read<LootArray>( (uint64_t)game_world.m_pItemList );
				if ( !item_list.Count )
					return;

				if ( g_local_player.get_destroyed() )
					return;

				if ( !item_list.m_pItemList )
					return;

				v_items.clear();

				for ( int i = 0; i < item_list.Count; i++ )
				{
					if ( g_local_player.get_destroyed() )
						return;

					uint64_t itemAddr = driver::read<uint64_t>( (uint64_t)item_list.m_pItemList + (0x20 + (i * 8)) );
					if ( !itemAddr )
						continue;

					if ( g_local_player.get_destroyed() )
						return;

					c_item cItem( itemAddr );
					if ( !cItem.name.size() )
						continue;

					if ( g_local_player.get_destroyed() )
						return;

					cItem.position = cItem.get_position();
					if ( !cItem.position.x && !cItem.position.y )
						continue;

					if ( g_local_player.get_destroyed() )
						return;

					float f_distance = local_player_head.Distance( cItem.position );
					if ( !f_distance )
						continue;

					if ( g_local_player.get_destroyed() )
						return;

					float f_item_distance_req = 25.0f;
					if ( f_distance > f_item_distance_req )
						continue;

					v_items.push_back( cItem );
				}

				b_update_items = false;
			}

			if ( v_items.size() )
			{
				//draw items
				for ( c_item item : v_items )
				{
					if ( g_local_player.get_destroyed() )
						return;

					Vector2D v_position_screen{};
					if ( !World2Screen( item.position, &v_position_screen, local_camera ) )
						continue;

					if ( g_local_player.get_destroyed() )
						return;

					overlay::draw_text( item.name, v_position_screen.x, v_position_screen.y, col_green, true, false );
				}
			}
		}

		world_update();
		if ( !b_world_active )
			return;

		run_aimbot( g_local_player.entity, aimbot_target_address );

		world_update();
		if ( !b_world_active )
			return;

		//localplayer misc
		g_local_player.recoil_remove();
		g_local_player.infinite_stamina();
	}

	void thread_overlay()
	{
		//init background threads
		{
			if ( HANDLE handle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)thread_process_monitor, 0, 0, 0 ) )
				CloseHandle( handle );

			if ( HANDLE handle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)thread_hotkey_monitor, 0, 0, 0 ) )
				CloseHandle( handle );

			if ( HANDLE handle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)thread_update, 0, 0, 0 ) )
				CloseHandle( handle );
		}

		//init overlay window
		HWND win{};

		win = overlay::setup_nvidia();
		//win = overlay::setup_nvidia_desktop();
		//win = setup_non_overlay();
		//win = overlay::setup_debug_overlay();

		lg( "window setup" );

		static IDirect3D9Ex* p_Object = NULL;
		HWND windowid = NULL;
		RECT GameRect = { NULL };
		D3DPRESENT_PARAMETERS d3dpp;
		MSG Message = { NULL };

		Direct3DCreate9Ex( D3D_SDK_VERSION, &p_Object );

		ZeroMemory( &d3dpp, sizeof( d3dpp ) );
		d3dpp.BackBufferWidth = vars::i_screen_x;
		d3dpp.BackBufferHeight = vars::i_screen_y;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.hDeviceWindow = win;
		d3dpp.FullScreen_RefreshRateInHz = 0;
		d3dpp.Windowed = TRUE;

		p_Object->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, win, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &overlay::m_device );

		MSG m;
		ZeroMemory( &m, sizeof( m ) );

		D3DXCreateLine( overlay::m_device, &overlay::m_line );
		D3DXCreateFontA( overlay::m_device, 12, NULL, FW_HEAVY, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &overlay::m_font );

		lg( "overlay ready" );

		//init cheat seup
		if ( HANDLE handle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)Tarky::setup, 0, 0, 0 ) )
		{
			CloseHandle( handle );
		}

		while ( m.message != WM_QUIT )
		{
			ZeroMemory( &m, sizeof( MSG ) );
			while ( PeekMessage( &m, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &m );
				DispatchMessage( &m );
			}

			std::this_thread::sleep_for( std::chrono::milliseconds( 2 ) );

			while ( !vars::b_setup )
				std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

			//Start scene
			{
				overlay::m_device->Clear( NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.f, 0 );
				overlay::m_device->BeginScene();
			}

			main_loop();

			//End scene
			{
				overlay::m_device->EndScene();
				overlay::m_device->Present( NULL, NULL, NULL, NULL );
			}
		}
		p_Object->Release();
	}
}

//debug entrypoint
int main()
{
	SetConsoleTitleA( "" );
	std::string in_username, in_password;
	
	if ( !framework::api::auth( in_username, in_password ) )
	{
		lg( "auth sad :(" );
		Sleep( 3000 );
		return 0;
	}
	lg( "auth happy :)" );

	//start driver
	if ( driver::load() != 0 )
	{
		lg( "driver too tired to wake up :(" );
		Sleep( 1000 );
		return 0;
	}

	Sleep( 5000 );

	//ping driver
	driver::ping( 1 );
	lg( "driver awake" );

	//be+bsg spoofer
	driver::spoofer( true );
	framework::util::spoofer();
	lg( "Spoofer ran" );

	//start cheat
	if ( HANDLE handle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)Tarky::thread_overlay, 0, 0, 0 ) )
		CloseHandle( handle );

	while ( !vars::b_close )
		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

	//end driver
	lg( "putting driver down" );
	driver::unload();
	lg( "driver euthanized" );
	Sleep( 3500 );
	return 1337;
}
