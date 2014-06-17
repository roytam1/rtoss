#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
#define ACTIVE_POKEMON_COUNT	(6)
#define STORED_POKEMON_COUNT	(14 * 30)

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
	if dwType == direct, value is stored at dwAddress + dwOffset
	if dwType == pointer, value is stored at [dwAddress] + dwOffset
*/
enum { direct=0, pointer=1 };
struct AddressStruct
{
	DWORD	dwAccessType;
	DWORD	dwAddress;
	DWORD	dwOffest;
};

#include "pshpack1.h"

enum { normalitem=0, keyitem=1, pokeball=2, machine=3, berry=4, pokeblock=5, unknownitem=-1};
struct PokemonItem
{
	WORD	wItem;
	WORD	wQuantity;
};

struct PokemonItemAddress
{
	AddressStruct	Address;
	DWORD			dwBagSize;	// # of items it can hold
};

struct PokemonPokeblock
{
	BYTE	bColor;			// 0x01 - 0x0E
	BYTE	rgbTastes[5];	// からい, しぶり, あまい, にがい, すっぱい
	BYTE	bWeak;			// the lower the better
	BYTE	bNumberOfPeople;
};

struct PmClockAdjustment
{
	WORD	wDay;
	BYTE	bHour;
	BYTE	bMinute;
	BYTE	bSecond;
};

#include "poppack.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
enum { mem_rs_jp=0, mem_rs_en=1, mem_fl_jp=2, mem_fl_en=3, mem_em_jp=4, mem_em_en=5 };

class CPokemonMemHackCore
{
public:
	CPokemonMemHackCore(VOID);
	~CPokemonMemHackCore(VOID);

	CPokemonCodec *		m_pActivePokeCodec;
	PokemonStructActive *m_pActivePokemon;

	CPokemonCodec *		m_pStoredPokeCodec;
	PokemonStruct *		m_pStoredPokemon;

	CVisualBoyHandler	m_vbah;
	DWORD				m_dwVersion;		// version used for memory hack
	DWORD				m_dwGameVersion;	// version used for ディオキシス
	DWORD				m_dwLang;

	BOOL Initialize();

	/////////////////////////////////////////////
	BOOL GetVersion(VOID);

	/////////////////////////////////////////////
	BOOL DecodeFirstActivePokemon(VOID);
	BOOL EncodeFirstActivePokemon(VOID);
	BOOL GetFirstActivePokemonAddress(DWORD &dwAddress);

	/////////////////////////////////////////////
	BOOL ReadActivePokemons(VOID);
	BOOL SaveActivePokemons(VOID);
	BOOL SwapActivePokemons(DWORD dwIndex1, DWORD dwIndex2);

	/////////////////////////////////////////////
	BOOL ReadPokemonsInPC(VOID);
	BOOL SavePokemonsInPC(VOID);

	/////////////////////////////////////////////
	BOOL	GetPlayerId(DWORD &dwId);
	BOOL	SetPlayerId(DWORD dwId);

	BOOL	GetPlayerName(CString &szName);
	BOOL	GetPlayerName(BYTE bCode[POKEMON_TRAINER_NAME_SIZE]);
	BOOL	SetPlayerName(LPCTSTR szName);

	BOOL	GetMoney(DWORD &dwMoney);
	BOOL	SetMoney(DWORD dwMoney);

	BOOL	GetCoin(WORD &wCoin);
	BOOL	SetCoin(WORD wCoin);

	BOOL	GetDust(WORD &wDust);
	BOOL	SetDust(WORD wDust);

	BOOL	GetSprayTime(WORD &wSprayTime);
	BOOL	SetSprayTime(WORD wSprayTime);

	BOOL	GetSafariTime(WORD &wSafariTime);
	BOOL	SetSafariTime(WORD wSafariTime);

	BOOL	GetBattlePointsCurrent(WORD &wBP);
	BOOL	SetBattlePointsCurrent(WORD wBP);

	BOOL	GetBattlePointsTrainerCard(WORD &wBP);
	BOOL	SetBattlePointsTrainerCard(WORD wBP);

	/////////////////////////////////////////////
	BOOL	SetDaycareCenterStepByte(DWORD dwIndex, BYTE bCount);
	BOOL	SetStepCounter(BYTE bCount);
	BOOL	SetExpGain(SHORT nExpGain);

	/////////////////////////////////////////////
	BOOL	UpgradePokedex();
	BOOL	SetFullPokedex();

	/////////////////////////////////////////////
	BOOL	AdjustClock();

	/////////////////////////////////////////////
	VOID	GetItemType(WORD wItem, DWORD &dwItemType, WORD &wMaxQuantity);
	BOOL	AddItem(WORD wItem, WORD wQuantity);
	BOOL	IncreaseNormalItems(WORD wQuantity);

	BOOL	AddItemToBattlePyramid50(WORD wItem, BYTE bQuantity);
	BOOL	AddItemToBattlePyramid60(WORD wItem, BYTE bQuantity);
	BOOL	SetBattlePyramidViewRadius(BYTE bRadius);

	BOOL	GiveAllPokeball(WORD wQuantity);
	BOOL	GiveAllSkillMachines(WORD wQuantity);
	BOOL	GiveAllBerries(WORD wQuantity);
	BOOL	GiveAllPokeblocks();

	BOOL	GiveAllDecorates();

	BOOL	DropLastKeyItem();

	BOOL	GiveAllBadges();

	BOOL	EnableAllTickets();

	BOOL	EnableMirageIsland();

protected:
	BOOL Read(CONST AddressStruct &rAddressStruct, DWORD_PTR dwSize, LPVOID lpBuffer);
	BOOL Write(CONST AddressStruct &rAddressStruct, DWORD_PTR dwSize, LPCVOID lpBuffer);
	BOOL GetXorMask(DWORD &dwXorMask);
	VOID MaskItem(PokemonItem &Item, DWORD dwXorMask);
};
