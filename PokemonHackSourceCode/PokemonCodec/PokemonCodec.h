/*///////////////////////////////////////////////////////////////////////////////////////////////////
	Copyright(c) Fuzzier, 2004/8/22
///////////////////////////////////////////////////////////////////////////////////////////////////*/
#pragma once

extern LPCTSTR	szGameVersions[];
extern DWORD	dwGameVersionsCount;

///////////////////////////////////////////////////////////////////////////////////////////////////
BYTE	GetIsPokemonShiny(DWORD dwChar, DWORD dwID);

BYTE	GetPokemonPersonality(DWORD dwChar);

BYTE	GetSex(DWORD dwChar, BYTE bFemaleRatio);

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "pshpack1.h"
#define POKEMON_NICK_NAME_SIZE		(10)
#define POKEMON_TRAINER_NAME_SIZE	(7)
#define POKEMON_SKILL_COUNT			(4)
#define POKEMON_ABILITY_COUNT		(6)
#define POKEMON_CONDITION_COUNT		(6)

// note:
// bBadEgg: bit flags, usually '02', or '07' for bad eggs
// bit1: if this bit is not set, daycare center will not increase this pokemon's step counter, so it cannot gain exp or make egg
// bit0 & bit2: if both bits are set, the pokemon is a bag egg(だめタマゴ), and the bIsEgg bit in PokemonStructInnateInfo should be set
struct PokemonStructHeader
{
	DWORD	dwChar;					// @00, 性格値
	DWORD	dwID;					// @04, トレーナのＩＤ
	BYTE	bNickName[10];			// @08, ニックネーム
	BYTE	bNickNameLanguage;		// @12, '01' = 6 bytes (jp), '02' = 10 bytes (en)
	BYTE	bBadEgg;				// @12, '02' for daycare-center-enabled, '07' for ダメタマゴ
	BYTE	bCatcherName[7];		// @14, トレーナの名前
	BYTE	bMarkings;				// @1B, only the lowest 4 bits are used
	WORD	wChecksum;				// @1C, Checksum
	BYTE	unk0[0x02];				// @1E, '00 00'
};

struct PokemonStructBreedInfo
{
	WORD	wBreed		: 16;	// @0x00, ポケモンの種族のＩＤ
	WORD	wItem		: 16;	// @0x10, 持ち物
	DWORD	dwExp		: 32;	// @0x20, 経験値
	BYTE	bPointUp0	: 2;	// @0x40, わざの最大値が増えた値。increase by # * 20% of the base value
	BYTE	bPointUp1	: 2;	// @0x42,
	BYTE	bPointUp2	: 2;	// @0x44,
	BYTE	bPointUp3	: 2;	// @0x46,
	BYTE	bIntimate	: 8;	// @0x48, 懐き度
	WORD	unk0		: 16;	// @0x50, '00 00'
};

struct PokemonStructSkillInfo
{
	WORD	rgwSkillId[0x04];	// ID of the Skill
	BYTE	rgbPoints[0x04];	// Skill Points
};

struct PokemonStructAcquiredInfo
{
	BYTE	rgbBattleBonuses[0x06];	// 努力値（ＨＰ、攻撃、防御、素早さ、特攻、特防）
	BYTE	rgbApealPoints[0x06];	// コンディション（格好良さ、美しさ、可愛さ、賢さ、逞しさ、滑らかさ）
};

struct PokemonStructInnateInfo
{
	DWORD	bPokeVirus	: 4;	// @0x00, if any bit is set, ポケルスがかかる
	DWORD	bBlackPoint	: 4;	// @0x04, if any bit is set, 黒い点がつき
	DWORD	bCatchPlace	: 8;	// @0x08, where the Pokemon was caught
	DWORD	bCatchLevel	: 7;	// @0x10, the Pokemon's level when it was caught
	DWORD	bGameVersion: 4;	// @0x17, game version (sapphire=1, ruby=2, emerald=3, fire=4, leaf=5)
	DWORD	bPokeBall	: 4;	// @0x1B, Poke-Ball type (Write twice, why?)
	DWORD	unk1		: 1;	// @0x1F, 0
	DWORD	bHPIndv		: 5;	// @0x20, ＨＰ  (個体値)
	DWORD	bAtkIndv	: 5;	// @0x25, 攻撃  (個体値)
	DWORD	bDefIndv	: 5;	// @0x2A, 防御  (個体値)
	DWORD	bDexIndv	: 5;	// @0x2F, 素早さ(個体値)
	DWORD	bSAtkIndv	: 5;	// @0x34, 特攻  (個体値)
	DWORD	bSDefIndv	: 5;	// @0x39, 特防  (個体値)
	DWORD	bIsEgg		: 1;	// @0x3E, タマゴ
	DWORD	bSpecialty	: 1;	// @0x3F, 特性 (0: the 1st, 1: the 2nd)
	DWORD	bRibbon0	: 3;	// @0x40, # of リボン (0 - 4) かっこよさ：ノーマル、スーパー、ハイパー、マスター
	DWORD	bRibbon1	: 3;	// @0x43, # of リボン (0 - 4) うつくしさ：ノーマル、スーパー、ハイパー、マスター
	DWORD	bRibbon2	: 3;	// @0x46, # of リボン (0 - 4) かわいさ：ノーマル、スーパー、ハイパー、マスター
	DWORD	bRibbon3	: 3;	// @0x49, # of リボン (0 - 4) かしこさ：ノーマル、スーパー、ハイパー、マスター
	DWORD	bRibbon4	: 3;	// @0x4C, # of リボン (0 - 4) たくましさ：ノーマル、スーパー、ハイパー、マスター
	DWORD	bRibbon5	: 12;	// @0x4F, # of リボン (0 - 1) x 12 (bit0-4: ok, bit5-11: reserved)
	DWORD	unk2		: 4;	// @0x5B, 0
	DWORD	bObedience	: 1;	// @0x5F, 幻のポケモンは命令を聞く
};

union PokemonStructInfo
{
	PokemonStructBreedInfo		BreedInfo;
	PokemonStructSkillInfo		SkillInfo;
	PokemonStructAcquiredInfo	AcquiredInfo;
	PokemonStructInnateInfo		InnateInfo;
};

struct PokemonStruct
{
	PokemonStructHeader	Header;
	PokemonStructInfo	rgInfo[0x04];
};

struct PokemonStructRear	// only for active (in group) pokemons
{
	BYTE	bStatus;	// bit0-2:ねむりの回数, bit3:どく; bit4:やけど, bit5:こおり, bit6:まひ, bit7:もうどく
	BYTE	unk0[3];
	BYTE	bLevel;
	BYTE	unk1;		// this is always 'FF'
	WORD	wHPcur;
	WORD	wHPcap;
	WORD	wAtk;
	WORD	wDef;
	WORD	wDex;
	WORD	wSAtk;
	WORD	wSDef;
};

struct PokemonStructActive	// for active pokemons
{
	PokemonStruct		pmStruct;
	PokemonStructRear	pmRear;
};

#include "poppack.h"

enum eGameVersion
{
	pm_unknown=0,
	pm_sapphire=1,
	pm_ruby=2,
	pm_emerald=3,
	pm_fire=4,
	pm_leaf=5
};

enum { pm_female = 0, pm_male = 1, pm_neither = 2 };

///////////////////////////////////////////////////////////////////////////////////////////////////
class CPokemonCodec
{
protected:
	PokemonStruct *				m_pPokemon;
	PokemonStructBreedInfo *	m_pBreedInfo;
	PokemonStructSkillInfo *	m_pSkillInfo;
	PokemonStructAcquiredInfo *	m_pAcquiredInfo;
	PokemonStructInnateInfo *	m_pInnateInfo;

	BOOL	m_bEncoded;

	// the language system of the nick name is determined by PokemonStruct itself
	// if the PokemonStruct cannot determine (making a new pokemon ...), this value is used
	// so make sure this value is the language of the game
	// the language system of the catcher's name is determined by the language of the game
	DWORD	m_dwLang;

	VOID DetermineWhereIsWhich(BYTE bOrder[4]);
	VOID DetermineWhichIsWhere(BYTE bOrder[4]);
	VOID CalculatePokemonStructInfoPtr(VOID);

public:
	CPokemonCodec(VOID);
	virtual ~CPokemonCodec(VOID);

	VOID SetLang(DWORD dwLang);

	/*
		make sure lpPokemon is a valid pokemon,
		which should be 0x50 bytes each
	*/
	BOOL Attach(PokemonStruct * pPokemon, BOOL bEncoded);

	/*
		return value is the pokemon,
		which may be encoded or decoded
	*/
	PokemonStruct * Detach(VOID);

	VOID 	Decode(VOID);
	VOID 	Encode(VOID);

	/*
		a pokemon MUST be decoded to modify,
		the following functions will fail if it is not decoded
	*/
	VOID	GetNickName(BYTE bCode[POKEMON_NICK_NAME_SIZE]);
	VOID 	SetNickName(BYTE bCode[POKEMON_NICK_NAME_SIZE]);

	VOID	GetNickName(CString &szName);
	VOID	SetNickName(LPCTSTR szName);

	VOID	GetCatcherName(BYTE bCode[POKEMON_TRAINER_NAME_SIZE]);
	VOID	SetCatcherName(BYTE bCode[POKEMON_TRAINER_NAME_SIZE]);

	VOID	GetCatcherName(CString &szCatcherName);
	VOID	SetCatcherName(LPCTSTR szCatcherName);

	DWORD	GetChar(VOID);
	VOID	SetChar(DWORD dwChar);
	// preserves low word of char
	DWORD	GenShinyChar(VOID);
	DWORD	GenNoShinyChar(VOID);

	DWORD	GetID(VOID);
	VOID	SetID(DWORD dwID);
	// perserves low word of id, and char is unchanged
	DWORD	GenShinyID(VOID);
	DWORD	GenNoShinyID(VOID);

	BYTE	GetIsShiny(VOID);

	// return value: 0 - 24
	// bFemaleRatio: the value from rom
	BYTE	GetPersonality(VOID);
	// preserves shiny and sex, low word of char and id
	VOID	SetPersonality(BYTE bType);

	BYTE	GetSexByte(VOID);
	VOID	SetSexByte(BYTE bSexByte);

	// return value: 0=pm_female, 1=pm_male, 2=pm_neither
	BYTE	GetSex(BYTE bFemaleRatio);
	// preserves shiny and personality, low word of id
	VOID	SetSex(BYTE bType, BYTE bFemaleRatio);

	WORD	GetBreed(VOID);
	// after calling this function,
	// the pokemon SHOULD be set back to pc to refresh its level and other states,
	// or the game may crash
	VOID	SetBreed(WORD wBreed);

	BYTE	GetMarking(BYTE bIndex);
	VOID	SetMarking(BYTE bIndex, BYTE bFlag);

	WORD	GetItem(VOID);
	VOID	SetItem(WORD wItem);

	DWORD	GetExp(VOID);
	VOID	SetExp(DWORD dwExp);

	BYTE	GetIntimate(VOID);
	VOID	SetIntimate(BYTE bIntimate);

	WORD 	GetSkill(BYTE bIndex);
	BYTE	GetSkillPoints(BYTE bIndex);
	VOID	SetSkill(BYTE bIndex, WORD wSkill);
	VOID	SetSkillPoints(BYTE bIndex, BYTE bPoint);
	VOID 	SetSkill(BYTE bIndex, WORD wSkill, BYTE bPoint);

	BYTE	GetSkillPointBoost(BYTE bIndex);
	VOID	SetSkillPointBoost(BYTE bIndex, BYTE bBoost);
	VOID 	SetMaxSkillPointBoosts(VOID);

	VOID	GetBattleBonuses(BYTE bBB[6]);
	BYTE	GetBattleBonus(BYTE bIndex);
	VOID 	SetBattleBonuses(BYTE bBB[6]);
	VOID	SetBattleBonus(BYTE bIndex, BYTE bPoint);
	VOID	SetMaxBattleBonuses(VOID);

	VOID	GetApealPoints(BYTE bAP[6]);
	BYTE	GetApealPoint(BYTE bIndex);
	VOID 	SetApealPoints(BYTE bAP[6]);
	VOID	SetApealPoint(BYTE bIndex, BYTE bPoint);
	VOID	SetMaxApealPoints(VOID);

	BYTE	GetCatchPlace(VOID);
	VOID	SetCatchPlace(BYTE bCatchPlace);

	BYTE	GetCatchLevel(VOID);
	VOID	SetCatchLevel(BYTE bCatchLevel);

	BYTE	GetPokeBall(VOID);
	VOID	SetPokeBall(BYTE bPokeBall);

	VOID	GetIndvAbilities(BYTE lpbIndvAbl[6]);
	BYTE	GetIndvAbility(BYTE bIndex);
	VOID	SetIndvAbilities(BYTE lpbIndvAbl[6]);
	VOID	SetIndvAbility(BYTE bIndex, BYTE bPoint);
	VOID	SetMaxIndvAbilities(VOID);
	VOID	SetRandIndvAbilities(VOID);

	BYTE	GetIsEgg(VOID);
	VOID	SetIsEgg(BYTE bIsEgg);

	BYTE	GetSpecialty(VOID);
	VOID	SetSpecialty(BYTE bIndex);

	BYTE	GetPokerus(VOID);
	VOID	SetPokerus(BYTE bPokerus);

	BYTE	GetBlackPoint(VOID);
	VOID	SetBlackPoint(BYTE bBlackPoint);

	BYTE	GetGameVersion(VOID);
	VOID	SetGameVersion(BYTE bGameVersion);

	BYTE	GetObedience();
	VOID	SetObedience(BYTE bObedience);
	VOID	SetObedience();

	BYTE *	GetRibbonPointer(VOID);
	VOID	SetMaxRibbon(VOID);
	VOID	SetMaxRibbon32(VOID);
	VOID	ClearRibbons(VOID);

	VOID	CreatePokemon(WORD wBreed, DWORD dwLang, BYTE bGameVersion);
	VOID	CreatePokemon(WORD wBreed, BYTE bGameVersion);
};
