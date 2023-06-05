#pragma once
#include "include.hpp"
#include "variables.hpp"

#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>


namespace overlay
{
	struct D3DTLVERTEX
	{
		float x, y, z, rhw;
		unsigned long color;
	};

	IDirect3DDevice9 *m_device = nullptr;
	ID3DXLine *m_line = nullptr;
	ID3DXFont *m_font = nullptr;

	void draw_line( int x0, int y0, int x1, int y1, unsigned long color )
	{
		D3DXVECTOR2 lines[2] = {
			D3DXVECTOR2( x0, y0 ),
			D3DXVECTOR2( x1, y1 )
		};

		m_line->Begin();
		m_line->Draw( lines, 2, color );
		m_line->End();
	}

	void draw_rect( int x, int y, int w, int h, unsigned long color )
	{
		draw_line( x, y, x + w, y, color );
		draw_line( x, y, x, y + h, color );
		draw_line( x + w, y, x + w, y + h, color );
		draw_line( x, y + h, x + w + 1, y + h, color );
	}

	void draw_filled_rect( int x, int y, int w, int h, unsigned long color )
	{
		D3DTLVERTEX qV[4] = {
			{ float( x ), float( y + h ), 0.f, 1.f, color },
			{ float( x ), float( y ), 0.f, 1.f, color },
			{ float( x + w ), float( y + h ), 0.f, 1.f, color },
			{ float( x + w ), float( y ), 0.f, 1.f, color }
		};

		m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
		m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		m_device->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		m_device->SetTexture( 0, nullptr );
		m_device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, qV, sizeof( D3DTLVERTEX ) );
	}

	void draw_outlined_rect( int x, int y, int w, int h, unsigned long color )
	{
		draw_rect( x - 1, y - 1, w + 2, h + 2, D3DCOLOR_RGBA( 1, 1, 1, 255 ) );
		draw_rect( x + 1, y + 1, w - 2, h - 2, D3DCOLOR_RGBA( 1, 1, 1, 255 ) );
		draw_rect( x, y, w, h, color );
	}

	RECT get_text_dimensions( std::string_view text )
	{
		RECT r;
		m_font->DrawTextA( NULL, text.data(), -1, &r, DT_CALCRECT, 0xFFFFFFFF );
		return r;
	}

	void draw_text( std::string_view text, int x, int y, unsigned long color, bool center, bool outline )
	{
		if ( center )
		{
			RECT dimensions = get_text_dimensions( text );
			x -= (dimensions.right - dimensions.left) / 2;
		}

		auto _text = [&]( std::string_view _text, int _x, int _y, unsigned long _color )
		{
			RECT r { _x, _y, _x, _y };
			m_font->DrawTextA( NULL, _text.data(), -1, &r, DT_NOCLIP, _color );
		};

		if ( outline )
		{
			_text( text, x - 1, y, D3DCOLOR_RGBA( 1, 1, 1, 255 ) );
			_text( text, x + 1, y, D3DCOLOR_RGBA( 1, 1, 1, 255 ) );
			_text( text, x, y - 1, D3DCOLOR_RGBA( 1, 1, 1, 255 ) );
			_text( text, x, y + 1, D3DCOLOR_RGBA( 1, 1, 1, 255 ) );
		}

		_text( text, x, y, color );
	}

	LRESULT CALLBACK m_wnd_proc( HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam )
	{
		switch ( msg )
		{
			case WM_DESTROY:
				exit( 0 );
				break;
			default:
				return DefWindowProc( wnd, msg, wparam, lparam );
		}
	}

	struct wnd_rect_t : public RECT
	{
		int width()
		{
			return right - left;
		}
		int height()
		{
			return bottom - top;
		}
	};

	HWND setup_debug_overlay()
	{
		HWND h_overlay_wnd {};

		HWND h_target_wnd = FindWindowA( "UnityWndClass", "EscapeFromTarkov" );
		if ( !h_target_wnd )
			return 0;

		wnd_rect_t r_target {};
		GetWindowRect( h_target_wnd, &r_target );

		WNDCLASSEX wc;
		wc.cbSize = sizeof( wc );

		//Create our window class
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = m_wnd_proc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = NULL;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );
		wc.hbrBackground = HBRUSH( RGB( 0, 0, 0 ) );
		wc.lpszMenuName = "";
		wc.lpszClassName = "test";
		wc.hIconSm = NULL;

		//Register our window class
		if ( !RegisterClassExA( &wc ) )
			throw std::exception( "dx_overlay::create_overlay: failed to register wndclassex" );

		//Get the size of our target window
		GetWindowRect( h_target_wnd, &r_target );

		//Make the window transparent
		DWORD ex_styles = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST;

		//Create our window
		h_overlay_wnd = CreateWindowExA( ex_styles, "test", "", WS_POPUP | WS_VISIBLE,
			 r_target.left, r_target.top, r_target.width(), r_target.height(), NULL, NULL, NULL, NULL);

		if ( !h_overlay_wnd )
			return 0;

		//Let DWM handle our window
		MARGINS m = { r_target.left, r_target.top, r_target.width(), r_target.height() };
		DwmExtendFrameIntoClientArea( h_overlay_wnd, &m );

		//Set window to use alpha channel
		SetLayeredWindowAttributes( h_overlay_wnd, RGB( 0, 0, 0 ), 255, LWA_ALPHA );

		//Show our window
		ShowWindow( h_overlay_wnd, SW_SHOW );
		return h_overlay_wnd;
	}

	HWND setup_non_overlay()
	{
		//wnd_rect_t r_target {};
		//GetWindowRect( h_target_wnd, &r_target );

		WNDCLASSEX wc;
		wc.cbSize = sizeof( wc );

		//Create our window class
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = m_wnd_proc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = NULL;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );
		wc.hbrBackground = HBRUSH( RGB( 0, 0, 0 ) );
		wc.lpszMenuName = "";
		wc.lpszClassName = "test";
		wc.hIconSm = NULL;

		//Register our window class
		if ( !RegisterClassExA( &wc ) )
			throw std::exception( "dx_overlay::create_overlay: failed to register wndclassex" );

		//Get the size of our target window
		//GetWindowRect( h_target_wnd, &r_target );

		//Make the window transparent
		//DWORD ex_styles = WS_EX_LAYERED | WS_EX_TRANSPARENT;		
		DWORD ex_styles {};

		//Create our window
		HWND h_overlay_wnd = CreateWindowExA( ex_styles, "test", "", WS_POPUP | WS_VISIBLE,
			5, 5, 640, 360, NULL, NULL, NULL, NULL);

		if ( !h_overlay_wnd )
			return 0;

		//Let DWM handle our window
		//MARGINS m = { r_target.left, r_target.top, r_target.width(), r_target.height() };
		//DwmExtendFrameIntoClientArea( h_overlay_wnd, &m );

		//Set window to use alpha channel
		//SetLayeredWindowAttributes( h_overlay_wnd, RGB( 0, 0, 0 ), 255, LWA_ALPHA );

		//Show our window
		ShowWindow( h_overlay_wnd, SW_SHOW );
		return h_overlay_wnd;
	}

	HWND setup_nvidia_desktop()
	{
		HWND win {};
		HWND old_win = FindWindowA( "CEF-OSC-WIDGET", "NVIDIA GeForce Overlay" );
		ShowWindow( old_win, SW_HIDE );

		win = FindWindowA("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay DT");
			
		ShowWindow(win, SW_SHOW);
		/*
		SetWindowLongA(win, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);

		SetWindowLongW(
			win,
			-20,
			static_cast<LONG_PTR>(
				static_cast<int>(GetWindowLongW(win, -20)) | 0x20
				)
		);

		//transparency
		MARGINS margin = {-1, -1, -1, -1};
		DwmExtendFrameIntoClientArea(
			win,
			&margin
		);

		SetLayeredWindowAttributes(
			win,
			NULL,
			0xFF,
			0x02
		);
		*/
		// top most
		SetWindowPos(
			win,
			HWND_TOPMOST,
			0, 0, vars::i_screen_x, vars::i_screen_y,
			0x0002 | 0x0001
		);
		
		UpdateWindow(win);
		return win;
	}

	HWND setup_nvidia()
	{
		HWND win {};
		win = FindWindowA("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay");
			
		ShowWindow(win, SW_SHOW);

		SetWindowLongA(win, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);

		SetWindowLongW(
			win,
			-20,
			static_cast<LONG_PTR>(
				static_cast<int>(GetWindowLongW(win, -20)) | 0x20
				)
		);

		//transparency
		MARGINS margin = {-1, -1, -1, -1};
		DwmExtendFrameIntoClientArea(
			win,
			&margin
		);

		SetLayeredWindowAttributes(
			win,
			NULL,
			0xFF,
			0x02
		);

		// top most
		SetWindowPos(
			win,
			HWND_TOPMOST,
			0, 0, vars::i_screen_x, vars::i_screen_y,
			0x0002 | 0x0001
		);
		
		UpdateWindow(win);
		return win;
	}
}
