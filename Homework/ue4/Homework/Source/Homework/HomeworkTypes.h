#pragma once

#define ECC_InteractionVolume ECC_GameTraceChannel1
#define ECC_Climbing ECC_GameTraceChannel2
#define ECC_WallRunnable ECC_GameTraceChannel3
#define ECC_FloorSlide ECC_GameTraceChannel4
#define ECC_Bullet ECC_GameTraceChannel5

const FName FXParamTraceEnd = FName("TraceEnd");

const FName SocketCharacterWeapon = FName("WeaponCharacterSocket");
const FName SocketWeaponMuzzle = FName("MuzzleSocket");
const FName SocketWeaponSecondaryMuzzle = FName("SecondaryMuzzleSocket");
const FName SocketWeaponForeGrip = FName("ForeGripSocket");
const FName SocketCharacterThrowable = FName("ThrowableSocket");

const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");
const FName CollisionProfileRagdoll = FName("Ragdoll");

const FName SectionMontageReloadName = FName("ReloadEnd");

const FName DebugCategoryCharacterAttributes = FName("CharacterAttributes");
const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryWallDetection = FName("WallDetection");
const FName DebugCategoryFloorDetection = FName("FloorDetection");
const FName DebugCategoryCeilingDetection = FName("CeilingDetection");
const FName DebugCategoryShotDetection = FName("ShotDetection");

UENUM(BlueprintType)
enum class EEquipableItemType : uint8
{
	None,
	Pistol,
	Rifle,
	Shotgun,
	Throwable,
	SniperRifle
	
};

UENUM(BlueprintType)
enum class EAmunitionType : uint8
{
	None,
	Pistol,
	Rifle,
	Shotgun,
	SniperRifle,
	RifleGrenade,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None,
	SideArm,
	PrimaryWeapon,
	SecondaryWeapon,
	PrimaryItemSlot,
	BigGunWeapon,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECurrentAttribute : uint8
{
	None,
	Health,
	Stamina,
	Oxygen,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECurrentReticle : uint8
{
	None,
	Default,
	SniperRilfe,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponMode : uint8
{
	FirstMode,
	SecondMode,
	MAX UMETA(Hidden)
};