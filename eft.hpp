#pragma once
#include "include.hpp"

class LOCALGAMEWORLD // Aka LocalGameWorld
{
public:
    /*
    char pad_0000[24]; //0x0000
    class ExitController* m_pExitController; //0x0018
    char pad_0020[24]; //0x0020
    class UnityEngineString* m_LocalPlayerID; //0x0038
    char pad_0038[24]; //0x0040
     */
    char pad58[0x68];
    class LootArray *m_pItemList; //0x0068
    char pad_18[0x18]; //0x0070
    class Array *m_pPlayerList; //0x0088
}; //Size: 0x0078

class Array
{
public:
    char pad_0000[0x10]; //0x0000
    class List *m_pList; //0x0010
    int32_t Count; //0x0018
    int32_t MaxCount; //0x001C
}; //Size: 0x0020

class List
{
public:
    char pad_0000[0x20]; //0x0000
    class Player *m_pFirstEntity; //0x0020
    class Player *m_pSecondPlayer; //0x0028
}; //Size: 0x0030

class WeaponTemplate
{
public:
    char _pad[0x24C];
    bool m_allowJam;
    bool m_allowFeed;
    bool m_allowMisfire;
    bool m_allowSlide;
};
class WeaponItem
{
public:
    char _pad[0x10];
    class UnityEngineString *m_Id;      // 0x10
    class ItemAddress *m_itemAddress;   // 0x18
    class Components *m_components;     // 0x20
    class StringCache *m_stringCache;   // 0x28
    class ItemAddress *m_currentAddress;// 0x30
    class asdfasdf *m_ChildrenChanged;  // 0x38
    class WeaponTemplate *m_BackingField; // 0x40 //EFT.InventoryLogic.WeaponTemplate
    class List *m_Attributes;   // 0x48
    class Nullable *m_BackingField2;// 0x50
    uint32_t m_bUnlimitedCount;         // 0x58
    int32_t m_BuyRestrictionMax;        // 0x5C
    int32_t m_BuyRestrictionCurrent;    // 0x60
    int32_t m_StackObjectsCount;        // 0x64
    int32_t m_Version;                  // 0x68
    int32_t m_SpawnedInSession;         // 0x6C
    /*
    class Grid *m_Grids;                // 0x70
    class Slot *m_Slots;                // 0x78
    class RepairableComponent *m_Repairable; // 0x80
    class FoldableComponent *m_Foldable;// 0x88
    class FireModeComponent *m_FireMode;// 0x90
    class Slot *m_Chambers;             // 0x98 [pointer to Array]
    class Vector3D *m_OpticCalibrationPts;// 0xA0
    class Vector3D *m_OpticTrajInfos;   // 0xA8
    class asdfff *m_MalfState;          // 0xB0
    class Slot *m_magSlotCache;         // 0xB8*/
    // aimindex
    //onmalfunctionvalidate
    //shellsinchambers
    //compatibleammo
    //armed
    //cylinderhammerclosed
};
class HandsController
{
public:
    char pad_60[0x60];
    class WeaponItem *m_pWeaponItem; // 0x60       EFT.InventoryLogic.Item
};
class Player
{
public:
    char pad_0000[24]; //0x0000
    class LocalPlayerChecker *m_pLocalPlayerChecker; //0x0018
    char pad_0020[32]; //0x0020
    class MovementContext *m_pMovementContext; //0x0040
    char pad_60[96];
    class PlayerBody *m_pPlayerBody; //0x00A8
    char pad_c0[232];
    class ProceduralWeaponAnimation *m_pProceduralWeaponAnimation; //0x0198
    char pad_270[672];
    char pad_a0[0xB0];
    class PlayerProfile *m_pPlayerProfile; //0x04F0
    char pad_028[0x30];
    class HealthController *m_pHealthController; //0x0528
    char _paderino[0x18];
    class HandsController *m_pHandsController; // 0x0540
};

struct game_object_wrapper
{
    char pad_0[0x18];
    void *gameObject;
};

struct mono_object
{
    char pad_0000[0x30];
    game_object_wrapper *pObjectClass;
    char pad_0038[0x10];
    int16_t Unk;
    char pad_004A[6];
    int32_t Layer;
    int16_t Tag;
    char pad_0056[10];
    char *objectname;
};

struct GameWorldWrapper
{
    char pad_0[0x28];
    LOCALGAMEWORLD *localgameworld;
};

struct mono_object_wrapper
{
    char pad_0[0x8];
    mono_object_wrapper *next;
    mono_object *object;
};

class MovementContext
{
public:
    char pad_0000[0x22C]; //0x0000
    Vector2D ViewAngles;
    Vector2D ViewAngles2;
    Vector3D LocalPosition;
}; //Size: 0x01FC

class CAMERA // Aka FPS Camera
{
public:
    char pad_0000[48]; //0x0000
    class FPSCamera *m_pFpsCamera; //0x0030
}; //Size: 0x0038

class FPSCamera
{
public:
    char pad_0000[48]; //0x0000
    class CameraObject *m_pCameraObject; //0x0030
    char pad_0038[40]; //0x0038
    char *ObjectName; //0x0060
    char pad_0068[8]; //0x0068
    class OpticCameraManager *m_pOpticCameraManager; // 0x0070
}; //Size: 0x0068

class CameraObject
{
public:
    char pad_0000[8]; //0x0000
    class CameraContainer *m_pCameraContainer; //0x0008
    char pad_0010[8]; //0x0010
    class CameraEntity *m_pCameraEntity; //0x0018
}; //Size: 0x0020

class OpticCameraManager
{
public:
    char pad_0000[16]; //0x0000
    class TemplateCamera *m_pTemplateCamera; // 0x0010 aka ScopeCamera
    char pad_0001[16]; //0x0018
    class OpticSight *m_pOpticSight; // 0x0028
};

class TemplateCamera
{
public:
    char pad_0000[48]; //0x0000
    class CameraObject *m_pCameraObject; //0x0030
};

class CameraEntity
{
public:
    char pad_0000[0xDC];
    Matrix4x4 ViewMatrix; //0x00D8
    char pad_00E0[64]; //0x00E0
    float FOVY; // 0x0098
    char pad_009C[20]; //0x009C
    float FOVX; // 0x00AC
}; //Size: 0x0118

class PlayerBody
{
public:
    char pad_0000[40]; //0x0000
    class m_pSkeletonRootJoin *m_pSkeletonRootJoin; //0x0028
}; //Size: 0x0030

class m_pSkeletonRootJoin
{
public:
    char pad_0000[40]; //0x0000
    class BoneEnumerator *m_pBoneEnumerator; //0x0028
}; //Size: 0x0030

class BoneEnumerator
{
public:
    char pad_0000[16]; //0x0000
    class TransformArray *m_pTransformArray; //0x0010
}; //Size: 0x0018

class TransformArray
{
public:
    char pad_0000[8]; //0x0000
}; //Size: 0x0008

class PlayerProfile
{
public:
    char pad_0000[16]; //0x0000
    class UnityEngineString *m_pPlayerId; //0x0010
    char pad_0018[16]; //0x0018
    class PlayerInfo *m_PlayerInfo; //0x0028
    char pad_0030[16]; //0x0030
    class PlayerHealth *m_pHealth; //0x0040
    class Inventory *m_pInventory; //0x0048
    void *pad;                     //0x0050
    void *pad2;                    //0x0058
    void *skills;                  //0x0060
}; //Size: 0x0050

class PlayerInfoSettings
{
public:
    char pad[0x10];
    int role;          // 0x10
    int botDifficulty; // 0x14
};
class PlayerInfo
{
public:
    char pad_0000[0x10]; //0x0000
    class UnityEngineString *m_pPlayerName; // 0x10
    class UnityEngineString *m_pNickname;   // 0x18
    class UnityEngineString *m_pGroupId;    // 0x20
    class UnityEngineString *m_pEntryPoint; // 0x28
    class UnityEngineString *m_pVoice;      // 0x30
    void *list;
    void *pad;
    class PlayerInfoSettings *settings;     // 0x48
    void *action1;                          // 0x50
    void *action2;                          // 0x58
    void *action3;                          // 0x60
    int32_t Side;                           // 0x68
    int32_t CreationDate;                   // 0x6c
    double savageLockTime;
    int64_t nicknameChangeDate;
    int isStreamerModeAvailable;
    int memberCategory;
}; //Size: 0x0058

class UnityEngineString
{
public:
    char pad_0000[16]; //0x0000
    int32_t size; //0x0010
    //wchar_t name[24]; //0x0014
    char16_t name[16]; //0x0014
}; //Size: 0x0028

class HealthController // _healthController
{
public:
    char pad_0000[0x50]; //0x0000
    class HealthBody *m_pHealthBody;
}; //Size: 0x0020

class HealthBody
{
public:
    char pad_0000[0x18]; //0x0000
    class BodyController *m_pBodyController; //0x0018
}; //Size: 0x0020

enum BodyParts_t
{
    HEAD = 0,
    THORAX,
    STOMACH,
    LEFTARM,
    RIGHTARM,
    LEFTLEG,
    RIGHTLEG,
    NUM_BODY_PARTS
};

class BodyPartContainer
{
public:
    char _pad[0x10];
    class HealthContainer *m_pBodyPart;
};
class BodyController
{
public:
    char pad_0000[0x20]; //0x0000

    BodyPartContainer m_bodyParts[NUM_BODY_PARTS];
}; //Size: 0x00C8

class HealthContainer
{
public:
    char pad_0000[0x10]; //0x0000
    class Health *m_pHealth; //0x0010
}; //Size: 0x0018

class Health
{
public:
    char pad_0000[0x10]; //0x0000
    float Health; //0x0010
    float HealthMax; //0x0014
}; //Size: 0x0018

class PlayerHealth
{
public:
    char pad_0000[24]; //0x0000
    class HealthInfo *m_playerHealthInfo; //0x0018
}; //Size: 0x0020

class HealthInfo
{
public:
    char pad_0000[16]; //0x0000
    float CurentHealth; //0x0010
    float MaxHealth; //0x0014
}; //Size: 0x0018

class CameraContainer
{
public:
    char pad_0000[56]; //0x0000
    class CameraPosition *m_pCameraPosition; //0x0038
}; //Size: 0x0040

class CameraPosition
{
public:
    char pad_0000[176]; //0x0000
    Vector3D cameraPosition; //0x00B0
    char pad_00BC[4]; //0x00BC
    Vector3D cameraRotation; //0x00C0
}; //Size: 0x00D0

class LootArray
{
public:
    char pad_0000[0x10]; //0x0000
    class ItemList *m_pItemList; //0x0010
    int32_t Count; //0x0018
    int32_t MaxCount; //0x001C
}; //Size: 0x0020

class ItemList
{
public:
    char pad_0000[0x20]; //0x0000
    class Item *m_pFirstItem; //0x0020
    class Item *m_pSecondItem; //0x0028
}; //Size: 0x0030

class Item
{
public:
    char pad_0000[0x10]; //0x0000
    class ItemProfile *m_pItemProfile; //0x0010
}; //Size: 0x0018

class ItemProfile
{
public:
    char pad_0000[0x28]; //0x0000
    class ItemStats *m_pItemStats; //0x0028
    class ItemBasicInformation *m_pItemInformation; //0x0030
}; //Size: 0x0038S

class ItemBasicInformation
{
public:
    char pad_0000[48]; //0x0000
    class ItemLocalization *m_pItemLocalization; //0x0030
    char pad_0038[40]; //0x0038
    char *ItemPatName; //0x0060
}; //Size: 0x0068

class ItemLocalization
{
public:
    char pad_0000[8]; //0x0000
    class ItemCoordinates *m_pItemCoordinates; //0x0008
}; //Size: 0x0010

class ItemCoordinates
{
public:
    char pad_0000[0x38]; //0x0000
    class ItemLocationContainer *m_pItemLocationContainer; //0x0038
}; //Size: 0x0040

class ItemLocationContainer
{
public:
    char pad_0000[0x90]; //0x0000
    Vector3D ItemPosition;
}; //Size: 0x00BC

class Inventory
{
public:
    char pad_0000[48]; //0x0000
    class FastAccess *m_pFastAccess; //0x0030
}; //Size: 0x0038

class FastAccess
{
public:
    char pad_0000[16]; //0x0000
    class BoundCells *m_pBoundCells; //0x0010
}; //Size: 0x0018

class BoundCells
{
public:
    char pad_0000[24]; //0x0000
    class CollectionEntry *Entries; //0x0018
    char pad_0020[32]; //0x0020
    int32_t Count; //0x0040
    int32_t Max; //0x0044
}; //Size: 0x0048

class m_pDictionaryItems
{
public:
    char pad_0000[32]; //0x0000
    class Weapon *Knife; //0x0020
    class Weapon *Pistol; //0x0028
    class Weapon *FirstWeapon; //0x0030
    class Weapon *SecondWeapon; //0x0038
    char pad_0040[64]; //0x0040
}; //Size: 0x0080

class Weapon
{
public:
    char pad_0000[32]; //0x0000
    class WeaponDetails *m_pWeaponDetails; //0x0020
}; //Size: 0x0028

class WeaponDetails
{
public:
    char pad_0000[32]; //0x0000
    class WeaponNameInfo *m_pWeaponNameInfo; //0x0020
}; //Size: 0x0028

class WeaponNameInfo
{
public:
    char pad_0000[16]; //0x0000
    class UnityEngineString *Name; //0x0010
    char pad_0018[16]; //0x0018
    class UnityEngineString *NameType; //0x0028
}; //Size: 0x0030

class FirearmController
{
public:
    char _padd[0x60];
    class WeaponItem *m_pItem;
    char _pad[0xD8];
    float m_hipInaccuracy; // 0x140
    float m_weaponLn; // weapon length
};

class ProceduralWeaponAnimation
{
public:
    char pad_0000[0x28]; //0x0000
    class BreathEffector *m_pBreath;         //0x0028
    class WalkEffector *m_pWalk;             // 0x30
    class MotionEffector *m_pMotionReact;    // 0x38
    class ForceEffector *m_pForceReact;      // 0x40
    class ShotEffector *m_pShooting;         // 0x48
    class TurnAwayEffector *m_pTurnaway;     // 0x50
    char pad_0050[0x28];                     //0x0058
    class FirearmController *m_pFiring;      //0x0080
    char pad_0088[0x78];                     //0x0088
    uint32_t mask; //0x0100
    float sway1;
    float sway2;
    float sway3;
}; //Size: 0x01D8

class ItemStats
{
public:
    char pad_0000[0xB0]; //0x0000
    class GameItem *m_pGameItem;
}; //Size: 0x0058

class StatsNames
{
public:
    char pad_0000[0x40]; //0x0000
    class ItemNames *m_pItemNames; //0x0040
}; //Size: 0x0028

class ItemNames
{
public:
    char pad_0000[80]; //0x0000
    class UnityEngineString *m_pItemHashIdentifier; //0x0050
    char pad_0058[8]; //0x0058
    class UnityEngineString *m_pParentHashIdentifier; //0x0060
}; //Size: 0x0068

class OpticSight
{
public:
    char pad_0000[40]; //0x0000
    class UnityCamera *m_pCamera; //0x0020
}; //Size: 0x0028

class UnityCamera
{
public:
    char pad_0000[40]; //0x0000
    class CameraRaw *m_pCameraRaw; //0x0028
}; //Size: 0x0030

class BreathEffector
{
public:
    char pad_0000[0xA0]; //0x0000
    bool IsAiming; //0x00A0
    char pad_0091[3];
    float Intensity; //0x00A4
}; //Size: 0x0098

class ShotEffector
{
public:
    char pad_0000[0x6C]; //0x0000
    float padyolo;
    float recoilHori;
    float recoilVert;
    float Intensity4;
    float Intensity5;
    float Intensity6;
    float Intensity7;
}; //Size: 0x006C

class CameraRaw
{
public:
    char pad_0000[48]; //0x0000
    class CameraEntity *m_pCameraEntity; //0x0030
}; //Size: 0x0038

class PlayerId
{
public:
    char pad_0000[16]; //0x0000
    int32_t N000007E4; //0x0010
    wchar_t N000007F5[24]; //0x0014
    char pad_0044[112]; //0x0044
}; //Size: 0x00B4

class LocalPlayerChecker
{
public:
    char pad_0000[8]; //0x0000
}; //Size: 0x0008

class ExitController
{
public:
    char pad_0000[32]; //0x0000
    class ExitPoint *m_pExitPoint; //0x0020
}; //Size: 0x0028

class ExitPoint
{
public:
    char pad_0000[24]; //0x0000
    int32_t ExitCount; //0x0018
    char pad_001C[4]; //0x001C
    class Extraction *m_pFirstEntry; //0x0020
    class Extraction *m_pSecondEntry; //0x0028
}; //Size: 0x0030

class Extraction
{
public:
    char pad_0000[16]; //0x0000
    class PositionStuff *m_pPositionStuff; //0x0010
    char pad_0018[64]; //0x0018
    class ExtractionName *m_pExtractName; //0x0058
}; //Size: 0x0060

class ExtractionName
{
public:
    char pad_0000[16]; //0x0000
    class UnityEngineString *Name; //0x0010
}; //Size: 0x0018

class PositionStuff
{
public:
    char pad_0000[48]; //0x0000
    class PositionStuff2 *m_pPositionStuff2; //0x0030
}; //Size: 0x0038

class PositionStuff2
{
public:
    char pad_0000[48]; //0x0000
    class PositionStuff3 *m_pPositionStuff3; //0x0030
}; //Size: 0x0038

class PositionStuff3
{
public:
    char pad_0000[8]; //0x0000
    class ExtractionTransforms *m_pExtractionTransform; //0x0008
}; //Size: 0x0010

class ExtractionTransforms
{
public:
    char pad_0000[40]; //0x0000
    class ExtranctionTransformPointer *m_pExtractionTransform; //0x0028
}; //Size: 0x0030

class ExtranctionTransformPointer
{
public:
    char pad_0000[16]; //0x0000
    class Transform *m_pTransform; //0x0010
}; //Size: 0x0018

class Transform
{
public:
    char pad_0000[40]; //0x0000
    Matrix4x4 ExitMatrix; //0x0028
}; //Size: 0x0068

class CollectionEntry
{
public:
    char pad_0000[48]; //0x0000
    class InventoryLogicSlot *m_pKnife; //0x0030
    char pad_0038[16]; //0x0038
    class InventoryLogicSlot *m_pPistol; //0x0048
    char pad_0050[16]; //0x0050
    class InventoryLogicSlot *m_pFirstWeapon; //0x0060
    char pad_0068[16]; //0x0068
    class InventoryLogicSlot *m_pSecondWeapon; //0x0078
}; //Size: 0x0080

class InventoryLogicSlot
{
public:
    char pad_0000[24]; //0x0000
    class DizBindingBindableState *m_pReactiveContainedItem; //0x0018
}; //Size: 0x0020

class DizBindingBindableState
{
public:
    char pad_0000[16]; //0x0000
    class GameItem *m_pGameItem; //0x0010
}; //Size: 0x0018

class GameItem
{
public:
    char pad_0000[32]; //0x0000
    class ItemTemplate *m_pItemTemplate; //0x0020
}; //Size: 0x0028

///           ///
/// Functions ///
///           ///

uint64_t matrix_list_base;
uint64_t dependency_index_table_base = 0;

static Vector3D GetPosition( uint64_t transform )
{
    auto transform_internal = framework::driver::read<uint64_t>( transform + 0x10 );

    auto matrices = framework::driver::read<uint64_t>( transform_internal + 0x38 );
    auto index = framework::driver::read<int>( transform_internal + 0x40 );

    framework::driver::read_buffer( (uintptr_t)(matrices + 0x18), sizeof( matrix_list_base ), &matrix_list_base );
    framework::driver::read_buffer( (uintptr_t)(matrices + 0x20), sizeof( dependency_index_table_base ), &dependency_index_table_base );

    static auto get_dependency_index = []( uint64_t base, int32_t index )
    {
        framework::driver::read_buffer( (uintptr_t)(base + index * 4), sizeof( index ), &index );
        return index;
    };

    static auto get_matrix_blob = []( uint64_t base, uint64_t offs, float *blob, uint32_t size )
    {
        framework::driver::read_buffer( (uintptr_t)(base + offs), size, blob );
    };

    int32_t index_relation = get_dependency_index( dependency_index_table_base, index );

    Vector3D ret_value;
    {
        float *base_matrix3x4 = (float *)malloc( 64 ),
            *matrix3x4_buffer0 = (float *)((uint64_t)base_matrix3x4 + 16),
            *matrix3x4_buffer1 = (float *)((uint64_t)base_matrix3x4 + 32),
            *matrix3x4_buffer2 = (float *)((uint64_t)base_matrix3x4 + 48);

        get_matrix_blob( matrix_list_base, index * 48, base_matrix3x4, 16 );

        __m128 xmmword_1410D1340 = { -2.f, 2.f, -2.f, 0.f };
        __m128 xmmword_1410D1350 = { 2.f, -2.f, -2.f, 0.f };
        __m128 xmmword_1410D1360 = { -2.f, -2.f, 2.f, 0.f };

        while ( index_relation >= 0 )
        {
            uint32_t matrix_relation_index = 6 * index_relation;

            // paziuret kur tik 3 nureadina, ten translationas, kur 4 = quatas ir yra rotationas.
            get_matrix_blob( matrix_list_base, 8 * matrix_relation_index, matrix3x4_buffer2, 16 );
            __m128 v_0 = *(__m128 *)matrix3x4_buffer2;

            get_matrix_blob( matrix_list_base, 8 * matrix_relation_index + 32, matrix3x4_buffer0, 16 );
            __m128 v_1 = *(__m128 *)matrix3x4_buffer0;

            get_matrix_blob( matrix_list_base, 8 * matrix_relation_index + 16, matrix3x4_buffer1, 16 );
            __m128i v9 = *(__m128i *)matrix3x4_buffer1;

            __m128 *v3 = (__m128 *)base_matrix3x4; // r10@1
            __m128 v10; // xmm9@2
            __m128 v11; // xmm3@2
            __m128 v12; // xmm8@2
            __m128 v13; // xmm4@2
            __m128 v14; // xmm2@2
            __m128 v15; // xmm5@2
            __m128 v16; // xmm6@2
            __m128 v17; // xmm7@2

            v10 = _mm_mul_ps( v_1, *v3 );
            v11 = _mm_castsi128_ps( _mm_shuffle_epi32( v9, 0 ) );
            v12 = _mm_castsi128_ps( _mm_shuffle_epi32( v9, 85 ) );
            v13 = _mm_castsi128_ps( _mm_shuffle_epi32( v9, -114 ) );
            v14 = _mm_castsi128_ps( _mm_shuffle_epi32( v9, -37 ) );
            v15 = _mm_castsi128_ps( _mm_shuffle_epi32( v9, -86 ) );
            v16 = _mm_castsi128_ps( _mm_shuffle_epi32( v9, 113 ) );

            v17 = _mm_add_ps(
                _mm_add_ps(
                _mm_add_ps(
                _mm_mul_ps(
                _mm_sub_ps(
                _mm_mul_ps( _mm_mul_ps( v11, (__m128)xmmword_1410D1350 ), v13 ),
                _mm_mul_ps( _mm_mul_ps( v12, (__m128)xmmword_1410D1360 ), v14 ) ),
                _mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128( v10 ), -86 ) ) ),
                _mm_mul_ps(
                _mm_sub_ps(
                _mm_mul_ps( _mm_mul_ps( v15, (__m128)xmmword_1410D1360 ), v14 ),
                _mm_mul_ps( _mm_mul_ps( v11, (__m128)xmmword_1410D1340 ), v16 ) ),
                _mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128( v10 ), 85 ) ) ) ),
                _mm_add_ps(
                _mm_mul_ps(
                _mm_sub_ps(
                _mm_mul_ps( _mm_mul_ps( v12, (__m128)xmmword_1410D1340 ), v16 ),
                _mm_mul_ps( _mm_mul_ps( v15, (__m128)xmmword_1410D1350 ), v13 ) ),
                _mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128( v10 ), 0 ) ) ),
                v10 ) ),
                v_0 );

            *v3 = v17;

            index_relation = get_dependency_index( dependency_index_table_base, index_relation );
        }

        ret_value = *(Vector3D *)base_matrix3x4;
        delete[] base_matrix3x4;
    }

    return ret_value;
}


Vector3D GetBonePosition( uintptr_t playerAddr, int boneID )
{
    // The power of external!
    Player player = framework::driver::read<Player>( playerAddr );
    PlayerBody playerBody = framework::driver::read<PlayerBody>( (uintptr_t)player.m_pPlayerBody );
    m_pSkeletonRootJoin skeletonRootJoin = framework::driver::read<m_pSkeletonRootJoin>( (uintptr_t)playerBody.m_pSkeletonRootJoin );
    BoneEnumerator bones = framework::driver::read<BoneEnumerator>( (uintptr_t)skeletonRootJoin.m_pBoneEnumerator );

    uintptr_t transform = framework::driver::read<uintptr_t>( uintptr_t( bones.m_pTransformArray ) + (0x20 + (boneID * 8)) );
    return GetPosition( transform );
}

inline void lg( std::string msg )
{
    static std::mutex m_mutex;
    std::lock_guard<std::mutex> guard( m_mutex );

    std::cout << "[ bd_debug ] " + msg << std::endl;
}

uint64_t GetObjectFromList( uint64_t listPtr, uint64_t lastObjectPtr, const char *objectName )
{
    unity_string name;
    uint64_t classNamePtr = 0x0;

    EFTStructs::BaseObject activeObject = framework::driver::read<EFTStructs::BaseObject>( listPtr );
    if ( !activeObject.object )
        return 0;

    EFTStructs::BaseObject lastObject = framework::driver::read<EFTStructs::BaseObject>( lastObjectPtr );   
    if ( !lastObject.object )
        return 0;

    if ( activeObject.object != 0x0 )
    {
        while ( activeObject.object != lastObject.object )
        {
            if ( activeObject.object == 0 )
                continue;

            classNamePtr = framework::driver::read<uint64_t>( activeObject.object + 0x60 );
            name = framework::driver::read<unity_string>( classNamePtr + 0x0 );
            if ( strcmp( name.buffer, objectName ) == 0 )
            {
                return activeObject.object;
            }

            activeObject = framework::driver::read<EFTStructs::BaseObject>( activeObject.nextObjectLink );
        }
    }

    if ( lastObject.object != 0x0 )
    {
        classNamePtr = framework::driver::read<uint64_t>( lastObject.object + 0x60 );
        name = framework::driver::read<unity_string>( classNamePtr + 0x0 );
        if ( strcmp( name.buffer, objectName ) == 0 )
        {
            return lastObject.object;
        }
    }

    return 0;
}

uint64_t GetComponentfromCamera( const char *compname, uint64_t u_gom )
{
    uint64_t cameraObjects = framework::driver::read<uint64_t>( u_gom + 0x18 );
    uint64_t lastObjects = framework::driver::read<uint64_t>( u_gom + 0x10 );
    uint64_t u_fps_camera = GetObjectFromList( cameraObjects, lastObjects, "FPS Camera" );

    //Test I guess? Dunno
    char *name;
    uint64_t testicles = framework::driver::read<uint64_t>( u_fps_camera + 0x30 );
    if ( !testicles )return 0;

    for ( int i = 0x8; i < 0x1000; i += 0x10 )
    {
        uint64_t Fields = framework::driver::read<uint64_t>( framework::driver::read<uint64_t>( testicles + i ) + 0x28 );
        if ( !Fields )return 0;
        uint64_t NameChain = framework::driver::read<uint64_t>( framework::driver::read<uint64_t>( framework::driver::read<uint64_t>( Fields ) ) + 0x48 );
        if ( !NameChain )return 0;
        name = framework::driver::read<unity_string>( NameChain ).buffer;
        if ( strcmp( name, compname ) == 0 )
        {
            return Fields;
        }
    }

    return 0;
}