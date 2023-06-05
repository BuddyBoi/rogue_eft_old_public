#pragma once
#include "include.hpp"

class FVector;
class FRotator;
class Vector3;

float DegToRad( float x );
float RadToDeg( float x );
float DistancePointToLine( FVector Point, FVector LineOrigin, FVector Dir );

int length( uintptr_t addr )
{
	return framework::driver::read<int>( addr + 0x10 );
}

std::string readstring( uintptr_t addr )
{
	try
	{
		static char buff[128] = { 0 };
		buff[length( addr )] = '\0';

		for ( int i = 0; i < length( addr ); ++i )
		{
			if ( buff[i] < 128 )
			{
				buff[i] = framework::driver::read<char>( addr + 0x14 + (i * 2) );
			}
			else
			{
				buff[i] = '?';
				if ( buff[i] >= 0xD800 && buff[i] <= 0xD8FF )
					i++;
			}
		}
		return std::string( &buff[0], &buff[length( addr )] );
	}
	catch ( const std::exception &exc )
	{
	}
	return "Error";
}

struct monostr
{
	char buffer[128];
};

std::string readchar( uintptr_t addr )
{
	std::string str = framework::driver::read<monostr>( addr ).buffer;
	if ( !str.empty() )
		return str;
	else
		return "";
}

struct unity_string
{
	char buffer[256];
};

enum Bones : int
{
	HumanBase = 0,
	HumanPelvis = 14,
	HumanLThigh1 = 15,
	HumanLThigh2 = 16,
	HumanLCalf = 17,
	HumanLFoot = 18,
	HumanLToe = 19,
	HumanRThigh1 = 20,
	HumanRThigh2 = 21,
	HumanRCalf = 22,
	HumanRFoot = 23,
	HumanRToe = 24,
	HumanSpine1 = 29,
	HumanSpine2 = 36,
	HumanSpine3 = 37,
	HumanLCollarbone = 89,
	HumanLUpperarm = 90,
	HumanLForearm1 = 91,
	HumanLForearm2 = 92,
	HumanLForearm3 = 93,
	HumanLPalm = 94,
	HumanRCollarbone = 110,
	HumanRUpperarm = 111,
	HumanRForearm1 = 112,
	HumanRForearm2 = 113,
	HumanRForearm3 = 114,
	HumanRPalm = 115,
	HumanNeck = 132,
	HumanHead = 133
};

namespace EFTStructs
{
	struct BaseObject
	{
		uint64_t previousObjectLink; //0x0000
		uint64_t nextObjectLink; //0x0008
		uint64_t object; //0x0010
	};

	struct GameObjectManager
	{
		uint64_t lastTaggedNode; // 0x0
		uint64_t taggedNodes; // 0x8
		uint64_t lastMainCameraTaggedNode; // 0x10
		uint64_t mainCameraTaggedNodes; // 0x18
		uint64_t lastActiveNode; // 0x20
		uint64_t activeNodes; // 0x28
	};
	class ListInternal
	{
	public:
		char pad_0x0000[0x20]; //0x0000
		uintptr_t* firstEntry; //0x0020 
	}; //Size=0x0028

	class List
	{
	public:
		char pad_0x0000[0x10]; //0x0000
		ListInternal* listBase; //0x0010 
		__int32 itemCount; //0x0018 
	}; //Size=0x001C
}

uint64_t ReadChain( uint64_t base, const std::vector<uint64_t>& offsets )
{
	uint64_t result = framework::driver::read<uint64_t>( base + offsets.at( 0 ) );
	
	for ( int i = 1; i < offsets.size(); i++ )
	{
		result = framework::driver::read<uint64_t>( result + offsets.at( i ) );
		if ( !result )
			return 0;
	}
	return result;
}

enum ScriptingGHandleWeakness
{
	GCHANDLE_INVALID = 0,
	GCHANDLE_WEAK = 1,
	GCHANDLE_STRONG = 2
};