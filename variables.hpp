#pragma once
#include "include.hpp"

namespace vars
{
	class esp_player
	{
	public:
		float x, y, w, h;
		bool b_bot = false;
		float f_health = 0.0f;

		esp_player( float _x, float _y, float _w, float _h, float _health = 0.0f, bool _bot = false )
		{
			x = _x;
			y = _y;
			w = _w;
			h = _h;
			f_health = _health;
			b_bot = _bot;
		}

		esp_player()
		{
			x, y, w, h = 0;
		}
	};

	class esp_item
	{
	public:
		float x, y, w, h;
		std::string s_name = "";

		esp_item( float _x, float _y, float _w, float _h, std::string _name = "" )
		{
			x = _x;
			y = _y;
			w = _w;
			h = _h;
			s_name = _name;
		}

		esp_item()
		{
			x, y, w, h = 0;
		}
	};

	//api
	static int i_version = 1;	

	//runtime
	static bool b_setup = false;
	static bool b_close = false;	
	static bool b_in_game = false;

	//runtime_setup
	static uint64_t u_base_address = 0;	
	static uint64_t u_game_object_manager = 0;
	static uint64_t u_client_game_world = 0;
	static uint64_t u_camera_fps = 0;

	//setup settings
	static int i_screen_x = 1920;
	static int i_screen_y = 1080;

	//menu
	namespace menu
	{
		static int i_current_point = 0;
		static bool b_menu = false;
		static bool b_tab_aimbot = false;
		static bool b_tab_esp = false;
		static bool b_tab_misc = false;
	}

	namespace config
	{
		//aimbot
		static bool b_aimbot = false;

		//esp
		static bool b_player_esp = true;
		static bool b_player_health = true;
		static bool b_skeleton = false;
		static float f_player_distance = 200.0f;
		static bool b_item_esp = false;
		static float f_item_distance = 50.0f;
		static bool b_exfil_esp = false;

		//misc
		static bool b_infinite_stamina = true;
		static bool b_remove_recoil = true;
	}
}