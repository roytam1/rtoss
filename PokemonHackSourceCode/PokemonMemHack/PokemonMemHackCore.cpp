#include "stdafx.h"
#include "PokemonTool.h"
#include "PokemonCodec.h"
#include "PokemonMemHackCore.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////
AddressStruct ACTIVE_POKEMON_COUNT_ADDRESS[] =
{
	{ direct,  0x03004280, 0 },	// ruby/sapphire jp
	{ direct,  0x03004350, 0 },	// ruby/sapphire en
	{ direct,  0, 0 },			// does not exist in fire/leaf jp
	{ direct,  0, 0 },			// does not exist in fire/leaf en
	{ direct,  0x0202418D, 0 },	// emarald jp
	{ direct,  0x020244E9, 0 },	// emarald en d
};

AddressStruct ACTIVE_POKEMON_ADDRESS[] =
{
	{ direct, 0x03004290, 0 },	// ruby/sapphire jp
	{ direct, 0x03004360, 0 },	// ruby/sapphire en
	{ direct, 0x020241E4, 0 },	// fire/leaf jp
	{ direct, 0x02024284, 0 },	// fire/leaf en
	{ direct, 0x02024190, 0 },	// emarald jp
	{ direct, 0x020244EC, 0 },	// emarald en d
};

AddressStruct STORED_POKEMON_ADDRESS[] =
{
	{ direct,  0x0202FDC0, 0 },				// ruby/sapphire jp
	{ direct,  0x020300A4, 0 },				// ruby/sapphire en
	{ pointer, 0x03005050, 0x00000004 },	// fire/leaf jp
	{ pointer, 0x03005010, 0x00000004 },	// fire/leaf en
	{ pointer, 0x03005AF4, 0x00000004 },	// emarald jp
	{ pointer, 0x03005D94, 0x00000004 },	// emarald en d
};

AddressStruct PLAYER_NAME_ADDRESS[] =
{
	{ direct,  0x02024C04, 0 },				// ruby/sapphire jp
	{ direct,  0x02024EA4, 0 },				// ruby/sapphire en
	{ pointer, 0x0300504C, 0 },				// fire/leaf jp
	{ pointer, 0x0300500C, 0 },				// fire/leaf en
	{ pointer, 0x03005AF0, 0 },				// emarald jp
	{ pointer, 0x03005D90, 0 },				// emarald en d
};

AddressStruct PLAYER_ID_ADDRESS[] =
{
	{ direct,  0x02024C0E, 0 },				// ruby/sapphire jp
	{ direct,  0x02024EAE, 0 },				// ruby/sapphire en
	{ pointer, 0x0300504C, 0x0000000A },	// fire/leaf jp
	{ pointer, 0x0300500C, 0x0000000A },	// fire/leaf en
	{ pointer, 0x03005AF0, 0x0000000A },	// emarald jp
	{ pointer, 0x03005D90, 0x0000000A },	// emarald en d
};

AddressStruct XOR_MASK_ADDRESS[] =
{
	{ direct,  0,          0 },				// no xor mask in ruby/sapphire jp
	{ direct,  0,          0 },				// no xor mask in ruby/sapphire en
	{ pointer, 0x0300504C, 0x00000AF8 },	// fire/leaf jp
	{ pointer, 0x0300500C, 0x00000AF8 },	// fire/leaf en
	{ pointer, 0x03005AF0, 0x000000AC },	// emarald jp
	{ pointer, 0x03005D90, 0x000000AC },	// emarald en d
};

AddressStruct MONEY_ADDRESS[] =
{
	{ direct,  0x02025924, 0 },				// ruby/sapphire jp
	{ direct,  0x02025BC4, 0 },				// ruby/sapphire en
	{ pointer, 0x0300504C, 0x00001234 },	// fire/leaf jp
	{ pointer, 0x0300500C, 0x00001234 },	// fire/leaf en
	{ pointer, 0x03005AF0, 0x0000143C },	// emarald jp
	{ pointer, 0x03005D90, 0x0000143C },	// emarald en d
};

AddressStruct COIN_ADDRESS[] =
{
	{ direct,  0x02025928, 0 },				// ruby/sapphire jp
	{ direct,  0x02025BC8, 0 },				// ruby/sapphire en
	{ pointer, 0x0300504C, 0x00001238 },	// fire/leaf jp
	{ pointer, 0x0300500C, 0x00001238 },	// fire/leaf en
	{ pointer, 0x03005AF0, 0x00001440 },	// emarald jp
	{ pointer, 0x03005D90, 0x00001440 },	// emarald en
};

AddressStruct DUST_ADDRESS[] =
{
	{ direct,  0x02026864, 0 },				// ruby/sapphire jp
	{ direct,  0x02026B04, 0 },				// ruby/sapphire en
	{ pointer, 0x00000000, 0 },				// does not exist in fire/leaf jp
	{ pointer, 0x00000000, 0 },				// does not exist in fire/leaf en
	{ pointer, 0x03005AF0, 0x000023D8 },	// emarald jp
	{ pointer, 0x03005D90, 0x000023D8 },	// emarald en
};

// only the first 2 bits will be changed,
// the 3rd bit ポケナビ does not exist in file/leaf
AddressStruct MENU_ADDRESS[] =
{
	{ direct,  0x020267B4, 0 },				// ruby/sapphire jp
	{ direct,  0x02026A54, 0 },				// ruby/sapphire en
	{ pointer, 0x0300504C, 0x00001F89 },	// fire/leaf jp
	{ pointer, 0x0300500C, 0x00001F89 },	// fire/leaf en
	{ pointer, 0x03005AF0, 0x00002328 },	// emarald jp
	{ pointer, 0x03005D90, 0x00002328 },	// emarald en d
};

AddressStruct DECORATE_ADDRESS[] =
{
	{ direct,  0x02027B34, 0 },				// ruby/sapphire jp
	{ direct,  0x02027DD4, 0 },				// ruby/sapphire en
	{ pointer, 0,          0 },				// does not exist in fire/leaf jp
	{ pointer, 0,          0 },				// does not exist in fire/leaf en
	{ pointer, 0x03005AF0, 0x000036E0 },	// emarald jp
	{ pointer, 0x03005D90, 0x000036E0 },	// emarald en
};

AddressStruct BADGE_ADDRESS[] =
{
	{ direct,  0x020267B5, 0 },				// ruby/sapphire jp
	{ direct,  0x02026A55, 0 },				// ruby/sapphire en
	{ pointer, 0x0300504C, 0x00001F8A },	// fire/leaf jp
	{ pointer, 0x0300500C, 0x00001F8A },	// fire/leaf en
	{ pointer, 0x03005AF0, 0x00002329 },	// emarald jp
	{ pointer, 0x03005D90, 0x00002329 },	// emarald en d
};

AddressStruct CLOCK_ADJUSTMENT_ADDRESS[] =
{
	{ direct,  0x02024C9C, 0 },				// ruby/sapphire jp
	{ direct,  0x02024F3C, 0 },				// ruby/sapphire en
	{ pointer, 0x00000000, 0 },				// fire/leaf jp
	{ pointer, 0x00000000, 0 },				// fire/leaf en
	{ pointer, 0x03005AF0, 0x00000098 },	// emarald jp
	{ pointer, 0x03005D90, 0x00000098 },	// emarald en d
};

AddressStruct PER_DAY_RANDOM_DWORD_ADDRESS[] =
{
	{ direct,  0x0202681C, 0 },				// ruby/sapphire jp
	{ direct,  0x02026B0A, 0 },				// ruby/sapphire en
	{ pointer, 0x00000000, 0 },				// does not exist in fire/leaf jp
	{ pointer, 0x00000000, 0 },				// does not exist in fire/leaf en
	{ pointer, 0x03005AF0, 0x00002390 },	// emarald jp
	{ pointer, 0x03005D90, 0x00002390 },	// emarald en
};

AddressStruct SPRAY_TIME_ADDRESS[] =
{
	{ direct,  0x02026816, 0 },				// ruby/sapphire jp
	{ direct,  0x02026AB6, 0 },				// ruby/sapphire en
	{ pointer, 0x0300504C, 0x00001FE4 },	// fire/leaf jp
	{ pointer, 0x0300500C, 0x00001FE4 },	// fire/leaf en
	{ pointer, 0x03005AF0, 0x0000238A },	// emarald jp
	{ pointer, 0x03005D90, 0x0000238A },	// emarald en
};

AddressStruct SAFARI_BALLS_ADDRESS[] =
{
	{ direct,  0x02038504, 0 },		// ruby/sapphire jp
	{ direct,  0x02038808, 0 },		// ruby/sapphire en
	{ direct,  0x0203990C, 0 },		// fire/leaf jp
	{ direct,  0x02039994, 0 },		// fire/leaf en
	{ direct,  0x02039D18, 0 },		// emarald jp
	{ direct,  0x0203A9FC, 0 },		// emarald en ?????
};

AddressStruct SAFARI_TIME_ADDRESS[] =
{
	{ direct,  0x02038506, 0 },		// ruby/sapphire jp
	{ direct,  0x0203880A, 0 },		// ruby/sapphire en
	{ direct,  0x0203990E, 0 },		// fire/leaf jp
	{ direct,  0x02039996, 0 },		// fire/leaf en
	{ direct,  0x02039D1A, 0 },		// emarald jp
	{ direct,  0x0203A076, 0 },		// emarald en ?????
};

AddressStruct DAYCARE_CENTER_STEP[][2] =
{
	// ruby/sapphire jp
	{
		{ direct,  0x02028540, 0 },		// 1st pokemon
		{ direct,  0x02028544, 0 },		// 2nd pokemon
	},

	// ruby/sapphire en
	{
		{ direct,  0x020287E0, 0 },		// 1st pokemon
		{ direct,  0x020287E4, 0 },		// 2nd pokemon
	},

	// fire/leaf jp
	{
		{ pointer, 0x0300504C, 0x00003FAC },	// 1st pokemon
		{ pointer, 0x0300504C, 0x00004038 },	// 2nd pokemon
	},

	// fire/leaf en
	{
		{ pointer, 0x0300500C, 0x00003FAC },	// 1st pokemon
		{ pointer, 0x0300500C, 0x00004038 },	// 2nd pokemon
	},

	// emarald jp
	{
		{ pointer, 0x03005AF0, 0x00004064 },	// 1st pokemon
		{ pointer, 0x03005AF0, 0x000040F0 },	// 2nd pokemon
	},

	// emarald en
	{
		{ pointer, 0x03005D90, 0x00004064 },	// 1st pokemon
		{ pointer, 0x03005D90, 0x000040F0 },	// 2nd pokemon
	},
};

AddressStruct STEP_COUNTER_ADDRESS[] =
{
	{ direct,  0x0202854A, 0 },				// ruby/sapphire jp
	{ direct,  0x020287EA, 0 },				// ruby/sapphire en
	{ pointer, 0x0300504C, 0x0000403E },	// fire/leaf jp
	{ pointer, 0x0300500C, 0x0000403E },	// fire/leaf en
	{ pointer, 0x03005AF0, 0x000040F8 },	// emarald jp
	{ pointer, 0x03005D90, 0x000040F8 },	// emarald en
};

AddressStruct EXP_GAIN_ADDRESS[] =
{
	{ direct,  0x0202494C, 0 },		// ruby/sapphire jp
	{ direct,  0x02024BEC, 0 },		// ruby/sapphire en
	{ direct,  0x02023CB0, 0 },		// fire/leaf jp
	{ direct,  0x02023D50, 0 },		// fire/leaf en
	{ direct,  0x02023E94, 0 },		// emarald jp
	{ direct,  0x020241F0, 0 },		// emarald en d
};

PokemonItemAddress ITEM_ADDRESS[][6] =
{
	// ruby/sapphire jp
	{
		{ { direct,  0x020259F4, 0 }, 0x14 },	// normalitem
		{ { direct,  0x02025A44, 0 }, 0x14 },	// keyitem
		{ { direct,  0x02025A94, 0 }, 0x10 },	// pokeball
		{ { direct,  0x02025AD4, 0 }, 0x40 },	// machine
		{ { direct,  0x02025BD4, 0 }, 0x2E },	// berry
		{ { direct,  0x02025C8C, 0 }, 0x28 },	// pokeblock
	},

	// ruby/sapphire en
	{
		{ { direct,  0x02025C94, 0 }, 0x14 },	// normalitem
		{ { direct,  0x02025CE4, 0 }, 0x14 },	// keyitem
		{ { direct,  0x02025D34, 0 }, 0x10 },	// pokeball
		{ { direct,  0x02025D74, 0 }, 0x40 },	// machine
		{ { direct,  0x02025E74, 0 }, 0x2E },	// berry
		{ { direct,  0x02025F2C, 0 }, 0x28 },	// pokeblock
	},

	// fire/leaf jp
	{
		{ { pointer, 0x0300504C, 0x000012B4 }, 0x2A },	// normalitem
		{ { pointer, 0x0300504C, 0x0000135C }, 0x1E },	// keyitem
		{ { pointer, 0x0300504C, 0x000013D4 }, 0x0D },	// pokeball
		{ { pointer, 0x0300504C, 0x00001408 }, 0x3A },	// machine
		{ { pointer, 0x0300504C, 0x000014F0 }, 0x2B }, 	// berry
		{ { direct,  0,          0          }, 0    }, 	// no pokeblock in fire/leaf
	},

	// fire/leaf en
	{
		{ { pointer, 0x0300500C, 0x000012B4 }, 0x2A },	// normalitem
		{ { pointer, 0x0300500C, 0x0000135C }, 0x1E },	// keyitem
		{ { pointer, 0x0300500C, 0x000013D4 }, 0x0D },	// pokeball
		{ { pointer, 0x0300500C, 0x00001408 }, 0x3A },	// machine
		{ { pointer, 0x0300500C, 0x000014F0 }, 0x2B }, 	// berry
		{ { direct,  0,          0          }, 0    }, 	// no pokeblock in fire/leaf
	},

	// emerald jp
	{
		{ { pointer, 0x03005AF0, 0x0000150C }, 0x1E },	// normalitem
		{ { pointer, 0x03005AF0, 0x00001584 }, 0x1E },	// keyitem
		{ { pointer, 0x03005AF0, 0x000015FC }, 0x10 },	// pokeball
		{ { pointer, 0x03005AF0, 0x0000163C }, 0x40 },	// machine
		{ { pointer, 0x03005AF0, 0x0000173C }, 0x46 }, 	// berry
		{ { pointer, 0x03005AF0, 0x000017F4 }, 0x28 }, 	// pokeblock
	},

	// emerald en
	{
		{ { pointer, 0x03005D90, 0x0000150C }, 0x1E },	// normalitem
		{ { pointer, 0x03005D90, 0x00001584 }, 0x1E },	// keyitem
		{ { pointer, 0x03005D90, 0x000015FC }, 0x10 },	// pokeball
		{ { pointer, 0x03005D90, 0x0000163C }, 0x40 },	// machine
		{ { pointer, 0x03005D90, 0x0000173C }, 0x46 }, 	// berry
		{ { pointer, 0x03005D90, 0x000017F4 }, 0x28 }, 	// pokeblock
	},
};

///////////////////////////////////////////////////////////////////////////////////////////////////
CPokemonMemHackCore::CPokemonMemHackCore(VOID)
{
	m_dwVersion = mem_fl_jp;	// default to fire/leaf
	m_dwGameVersion = pm_unknown;
	m_pActivePokeCodec = NULL;
	m_pActivePokemon = NULL;
	m_pStoredPokeCodec = NULL;
	m_pStoredPokemon = NULL;
}

CPokemonMemHackCore::~CPokemonMemHackCore(VOID)
{
	if(m_pActivePokeCodec)
		delete []m_pActivePokeCodec;
	if(m_pActivePokemon)
		delete []m_pActivePokemon;

	if(m_pStoredPokeCodec)
		delete []m_pStoredPokeCodec;
	if(m_pStoredPokemon)
		delete []m_pStoredPokemon;
}

BOOL CPokemonMemHackCore::GetVersion(VOID)
{
	BOOL	bResult;
	CHAR	szGameTitle[0x10];
	CHAR	szGameVersion[0x20];

	bResult = m_vbah.RefreshAddressList();
	if(bResult)
	{
		bResult = m_vbah.Read(0x080000A0, sizeof(szGameTitle), szGameTitle);
	}
	if(bResult)
	{
		bResult = m_vbah.Read(0x08000108, sizeof(szGameVersion), szGameVersion);
	}
	if(bResult)
	{
		if(strncmp(szGameTitle, "POKEMON RUBYAXVJ", sizeof(szGameTitle)) == 0)
		{
			m_dwVersion = mem_rs_jp;
			m_dwGameVersion = pm_ruby;
			m_dwLang = lang_jp;
		}
		else if(strncmp(szGameTitle, "POKEMON SAPPAXPJ", sizeof(szGameTitle)) == 0)
		{
			m_dwVersion = mem_rs_jp;
			m_dwGameVersion = pm_sapphire;
			m_dwLang = lang_jp;
		}
		else if(strncmp(szGameTitle, "POKEMON RUBYAXVE", sizeof(szGameTitle)) == 0)
		{
			m_dwVersion = mem_rs_en;
			m_dwGameVersion = pm_ruby;
			m_dwLang = lang_en;
		}
		else if(strncmp(szGameTitle, "POKEMON SAPPAXPE", sizeof(szGameTitle)) == 0)
		{
			m_dwVersion = mem_rs_en;
			m_dwGameVersion = pm_sapphire;
			m_dwLang = lang_en;
		}
		else if(strncmp(szGameTitle, "POKEMON FIREBPRJ", sizeof(szGameTitle)) == 0)
		{
			m_dwVersion = mem_fl_jp;
			m_dwGameVersion = pm_fire;
			m_dwLang = lang_jp;
		}
		else if(strncmp(szGameTitle, "POKEMON LEAFBPGJ", sizeof(szGameTitle)) == 0)
		{
			m_dwVersion = mem_fl_jp;
			m_dwGameVersion = pm_leaf;
			m_dwLang = lang_jp;
		}
		else if(strncmp(szGameTitle, "POKEMON FIREBPRE", sizeof(szGameTitle)) == 0)
		{
			m_dwVersion = mem_fl_en;
			m_dwGameVersion = pm_fire;
			m_dwLang = lang_en;
		}
		else if(strncmp(szGameTitle, "POKEMON LEAFBPGE", sizeof(szGameTitle)) == 0)
		{
			m_dwVersion = mem_fl_en;
			m_dwGameVersion = pm_leaf;
			m_dwLang = lang_en;
		}
		else if(strncmp(szGameTitle, "POKEMON EMERBPEJ", sizeof(szGameTitle)) == 0)
		{
			m_dwVersion = mem_em_jp;
			m_dwGameVersion = pm_emerald;
			m_dwLang = lang_jp;
		}
		else if(strncmp(szGameTitle, "POKEMON EMERBPEE", sizeof(szGameTitle)) == 0)
		{
			m_dwVersion = mem_em_en;
			m_dwGameVersion = pm_emerald;
			m_dwLang = lang_en;
		}
		else if(strncmp(szGameTitle, "YJencrypted", sizeof(szGameTitle)) == 0)
		{
			if(strncmp(szGameVersion, "pokemon red version", sizeof(szGameVersion)) == 0)
			{
				m_dwVersion = mem_fl_jp;
				m_dwGameVersion = pm_fire;
				m_dwLang = lang_jp;
			}
			else if(strncmp(szGameVersion, "pokemon green version", sizeof(szGameVersion)) == 0)
			{
				m_dwVersion = mem_fl_jp;
				m_dwGameVersion = pm_leaf;
				m_dwLang = lang_jp;
			}
			else if(strncmp(szGameVersion, "pokemon emerald version", sizeof(szGameVersion)) == 0)
			{
				m_dwVersion = mem_em_jp;
				m_dwGameVersion = pm_emerald;
				m_dwLang = lang_jp;
			}
			else
			{
				bResult = FALSE;
			}
		}
		else
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::Read(CONST AddressStruct &rAddressStruct, DWORD_PTR dwSize, LPVOID lpBuffer)
{
	if(rAddressStruct.dwAddress == 0)
		return TRUE;

	switch(rAddressStruct.dwAccessType)
	{
	case direct:
		return m_vbah.Read(rAddressStruct.dwAddress + rAddressStruct.dwOffest, dwSize, lpBuffer);
		break;
	case pointer:
		return m_vbah.PtrRead(rAddressStruct.dwAddress, rAddressStruct.dwOffest, dwSize, lpBuffer);
		break;
	default:
		return FALSE;
	}
}

BOOL CPokemonMemHackCore::Write(CONST AddressStruct &rAddressStruct, DWORD_PTR dwSize, LPCVOID lpBuffer)
{
	if(rAddressStruct.dwAddress == 0)
		return TRUE;

	switch(rAddressStruct.dwAccessType)
	{
	case direct:
		return m_vbah.Write(rAddressStruct.dwAddress + rAddressStruct.dwOffest, dwSize, lpBuffer);
		break;
	case pointer:
		return m_vbah.PtrWrite(rAddressStruct.dwAddress, rAddressStruct.dwOffest, dwSize, lpBuffer);
		break;
	default:
		return FALSE;
	}
}

BOOL CPokemonMemHackCore::Initialize()
{
	BOOL	bResult;
	DWORD	dwIndex;

	try
	{
		if(m_pActivePokemon == NULL)
		{
			m_pActivePokemon = new PokemonStructActive[ACTIVE_POKEMON_COUNT];
			if(m_pActivePokemon == NULL)
				throw 0;

			ZeroMemory(m_pActivePokemon, ACTIVE_POKEMON_COUNT * sizeof(PokemonStructActive));

			for(dwIndex = 0; dwIndex < ACTIVE_POKEMON_COUNT; ++dwIndex)
				m_pActivePokemon[dwIndex].pmRear.unk1 = 0xFF;
		}
		if(m_pActivePokeCodec == NULL)
		{
			m_pActivePokeCodec = new CPokemonCodec[ACTIVE_POKEMON_COUNT];
			if(m_pActivePokeCodec == NULL)
				throw 0;
		}

		for(dwIndex = 0; dwIndex < ACTIVE_POKEMON_COUNT; ++dwIndex)
		{
			m_pActivePokeCodec[dwIndex].SetLang(lang_jp);
			if(!m_pActivePokeCodec[dwIndex].Attach(&(m_pActivePokemon[dwIndex].pmStruct), TRUE))
				throw 0;
			m_pActivePokeCodec[dwIndex].Decode();
		}
		
		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}

	if(!bResult)
	{
		if(m_pActivePokemon)
		{
			delete []m_pActivePokemon;
			m_pActivePokemon = NULL;
		}
		if(m_pActivePokeCodec)
		{
			delete []m_pActivePokeCodec;
			m_pActivePokeCodec = NULL;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::ReadActivePokemons(VOID)
{
	BOOL	bResult;
	DWORD	dwIndex;

	try
	{
		if(!GetVersion())
			throw 0;

		if(!Read(ACTIVE_POKEMON_ADDRESS[m_dwVersion],
				ACTIVE_POKEMON_COUNT * sizeof(PokemonStructActive),
				m_pActivePokemon))
			throw 0;

		for(dwIndex = 0; dwIndex < ACTIVE_POKEMON_COUNT; ++dwIndex)
		{
			m_pActivePokeCodec[dwIndex].SetLang(m_dwLang);
			if(!m_pActivePokeCodec[dwIndex].Attach(&(m_pActivePokemon[dwIndex].pmStruct), TRUE))
				throw 0;
			m_pActivePokeCodec[dwIndex].Decode();
		}

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SaveActivePokemons(VOID)
{
	BOOL	bResult;
	DWORD	dwIndex;
	BYTE	bPokemonCount;

	try
	{
		if(m_pActivePokemon == NULL || m_pActivePokeCodec == NULL)
			throw 0;

		if(!GetVersion())
			throw 0;

		for(dwIndex = 0; dwIndex < ACTIVE_POKEMON_COUNT; ++dwIndex)
		{
			m_pActivePokeCodec[dwIndex].Encode();
		}

		if(!Write(ACTIVE_POKEMON_ADDRESS[m_dwVersion], ACTIVE_POKEMON_COUNT * sizeof(PokemonStructActive), m_pActivePokemon))
			throw 0;

		bPokemonCount = 0;
		for(dwIndex = 0; dwIndex < ACTIVE_POKEMON_COUNT; ++dwIndex)
		{
			m_pActivePokeCodec[dwIndex].Decode();
			if(m_pActivePokeCodec[dwIndex].GetBreed() != 0)
				++bPokemonCount;
		}

		if(!Write(ACTIVE_POKEMON_COUNT_ADDRESS[m_dwVersion], sizeof(BYTE), &bPokemonCount))
			throw 0;

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SwapActivePokemons(DWORD dwIndex1, DWORD dwIndex2)
{
	BOOL	bResult = TRUE;
	DWORD	dwIndex;
	PokemonStructActive *	pActivePokemon = NULL;

	try
	{
		if(dwIndex1 < ACTIVE_POKEMON_COUNT &&
			dwIndex2 < ACTIVE_POKEMON_COUNT)
		{
			pActivePokemon = new PokemonStructActive;
			if(!pActivePokemon)
				throw 0;

			CopyMemory(pActivePokemon, &(m_pActivePokemon[dwIndex1]), sizeof(PokemonStructActive));
			CopyMemory(&(m_pActivePokemon[dwIndex1]), &(m_pActivePokemon[dwIndex2]), sizeof(PokemonStructActive));
			CopyMemory(&(m_pActivePokemon[dwIndex2]), pActivePokemon, sizeof(PokemonStructActive));
			delete pActivePokemon;
			pActivePokemon = NULL;

			for(dwIndex = 0; dwIndex < ACTIVE_POKEMON_COUNT; ++dwIndex)
			{
				m_pActivePokeCodec[dwIndex].Attach(&(m_pActivePokemon[dwIndex].pmStruct), FALSE);
			}
		}
	}
	catch(int)
	{
		bResult = FALSE;
	}

	if(pActivePokemon)
		delete pActivePokemon;

	return bResult;
}

BOOL CPokemonMemHackCore::GetFirstActivePokemonAddress(DWORD &dwAddress)
{
	BOOL	bResult;

	try
	{
		if(!GetVersion())
			throw 0;

		if(ACTIVE_POKEMON_ADDRESS[m_dwVersion].dwAccessType == direct)
		{
			dwAddress = ACTIVE_POKEMON_ADDRESS[m_dwVersion].dwAddress +
						ACTIVE_POKEMON_ADDRESS[m_dwVersion].dwOffest;
		}
		else
		{
			bResult = m_vbah.Read(ACTIVE_POKEMON_ADDRESS[m_dwVersion].dwAddress, sizeof(DWORD), &dwAddress);
			if(!bResult)
				throw 0;
			dwAddress += ACTIVE_POKEMON_ADDRESS[m_dwVersion].dwOffest;
		}

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL CPokemonMemHackCore::ReadPokemonsInPC(VOID)
{
	BOOL	bResult;
	DWORD	dwIndex;

	try
	{
		if(!GetVersion())
			throw 0;

		if(m_pStoredPokemon == NULL)
		{
			m_pStoredPokemon = new PokemonStruct[STORED_POKEMON_COUNT];
			if(m_pStoredPokemon == NULL)
				throw 0;
		}
		if(m_pStoredPokeCodec == NULL)
		{
			m_pStoredPokeCodec = new CPokemonCodec[STORED_POKEMON_COUNT];
			if(m_pStoredPokeCodec == NULL)
				throw 0;
		}

		if(!Read(STORED_POKEMON_ADDRESS[m_dwVersion],
				STORED_POKEMON_COUNT * sizeof(PokemonStruct),
				m_pStoredPokemon))
		{
			throw 0;
		}

		for(dwIndex = 0; dwIndex < STORED_POKEMON_COUNT; ++dwIndex)
		{
			m_pStoredPokeCodec[dwIndex].SetLang(m_dwLang);
			if(!m_pStoredPokeCodec[dwIndex].Attach(&(m_pStoredPokemon[dwIndex]), TRUE))
				throw 0;
			m_pStoredPokeCodec[dwIndex].Decode();
		}

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}

	if(!bResult)
	{
		if(m_pStoredPokemon)
		{
			delete []m_pStoredPokemon;
			m_pStoredPokemon = NULL;
		}
		if(m_pStoredPokeCodec)
		{
			delete []m_pStoredPokeCodec;
			m_pStoredPokeCodec = NULL;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SavePokemonsInPC(VOID)
{
	BOOL	bResult;
	DWORD	dwIndex;

	try
	{
		if(m_pStoredPokemon == NULL || m_pStoredPokeCodec == NULL)
			throw 0;

		if(!GetVersion())
			throw 0;

		for(dwIndex = 0; dwIndex < STORED_POKEMON_COUNT; ++dwIndex)
		{
			m_pStoredPokeCodec[dwIndex].Encode();
		}

		if(!Write(STORED_POKEMON_ADDRESS[m_dwVersion], STORED_POKEMON_COUNT * sizeof(PokemonStruct), m_pStoredPokemon))
			throw 0;

		for(dwIndex = 0; dwIndex < STORED_POKEMON_COUNT; ++dwIndex)
		{
			m_pStoredPokeCodec[dwIndex].Decode();
		}

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL CPokemonMemHackCore::DecodeFirstActivePokemon(VOID)
{
	BOOL			bResult;
	PokemonStruct	ps;
	CPokemonCodec	pc;

	bResult = GetVersion();
	if(bResult)
	{
		bResult = Read(ACTIVE_POKEMON_ADDRESS[m_dwVersion], sizeof(PokemonStruct), &ps);
	}
	if(bResult)
	{
		pc.Attach(&ps, TRUE);
		pc.Decode();
		bResult = Write(ACTIVE_POKEMON_ADDRESS[m_dwVersion], sizeof(PokemonStruct), &ps);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::EncodeFirstActivePokemon(VOID)
{
	BOOL			bResult;
	PokemonStruct	ps;
	CPokemonCodec	pc;

	bResult = GetVersion();
	if(bResult)
	{
		bResult = Read(ACTIVE_POKEMON_ADDRESS[m_dwVersion], sizeof(PokemonStruct), &ps);
	}
	if(bResult)
	{
		pc.Attach(&ps, FALSE);
		pc.Encode();
		bResult = Write(ACTIVE_POKEMON_ADDRESS[m_dwVersion], sizeof(PokemonStruct), &ps);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GetPlayerId(DWORD &dwId)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		bResult = Read(PLAYER_ID_ADDRESS[m_dwVersion], sizeof(DWORD), &dwId);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SetPlayerId(DWORD dwId)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		bResult = Write(PLAYER_ID_ADDRESS[m_dwVersion], sizeof(DWORD), &dwId);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GetPlayerName(CString &szName)
{
	BOOL	bResult = TRUE;
	BYTE	bCode[POKEMON_TRAINER_NAME_SIZE];

	bResult = GetVersion();
	if(bResult)
	{
		bResult = Read(PLAYER_NAME_ADDRESS[m_dwVersion], sizeof(bCode), bCode);
	}
	if(bResult)
	{
		if(m_dwLang == lang_jp)
			bResult = CodeToString(szName, bCode, 5, 0xFF, m_dwLang);
		else
			bResult = CodeToString(szName, bCode, POKEMON_TRAINER_NAME_SIZE, 0xFF, m_dwLang);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GetPlayerName(BYTE bCode[POKEMON_TRAINER_NAME_SIZE])
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		bResult = Read(PLAYER_NAME_ADDRESS[m_dwVersion], POKEMON_TRAINER_NAME_SIZE, bCode);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SetPlayerName(LPCTSTR szName)
{
	BOOL	bResult = TRUE;
	BYTE	bCode[POKEMON_TRAINER_NAME_SIZE + 1];	// make sure the string is properly terminated by 0xFF

	if(bResult)
	{
		bResult = GetVersion();
	}

	if(bResult)
	{
		if(m_dwLang == lang_jp)
		{
			bResult = StringToCode(szName, bCode, 5, 0xFF, 0xFF, m_dwLang);
			bCode[5] = 0xFF;
			FillMemory(bCode + 6, POKEMON_TRAINER_NAME_SIZE + 1 - 6, 0x00);
		}
		else
		{
			bResult = StringToCode(szName, bCode, POKEMON_TRAINER_NAME_SIZE, 0xFF, 0xFF, m_dwLang);
			bCode[POKEMON_TRAINER_NAME_SIZE] = 0xFF;
		}
	}

	if(bResult)
	{
		bResult = Write(PLAYER_NAME_ADDRESS[m_dwVersion], sizeof(bCode), bCode);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GetMoney(DWORD &dwMoney)
{
	BOOL	bResult = TRUE;
	DWORD	dwXorMask;

	bResult = GetVersion();

	if(bResult)
		bResult = GetXorMask(dwXorMask);

	if(bResult)
		bResult = Read(MONEY_ADDRESS[m_dwVersion], sizeof(DWORD), &dwMoney);

	if(bResult)
		dwMoney ^= dwXorMask;

	return bResult;
}

BOOL CPokemonMemHackCore::SetMoney(DWORD dwMoney)
{
	BOOL	bResult = TRUE;
	DWORD	dwXorMask;

	bResult = GetVersion();

	if(bResult)
		bResult = GetXorMask(dwXorMask);

	if(bResult)
	{
		dwMoney ^= dwXorMask;
		bResult = Write(MONEY_ADDRESS[m_dwVersion], sizeof(DWORD), &dwMoney);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GetCoin(WORD &wCoin)
{
	BOOL	bResult = TRUE;
	DWORD	dwXorMask;

	bResult = GetVersion();

	if(bResult)
		bResult = GetXorMask(dwXorMask);

	if(bResult)
		bResult = Read(COIN_ADDRESS[m_dwVersion], sizeof(WORD), &wCoin);

	if(bResult)
		wCoin ^= LOWORD(dwXorMask);

	return bResult;
}

BOOL CPokemonMemHackCore::SetCoin(WORD wCoin)
{
	BOOL	bResult = TRUE;
	DWORD	dwXorMask;

	bResult = GetVersion();

	if(bResult)
		bResult = GetXorMask(dwXorMask);

	if(bResult)
	{
		wCoin ^= LOWORD(dwXorMask);
		bResult = Write(COIN_ADDRESS[m_dwVersion], sizeof(WORD), &wCoin);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GetDust(WORD &wDust)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();

	if(bResult)
	{
		bResult = Read(DUST_ADDRESS[m_dwVersion], sizeof(WORD), &wDust);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SetDust(WORD wDust)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();

	if(bResult)
	{
		bResult = Write(DUST_ADDRESS[m_dwVersion], sizeof(WORD), &wDust);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GetSprayTime(WORD &wSprayTime)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		bResult = Read(SPRAY_TIME_ADDRESS[m_dwVersion], sizeof(WORD), &wSprayTime);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SetSprayTime(WORD wSprayTime)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		bResult = Write(SPRAY_TIME_ADDRESS[m_dwVersion], sizeof(WORD), &wSprayTime);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GetSafariTime(WORD &wSafariTime)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		bResult = Read(SAFARI_TIME_ADDRESS[m_dwVersion], sizeof(WORD), &wSafariTime);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SetSafariTime(WORD wSafariTime)
{
	BOOL	bResult = TRUE;
	BYTE	bBalls = 0x63;

	bResult = GetVersion();
	if(bResult)
	{
		bResult = Write(SAFARI_TIME_ADDRESS[m_dwVersion], sizeof(WORD), &wSafariTime);
	}
	if(bResult)
	{
		bResult = Write(SAFARI_BALLS_ADDRESS[m_dwVersion], sizeof(BYTE), &bBalls);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GetBattlePointsCurrent(WORD &wBP)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_em_jp:
			bResult = m_vbah.PtrRead(0x03005AF0, 0x00000EB8, sizeof(WORD), &wBP);
			break;
		case mem_em_en:
			bResult = m_vbah.PtrRead(0x03005D90, 0x00000EB8, sizeof(WORD), &wBP);
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SetBattlePointsCurrent(WORD wBP)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_em_jp:
			bResult = m_vbah.PtrWrite(0x03005AF0, 0x00000EB8, sizeof(WORD), &wBP);
			break;
		case mem_em_en:
			bResult = m_vbah.PtrWrite(0x03005D90, 0x00000EB8, sizeof(WORD), &wBP);
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GetBattlePointsTrainerCard(WORD &wBP)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_em_jp:
			bResult = m_vbah.PtrRead(0x03005AF0, 0x00000EBA, sizeof(WORD), &wBP);
			break;
		case mem_em_en:
			bResult = m_vbah.PtrRead(0x03005D90, 0x00000EBA, sizeof(WORD), &wBP);
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SetBattlePointsTrainerCard(WORD wBP)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_em_jp:
			bResult = m_vbah.PtrWrite(0x03005AF0, 0x00000EBA, sizeof(WORD), &wBP);
			break;
		case mem_em_en:
			bResult = m_vbah.PtrWrite(0x03005D90, 0x00000EBA, sizeof(WORD), &wBP);
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SetDaycareCenterStepByte(DWORD dwIndex, BYTE bCount)
{
	BOOL	bResult = FALSE;

	if(dwIndex < 2)
	{
		bResult = GetVersion();
		if(bResult)
		{
			bResult = Write(DAYCARE_CENTER_STEP[m_dwVersion][dwIndex], sizeof(BYTE), &bCount);
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SetStepCounter(BYTE bCount)
{
	BOOL	bResult;

	bResult = GetVersion();
	if(bResult)
		bResult = Write(STEP_COUNTER_ADDRESS[m_dwVersion], sizeof(BYTE), &bCount);

	return bResult;
}

BOOL CPokemonMemHackCore::SetExpGain(SHORT nExpGain)
{
	BOOL	bResult;

	if(nExpGain < 0)
		nExpGain = 0;

	bResult = GetVersion();
	if(bResult)
		bResult = Write(EXP_GAIN_ADDRESS[m_dwVersion], sizeof(SHORT), &nExpGain);

	return bResult;
}

BOOL CPokemonMemHackCore::UpgradePokedex()
{
	BOOL	bResult = TRUE;
	BYTE	bByte;

	bResult = GetVersion();

	// enable various menu items
	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_rs_jp:
			// Pokemon, Pokedex, PokeNavi
			if(bResult)
			{
				bResult = m_vbah.Read(0x020267B4, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x07;
				bResult = m_vbah.Write(0x020267B4, sizeof(BYTE), &bByte);
			}
			// Ribbons(PokeNavi)
			if(bResult)
			{
				bResult = m_vbah.Read(0x020267BB, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x08;
				bResult = m_vbah.Write(0x020267BB, sizeof(BYTE), &bByte);
			}
			// Running Shoes
			if(bResult)
			{
				bResult = m_vbah.Read(0x020267C0, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x01;
				bResult = m_vbah.Write(0x020267C0, sizeof(BYTE), &bByte);
			}
			break;

		case mem_rs_en:
			// Pokemon, Pokedex, PokeNavi
			if(bResult)
			{
				bResult = m_vbah.Read(0x02026A54, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x07;
				bResult = m_vbah.Write(0x02026A54, sizeof(BYTE), &bByte);
			}
			// Ribbons(PokeNavi)
			if(bResult)
			{
				bResult = m_vbah.Read(0x02026A5B, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x08;
				bResult = m_vbah.Write(0x02026A5B, sizeof(BYTE), &bByte);
			}
			// Running Shoes
			if(bResult)
			{
				bResult = m_vbah.Read(0x02026A60, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x01;
				bResult = m_vbah.Write(0x02026A60, sizeof(BYTE), &bByte);
			}
			break;

		case mem_fl_jp:
			// Pokemon, Pokedex, Running Shoes
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x0300504C, 0x00001F89, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x83;
				bResult = m_vbah.PtrWrite(0x0300504C, 0x00001F89, sizeof(BYTE), &bByte);
			}
			break;

		case mem_fl_en:
			// Pokemon, Pokedex, Running Shoes
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x0300500C, 0x00001F89, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x83;
				bResult = m_vbah.PtrWrite(0x0300500C, 0x00001F89, sizeof(BYTE), &bByte);
			}
			break;

		case mem_em_jp:
			// Pokemon, Pokedex, PokeNavi
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x03005AF0, 0x00002328, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x07;
				bResult = m_vbah.PtrWrite(0x03005AF0, 0x00002328, sizeof(BYTE), &bByte);
			}
			// Ribbons(PokeNavi)
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x03005AF0, 0x0000232F, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x08;
				bResult = m_vbah.PtrWrite(0x03005AF0, 0x0000232F, sizeof(BYTE), &bByte);
			}
			// Running Shoes
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x03005AF0, 0x00002334, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x01;
				bResult = m_vbah.PtrWrite(0x03005AF0, 0x00002334, sizeof(BYTE), &bByte);
			}
			break;

		case mem_em_en:
			// Pokemon, Pokedex, PokeNavi
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x03005D90, 0x00002328, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x07;
				bResult = m_vbah.PtrWrite(0x03005D90, 0x00002328, sizeof(BYTE), &bByte);
			}
			// Ribbons(PokeNavi)
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x03005D90, 0x0000232F, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x08;
				bResult = m_vbah.PtrWrite(0x03005D90, 0x0000232F, sizeof(BYTE), &bByte);
			}
			// Running Shoes
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x03005D90, 0x00002334, sizeof(BYTE), &bByte);
			}
			if(bResult)
			{
				bByte |= 0x01;
				bResult = m_vbah.PtrWrite(0x03005D90, 0x00002334, sizeof(BYTE), &bByte);
			}
			break;
		}
	}

	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_rs_jp:
			if(bResult)
			{
				bByte = 0xDA;
				bResult = m_vbah.Write(0x02024C1E, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x67;
				bResult = m_vbah.Write(0x020267BA, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x02;
				bResult = m_vbah.Write(0x02026860, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x03;
				bResult = m_vbah.Write(0x02026861, 1, &bByte);
			}
			break;

		case mem_rs_en:
			if(bResult)
			{
				bByte = 0xDA;
				bResult = m_vbah.Write(0x02024EBE, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x67;
				bResult = m_vbah.Write(0x02026A5A, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x02;
				bResult = m_vbah.Write(0x02026B00, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x03;
				bResult = m_vbah.Write(0x02026B01, 1, &bByte);
			}
			break;

		case mem_fl_jp:
			bByte = 0xB9;
			bResult = m_vbah.PtrWrite(0x0300504C, 0x0000001B, 1, &bByte);
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x0300504C, 0x00001F8C, 1, &bByte);
			}
			if(bResult)
			{
				bByte |= 0x01;
				bResult = m_vbah.PtrWrite(0x0300504C, 0x00001F8C, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x58;
				bResult = m_vbah.PtrWrite(0x0300504C, 0x00002040, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x62;
				bResult = m_vbah.PtrWrite(0x0300504C, 0x00002041, 1, &bByte);
			}
			break;

		case mem_fl_en:
			bByte = 0xB9;
			bResult = m_vbah.PtrWrite(0x0300500C, 0x0000001B, 1, &bByte);
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x0300500C, 0x00001F8C, 1, &bByte);
			}
			if(bResult)
			{
				bByte |= 0x01;
				bResult = m_vbah.PtrWrite(0x0300500C, 0x00001F8C, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x58;
				bResult = m_vbah.PtrWrite(0x0300500C, 0x00002040, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x62;
				bResult = m_vbah.PtrWrite(0x0300500C, 0x00002041, 1, &bByte);
			}
			break;

		case mem_em_jp:
			bResult = m_vbah.PtrRead(0x03005AF0, 0x00000019, 1, &bByte);
			if(bResult)
			{
				bByte |= 0x01;
				bResult = m_vbah.PtrWrite(0x03005AF0, 0x00000019, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0xDA;
				bResult = m_vbah.PtrWrite(0x03005AF0, 0x0000001A, 1, &bByte);
			}
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x03005AF0, 0x0000232E, 1, &bByte);
				bByte |= 0x40;
				bResult = m_vbah.PtrWrite(0x03005AF0, 0x0000232E, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x02;
				bResult = m_vbah.PtrWrite(0x03005AF0, 0x000023D4, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x03;
				bResult = m_vbah.PtrWrite(0x03005AF0, 0x000023D5, 1, &bByte);
			}
			break;

		case mem_em_en:
			bResult = m_vbah.PtrRead(0x03005D90, 0x00000019, 1, &bByte);
			if(bResult)
			{
				bByte |= 0x01;
				bResult = m_vbah.PtrWrite(0x03005D90, 0x00000019, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0xDA;
				bResult = m_vbah.PtrWrite(0x03005D90, 0x0000001A, 1, &bByte);
			}
			if(bResult)
			{
				bResult = m_vbah.PtrRead(0x03005D90, 0x0000232E, 1, &bByte);
				bByte |= 0x40;
				bResult = m_vbah.PtrWrite(0x03005D90, 0x0000232E, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x02;
				bResult = m_vbah.PtrWrite(0x03005D90, 0x000023D4, 1, &bByte);
			}
			if(bResult)
			{
				bByte = 0x03;
				bResult = m_vbah.PtrWrite(0x03005D90, 0x000023D5, 1, &bByte);
			}
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SetFullPokedex()
{
	BOOL	bResult = TRUE;

	static CONST DWORD	dwFlags[] = {
			0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
			0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
			0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
			0x00000003 };

	bResult = GetVersion();
	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_rs_jp:
			bResult = m_vbah.Write(0x02024C2C, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.Write(0x02024C60, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.Write(0x02025DCC, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.Write(0x02028F20, sizeof(dwFlags), dwFlags);
			break;

		case mem_rs_en:
			bResult = m_vbah.Write(0x02024ECC, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.Write(0x02024F00, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.Write(0x0202606C, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.Write(0x020291C0, sizeof(dwFlags), dwFlags);
			break;

		case mem_fl_jp:
			bResult = m_vbah.PtrWrite(0x0300504C, 0x00000028, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x0300504C, 0x0000005C, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x0300504C, 0x0000159C, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x0300504C, 0x000049BC, sizeof(dwFlags), dwFlags);
			break;

		case mem_fl_en:
			bResult = m_vbah.PtrWrite(0x0300500C, 0x00000028, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x0300500C, 0x0000005C, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x0300500C, 0x0000159C, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x0300500C, 0x000049BC, sizeof(dwFlags), dwFlags);
			break;

		case mem_em_jp:
			bResult = m_vbah.PtrWrite(0x03005AF0, 0x00000028, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x03005AF0, 0x0000005C, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x03005AF0, 0x00001934, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x03005AF0, 0x00004AD0, sizeof(dwFlags), dwFlags);
			break;

		case mem_em_en:
			bResult = m_vbah.PtrWrite(0x03005D90, 0x00000028, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x03005D90, 0x0000005C, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x03005D90, 0x00001934, sizeof(dwFlags), dwFlags);
			if(bResult)
				bResult = m_vbah.PtrWrite(0x03005D90, 0x00004AD0, sizeof(dwFlags), dwFlags);
			break;
		}
	}

	return bResult;
}

VOID CPokemonMemHackCore::GetItemType(WORD wItem, DWORD &dwItemType, WORD &wMaxQuantity)
{
	if(wItem >= 0x001 && wItem <= 0x00C)
	{
		dwItemType = pokeball;
		wMaxQuantity = 999;
	}
	else if((wItem >= 0x00D && wItem <= 0x033) ||
			(wItem >= 0x03F && wItem <= 0x047) ||
			(wItem >= 0x049 && wItem <= 0x051) ||
			(wItem >= 0x053 && wItem <= 0x056) ||
			(wItem >= 0x05D && wItem <= 0x062) ||
			(wItem >= 0x067 && wItem <= 0x068) ||
			(wItem >= 0x06A && wItem <= 0x06F) ||
			(wItem >= 0x079 && wItem <= 0x084) ||
			(wItem >= 0x0B3 && wItem <= 0x0E1) ||
			(wItem >= 0x0FE && wItem <= 0x102))
	{
		dwItemType = normalitem;
		wMaxQuantity = 999;
	}
	else if((wItem >= 0x103 && wItem <= 0x10A) ||
			(wItem >= 0x10C && wItem <= 0x120) ||
			(m_dwVersion >= mem_fl_jp && wItem >= 0x15D && wItem <= 0x176) ||
			(m_dwVersion >= mem_em_jp && wItem >= 0x177 && wItem <= 0x178))
	{
		dwItemType = keyitem;
		wMaxQuantity = 1;
	}
	else if(wItem >= 0x085 && wItem <= 0x0AF)
	{
		dwItemType = berry;
		wMaxQuantity = 999;
	}
	else if(wItem >= 0x121 && wItem <= 0x152)
	{
		dwItemType = machine;
		wMaxQuantity = 999;
	}
	else if(wItem >= 0x153 && wItem <= 0x15A)
	{
		dwItemType = machine;
		wMaxQuantity = 1;
	}
	else
	{
		dwItemType = unknownitem;
		wMaxQuantity = 0;
	}
}

BOOL CPokemonMemHackCore::AddItem(WORD wItem, WORD wQuantity)
{
	BOOL		bResult = TRUE;
	DWORD		dwItemType, dwXorMax;
	WORD		wMaxQuantity;
	DWORD		dwBagSize, dwIndex, dwFirstEmpty;
	PokemonItem *pItems = NULL;

	if(wQuantity == 0)
		return TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		GetItemType(wItem, dwItemType, wMaxQuantity);
		if(dwItemType == unknownitem)
			return TRUE;

		if(wQuantity > wMaxQuantity)
			wQuantity = wMaxQuantity;

		dwBagSize = ITEM_ADDRESS[m_dwVersion][dwItemType].dwBagSize;
		pItems = new PokemonItem[dwBagSize];
		if(pItems == NULL)
			bResult = FALSE;
	}

	if(bResult)
	{
		bResult = GetXorMask(dwXorMax);
	}

	if(bResult)
	{
		bResult = Read(ITEM_ADDRESS[m_dwVersion][dwItemType].Address, dwBagSize * sizeof(PokemonItem), pItems);
	}

	if(bResult)
	{
		// unmask items
		for(dwIndex = 0; dwIndex < dwBagSize; ++dwIndex)
		{
			MaskItem(pItems[dwIndex], dwXorMax);
		}

		// check if the specified item exists
		dwFirstEmpty = -1;
		for(dwIndex = 0; dwIndex < dwBagSize; ++dwIndex)
		{
			if(pItems[dwIndex].wItem == wItem)
			{
				if(pItems[dwIndex].wQuantity < wQuantity)
					pItems[dwIndex].wQuantity = wQuantity;
				break;
			}
			if(dwFirstEmpty >= dwBagSize && pItems[dwIndex].wItem == 0)
				dwFirstEmpty = dwIndex;
		}

		// if the item does not exist
		if(dwIndex >= dwBagSize)
		{
			if(dwFirstEmpty < dwBagSize)
			{
				pItems[dwFirstEmpty].wItem = wItem;
				pItems[dwFirstEmpty].wQuantity = wQuantity;
			}
			else
			{
				bResult = FALSE;
			}
		}

		// mask items
		for(dwIndex = 0; dwIndex < dwBagSize; ++dwIndex)
		{
			MaskItem(pItems[dwIndex], dwXorMax);
		}
	}

	if(bResult)
	{
		bResult = Write(ITEM_ADDRESS[m_dwVersion][dwItemType].Address, dwBagSize * sizeof(PokemonItem), pItems);
	}

	if(pItems)
		delete []pItems;

	return bResult;
}

BOOL CPokemonMemHackCore::DropLastKeyItem()
{
	BOOL		bResult = TRUE;
	DWORD		dwItemType = keyitem;
	DWORD		dwXorMax;
	DWORD		dwBagSize, dwIndex;
	PokemonItem *pItems = NULL;

	bResult = GetVersion();
	if(bResult)
	{
		dwBagSize = ITEM_ADDRESS[m_dwVersion][dwItemType].dwBagSize;
		pItems = new PokemonItem[dwBagSize];
		if(pItems == NULL)
			bResult = FALSE;
	}

	if(bResult)
	{
		bResult = GetXorMask(dwXorMax);
	}

	if(bResult)
	{
		bResult = Read(ITEM_ADDRESS[m_dwVersion][dwItemType].Address, dwBagSize * sizeof(PokemonItem), pItems);
	}

	if(bResult)
	{
		// unmask items
		for(dwIndex = 0; dwIndex < dwBagSize; ++dwIndex)
		{
			MaskItem(pItems[dwIndex], dwXorMax);
		}

		// enumerate from the last slot to the first slot
		for(dwIndex = dwBagSize - 1; dwIndex + 1 > 0; --dwIndex)
		{
			if(pItems[dwIndex].wItem != 0)
			{
				pItems[dwIndex].wItem = 0;
				pItems[dwIndex].wQuantity = 0;
				break;
			}
		}

		// mask items
		for(dwIndex = 0; dwIndex < dwBagSize; ++dwIndex)
		{
			MaskItem(pItems[dwIndex], dwXorMax);
		}
	}

	if(bResult)
	{
		bResult = Write(ITEM_ADDRESS[m_dwVersion][dwItemType].Address, dwBagSize * sizeof(PokemonItem), pItems);
	}

	if(pItems)
		delete []pItems;

	return bResult;
}

BOOL CPokemonMemHackCore::IncreaseNormalItems(WORD wQuantity)
{
	BOOL		bResult = TRUE;
	DWORD		dwXorMax;
	DWORD		dwBagSize, dwIndex;
	PokemonItem *pItems = NULL;

	bResult = GetVersion();

	if(bResult)
	{
		dwBagSize = ITEM_ADDRESS[m_dwVersion][normalitem].dwBagSize;
		pItems = new PokemonItem[dwBagSize];
		if(pItems == NULL)
			bResult = FALSE;
	}

	if(bResult)
		bResult = GetXorMask(dwXorMax);

	if(bResult)
		bResult = Read(ITEM_ADDRESS[m_dwVersion][normalitem].Address, dwBagSize * sizeof(PokemonItem), pItems);

	if(bResult)
	{
		for(dwIndex = 0; dwIndex < dwBagSize; ++dwIndex)
		{
			MaskItem(pItems[dwIndex], dwXorMax);
			if(pItems[dwIndex].wItem != 0 && pItems[dwIndex].wQuantity < wQuantity)
			{
				pItems[dwIndex].wQuantity = wQuantity;
			}
			MaskItem(pItems[dwIndex], dwXorMax);
		}
		bResult = Write(ITEM_ADDRESS[m_dwVersion][normalitem].Address, dwBagSize * sizeof(PokemonItem), pItems);
	}

	if(pItems)
		delete []pItems;

	return bResult;
}

BOOL CPokemonMemHackCore::GiveAllPokeball(WORD wQuantity)
{
	BOOL		bResult = TRUE;
	DWORD		dwXorMax;
	WORD		wIndex;
	DWORD		dwBagSize = min(ITEM_ADDRESS[m_dwVersion][pokeball].dwBagSize, 12);
	PokemonItem *pItems = NULL;

	bResult = GetVersion();

	if(bResult)
	{
		pItems = new PokemonItem[dwBagSize];
		if(pItems == NULL)
			bResult = FALSE;
	}

	if(bResult)
		bResult = GetXorMask(dwXorMax);

	if(bResult)
	{
		for(wIndex = 0; wIndex < dwBagSize; ++wIndex)
		{
			pItems[wIndex].wItem = wIndex + 0x001;
			pItems[wIndex].wQuantity = wQuantity;
			MaskItem(pItems[wIndex], dwXorMax);
		}
		bResult = Write(ITEM_ADDRESS[m_dwVersion][pokeball].Address, dwBagSize * sizeof(PokemonItem), pItems);
	}

	if(pItems)
		delete []pItems;

	return bResult;
}

BOOL CPokemonMemHackCore::GiveAllSkillMachines(WORD wQuantity)
{
	BOOL		bResult = TRUE;
	DWORD		dwXorMax;
	DWORD		dwBagSize = min(ITEM_ADDRESS[m_dwVersion][machine].dwBagSize, 58);
	WORD		wIndex;
	PokemonItem *pItems = NULL;

	bResult = GetVersion();

	if(bResult)
	{
		if(m_dwVersion == mem_fl_jp ||
			m_dwVersion == mem_fl_en)
		{
			bResult = AddItem(0x16C, 1);	// わざマシンケース
		}
	}

	if(bResult)
	{
		pItems = new PokemonItem[dwBagSize];
		if(pItems == NULL)
			bResult = FALSE;
	}

	if(bResult)
		bResult = GetXorMask(dwXorMax);

	if(bResult)
	{
		for(wIndex = 0; wIndex < dwBagSize; ++wIndex)
		{
			pItems[wIndex].wItem = wIndex + 0x121;
			if(wIndex < 50)
				pItems[wIndex].wQuantity = wQuantity;
			else
				pItems[wIndex].wQuantity = 1;
			MaskItem(pItems[wIndex], dwXorMax);
		}
		bResult = Write(ITEM_ADDRESS[m_dwVersion][machine].Address, dwBagSize * sizeof(PokemonItem), pItems);
	}

	if(pItems)
		delete []pItems;

	return bResult;
}

BOOL CPokemonMemHackCore::GiveAllBerries(WORD wQuantity)
{
	BOOL		bResult = TRUE;
	DWORD		dwXorMax;
	WORD		wIndex;
	DWORD		dwBagSize = min(ITEM_ADDRESS[m_dwVersion][berry].dwBagSize, 43);
	PokemonItem *pItems = NULL;

	bResult = GetVersion();

	if(bResult)
	{
		if(m_dwVersion == mem_fl_jp ||
			m_dwVersion == mem_fl_en)
		{
			bResult = AddItem(0x16D, 1);	// きのみぶくろ
		}
	}

	if(bResult)
	{
		pItems = new PokemonItem[dwBagSize];
		if(pItems == NULL)
			bResult = FALSE;
	}


	if(bResult)
		bResult = GetXorMask(dwXorMax);

	if(bResult)
	{
		for(wIndex = 0; wIndex < dwBagSize; ++wIndex)
		{
			pItems[wIndex].wItem = wIndex + 0x085;
			pItems[wIndex].wQuantity = wQuantity;
			MaskItem(pItems[wIndex], dwXorMax);
		}
		bResult = Write(ITEM_ADDRESS[m_dwVersion][berry].Address, dwBagSize * sizeof(PokemonItem), pItems);
	}

	if(pItems)
		delete []pItems;

	return bResult;
}

BOOL CPokemonMemHackCore::GiveAllPokeblocks()
{
	BOOL		bResult = TRUE;
	DWORD		dwIndex;
	DWORD		dwBagSize;
	PokemonPokeblock *pPorokku = NULL;

	bResult = GetVersion();

	if(bResult)
	{
		dwBagSize = ITEM_ADDRESS[m_dwVersion][pokeblock].dwBagSize;
		if(dwBagSize == 0)
		{
			bResult = FALSE;
		}
	}

	if(bResult)
		bResult = AddItem(0x111, 1);	// ポロックケース

	if(bResult)
	{
		pPorokku = new PokemonPokeblock[dwBagSize];
		if(pPorokku == NULL)
			bResult = FALSE;
	}

	if(bResult)
	{
		for(dwIndex = 0; dwIndex < dwBagSize; ++dwIndex)
		{
			pPorokku[dwIndex].bColor = (BYTE)((dwIndex % 0x0E) + 1);	// 0x01 - 0x0E
			FillMemory(pPorokku[dwIndex].rgbTastes, 5, 0xFF);
			pPorokku[dwIndex].bWeak = 0;
			pPorokku[dwIndex].bNumberOfPeople = 4;
		}
		bResult = Write(ITEM_ADDRESS[m_dwVersion][pokeblock].Address, dwBagSize * sizeof(PokemonPokeblock), pPorokku);
	}

	if(pPorokku)
		delete []pPorokku;

	return bResult;
}

BOOL CPokemonMemHackCore::GiveAllDecorates()
{
	BOOL			bResult = TRUE;
	BYTE			bIndex;
	BYTE			bBag[40];
	AddressStruct	DecorateAddress;

	bResult = GetVersion();

	if(bResult)
	{
		if(m_dwVersion == mem_fl_jp ||
			m_dwVersion == mem_fl_en)
		{
			bResult = FALSE;
		}
		else
		{
			DecorateAddress = DECORATE_ADDRESS[m_dwVersion];
		}
	}

	// つくえ（9種類）
	if(bResult)
	{
		for(bIndex = 0; bIndex < 9; ++bIndex)
			bBag[bIndex] = 0x01 + bIndex;
		bResult = Write(DecorateAddress, 9, bBag);
	}

	// いす（9種類）
	if(bResult)
	{
		DecorateAddress.dwOffest += 10;
		for(bIndex = 0; bIndex < 9; ++bIndex)
			bBag[bIndex] = 0x0A + bIndex;
		bResult = Write(DecorateAddress, 9, bBag);
	}

	// はちうえ（6種類）
	if(bResult)
	{
		DecorateAddress.dwOffest += 10;
		for(bIndex = 0; bIndex < 6; ++bIndex)
			bBag[bIndex] = 0x13 + bIndex;
		bResult = Write(DecorateAddress, 6, bBag);
	}

	// おきもの（23種類）
	if(bResult)
	{
		DecorateAddress.dwOffest += 10;
		for(bIndex = 0; bIndex < 23; ++bIndex)
			bBag[bIndex] = 0x19 + bIndex;
		bResult = Write(DecorateAddress, 23, bBag);
	}

	// マット（18種類）
	if(bResult)
	{
		DecorateAddress.dwOffest += 30;
		for(bIndex = 0; bIndex < 18; ++bIndex)
			bBag[bIndex] = 0x30 + bIndex;
		bResult = Write(DecorateAddress, 18, bBag);
	}

	// ポスター（10種類）
	if(bResult)
	{
		DecorateAddress.dwOffest += 30;
		for(bIndex = 0; bIndex < 10; ++bIndex)
			bBag[bIndex] = 0x42 + bIndex;
		bResult = Write(DecorateAddress, 10, bBag);
	}

	// ぬいぐるみ（25+10種類）
	if(bResult)
	{
		DecorateAddress.dwOffest += 10;
		for(bIndex = 0; bIndex < 25; ++bIndex)
			bBag[bIndex] = 0x4C + bIndex;
		for(bIndex = 0; bIndex < 10; ++bIndex)
			bBag[bIndex + 25] = 0x6F + bIndex;
		bResult = Write(DecorateAddress, 35, bBag);
	}

	// クッション（10種類）
	if(bResult)
	{
		DecorateAddress.dwOffest += 40;
		for(bIndex = 0; bIndex < 10; ++bIndex)
			bBag[bIndex] = 0x65 + bIndex;
		bResult = Write(DecorateAddress, 10, bBag);
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GiveAllBadges()
{
	BOOL		bResult = TRUE;
	BYTE		bBadge;
	WORD		wBadge;

	bResult = GetVersion();

	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_rs_jp:
		case mem_rs_en:
		case mem_em_jp:
		case mem_em_en:
			bResult = Read(MENU_ADDRESS[m_dwVersion], sizeof(WORD), &wBadge);
			if(bResult)
			{
				wBadge |= 0x7F80;
				bResult = Write(MENU_ADDRESS[m_dwVersion], sizeof(WORD), &wBadge);
			}
			break;

		case mem_fl_jp:
			bBadge = 0xFF;
			bResult = m_vbah.PtrWrite(0x0300504C, 0x00001F88, sizeof(BYTE), &bBadge);
			break;

		case mem_fl_en:
			bBadge = 0xFF;
			bResult = m_vbah.PtrWrite(0x0300500C, 0x00001F88, sizeof(BYTE), &bBadge);
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::EnableAllTickets()
{
	BOOL	bResult;
	BYTE	b;
	AddressStruct	as;

	bResult = GetVersion();

	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_rs_jp:
			as.dwAccessType = direct;
			as.dwAddress = 0x020267B4;
			as.dwOffest = 0;
			break;

		case mem_rs_en:
			as.dwAccessType = direct;
			as.dwAddress = 0x02026A54;
			as.dwOffest = 0;
			break;

		case mem_em_jp:
			as.dwAccessType = pointer;
			as.dwAddress = 0x03005AF0;
			as.dwOffest = 0x00002328;
			break;

		case mem_em_en:
			as.dwAccessType = pointer;
			as.dwAddress = 0x03005D90;
			as.dwOffest = 0x00002328;
			break;

		case mem_fl_jp:
			as.dwAccessType = pointer;
			as.dwAddress = 0x0300504C;
			as.dwOffest = 0x00001ECA;
			break;

		case mem_fl_en:
			as.dwAccessType = pointer;
			as.dwAddress = 0x0300500C;
			as.dwOffest = 0x00001ECA;
			break;
		}

		switch(m_dwVersion)
		{
		case mem_rs_jp:
		case mem_rs_en:
			// ふねのチケット（連絡船）
			bResult = Read(as, sizeof(BYTE), &b);
			if(bResult)
			{
				b |= 0x10;
				bResult = Write(as, sizeof(BYTE), &b);
			}
			// むげんのチケット（南の孤島･･･ラディアス or ラディオス）
			as.dwOffest += 0x0000000A;
			if(bResult)
				bResult = Read(as, sizeof(BYTE), &b);
			if(bResult)
			{
				b |= 0x08;
				bResult = Write(as, sizeof(BYTE), &b);
			}
			break;

		case mem_fl_jp:
		case mem_fl_en:
			// ふねのチケット（ＳＴ・アンヌ号）
			bResult = Read(as, sizeof(BYTE), &b);
			if(bResult)
			{
				b |= 0x10;
				bResult = Write(as, sizeof(BYTE), &b);
			}
			// １～３の島のマップ、４～７の島のマップ
			as.dwOffest = 0x00001F8C;
			bResult = Read(as, sizeof(BYTE), &b);
			if(bResult)
			{
				b |= 0x60;
				bResult = Write(as, sizeof(BYTE), &b);
			}
			// しんぴのチケット（臍の岩･･･ルギアとホウオウ）、オーロラチケット（誕生の島･･･ディオキシス）
			as.dwOffest = 0x00001F8D;
			if(bResult)
				bResult = Read(as, sizeof(BYTE), &b);
			if(bResult)
			{
				b |= 0x0C;
				bResult = Write(as, sizeof(BYTE), &b);
			}
			// トライパス（１～３の島）、レンボーパス（４～７の島）
			as.dwOffest = 0x00002090;
			if(bResult)
				bResult = Read(as, sizeof(BYTE), &b);
			if(bResult)
			{
				b |= 0x06;
				bResult = Write(as, sizeof(BYTE), &b);
			}
			break;

		case mem_em_jp:
		case mem_em_en:
			// ふねのチケット（連絡船）
			bResult = Read(as, sizeof(BYTE), &b);
			if(bResult)
			{
				b |= 0x10;
				bResult = Write(as, sizeof(BYTE), &b);
			}
			// ふねのチケット（バトルフロンティア）
			as.dwOffest = 0x00002256;
			bResult = Read(as, sizeof(BYTE), &b);
			if(bResult)
			{
				b |= 0x01;
				bResult = Write(as, sizeof(BYTE), &b);
			}
			// むげんのチケット（南の孤島･･･ラディオス）
			as.dwOffest = 0x00002332;
			if(bResult)
				bResult = Read(as, sizeof(BYTE), &b);
			if(bResult)
			{
				b |= 0x08;
				bResult = Write(as, sizeof(BYTE), &b);
			}
			// オーロラチケット（誕生の島･･･ディオキシス）、古びた海図（最果ての孤島･･･ミュウ）
			as.dwOffest = 0x00002336;
			if(bResult)
				bResult = Read(as, sizeof(BYTE), &b);
			if(bResult)
			{
				b |= 0x60;
				bResult = Write(as, sizeof(BYTE), &b);
			}
			// しんぴのチケット（臍の岩･･･ルギアとホウオウ）
			as.dwOffest = 0x00002338;
			if(bResult)
				bResult = Read(as, sizeof(BYTE), &b);
			if(bResult)
			{
				b |= 0x01;
				bResult = Write(as, sizeof(BYTE), &b);
			}
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::EnableMirageIsland()
{
	BOOL		bResult = FALSE;
	DWORD		dwCount, dwIndex, dwRand;
	WORD		wNumber;

	bResult = GetVersion();
	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_rs_jp:
		case mem_rs_en:
		case mem_em_jp:
		case mem_em_en:
			bResult = ReadActivePokemons();

			if(bResult)
			{
				// get the count of active pokemons
				for(dwIndex = dwCount = 0; dwIndex < ACTIVE_POKEMON_COUNT; ++dwIndex)
				{
					if(m_pActivePokeCodec[dwIndex].GetBreed() > 0)
						++dwCount;
				}

				if(dwCount == 0)
					break;

				// choose a pokemon randomly
				dwRand = (GenLongRandom() % dwCount) + 1;
				for(dwIndex = dwCount = 0; dwIndex < ACTIVE_POKEMON_COUNT; ++dwIndex)
				{
					if(m_pActivePokeCodec[dwIndex].GetBreed() > 0)
					{
						++dwCount;
						if(dwCount == dwRand)
						{
							wNumber = (WORD)(m_pActivePokeCodec[dwIndex].GetChar() & 0xFFFF);
							break;
						}
					}
				}

				// set the low WORD of "PER_DAY_RANDOM_DWORD" to the low WORD of the char of the chosen pokemon
				bResult = Write(PER_DAY_RANDOM_DWORD_ADDRESS[m_dwVersion], sizeof(WORD), &wNumber);
			}
			break;

		// does not exist in fire red/leaf green
		case mem_fl_jp:
		case mem_fl_en:
			bResult = FALSE;
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::AddItemToBattlePyramid50(WORD wItem, BYTE bQuantity)
{
	BOOL	bResult = FALSE;
	BOOL	bFound = FALSE, bModified = FALSE;
	DWORD	dwItemType, dwIndex;
	WORD	wMaxQuantity;
	WORD	wItems[10];
	BYTE	bQuantities[10];
	DWORD_PTR	dwBasePointer;

	bResult = GetVersion();
	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_em_jp:
		case mem_em_en:
			if(m_dwVersion == mem_em_jp)
				dwBasePointer = 0x03005AF0;
			else if(m_dwVersion == mem_em_en)
				dwBasePointer = 0x03005D90;

			// is the item a valid item?
			GetItemType(wItem, dwItemType, wMaxQuantity);
			if(wMaxQuantity == 0)
			{
				bResult = FALSE;
				break;
			}
			// make sure the quantity is legal
			if(bQuantity > wMaxQuantity)
				bQuantity = (BYTE)(min(wMaxQuantity, 99));

			// read items and quantities
			bResult = m_vbah.PtrRead(dwBasePointer, 0x00000E2C, 10 * sizeof(WORD), wItems);
			if(bResult)
				bResult = m_vbah.PtrRead(dwBasePointer, 0x00000E54, 10 * sizeof(BYTE), bQuantities);

			if(bResult)
			{
				// if the item alread exists, update its quantity
				for(dwIndex = 0; dwIndex < 10; ++dwIndex)
				{
					if(wItems[dwIndex] == wItem)
					{
						bFound = TRUE;
						if(bQuantities[dwIndex] < bQuantity)
						{
							bQuantities[dwIndex] = bQuantity;
							bModified = TRUE;
						}
						break;
					}
				}

				// find an empty item slot, and add the item
				if(!bFound)
				{
					for(dwIndex = 0; dwIndex < 10; ++dwIndex)
					{
						if(wItems[dwIndex] == 0 || bQuantities[dwIndex] == 0)
						{
							wItems[dwIndex] = wItem;
							bQuantities[dwIndex] = bQuantity;
							bModified = TRUE;
							break;
						}
					}
				}

				// if the item is added, write the modified items and quantities
				if(bModified)
				{
					bResult = m_vbah.PtrWrite(dwBasePointer, 0x00000E2C, 10 * sizeof(WORD), wItems);
					if(bResult)
						bResult = m_vbah.PtrWrite(dwBasePointer, 0x00000E54, 10 * sizeof(BYTE), bQuantities);
				}
			}
			break;

		default:
			bResult = FALSE;
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::AddItemToBattlePyramid60(WORD wItem, BYTE bQuantity)
{
	BOOL	bResult = FALSE;
	BOOL	bFound = FALSE, bModified = FALSE;
	DWORD	dwItemType, dwIndex;
	WORD	wMaxQuantity;
	WORD	wItems[10];
	BYTE	bQuantities[10];
	DWORD_PTR	dwBasePointer;

	bResult = GetVersion();
	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_em_jp:
		case mem_em_en:
			if(m_dwVersion == mem_em_jp)
				dwBasePointer = 0x03005AF0;
			else if(m_dwVersion == mem_em_en)
				dwBasePointer = 0x03005D90;

			// is the item a valid item?
			GetItemType(wItem, dwItemType, wMaxQuantity);
			if(wMaxQuantity == 0)
			{
				bResult = FALSE;
				break;
			}
			// make sure the quantity is legal
			if(bQuantity > wMaxQuantity)
				bQuantity = (BYTE)(min(wMaxQuantity, 99));

			// read items and quantities
			bResult = m_vbah.PtrRead(dwBasePointer, 0x00000E40, 10 * sizeof(WORD), wItems);
			if(bResult)
				bResult = m_vbah.PtrRead(dwBasePointer, 0x00000E5E, 10 * sizeof(BYTE), bQuantities);

			if(bResult)
			{
				// if the item alread exists, update its quantity
				for(dwIndex = 0; dwIndex < 10; ++dwIndex)
				{
					if(wItems[dwIndex] == wItem)
					{
						bFound = TRUE;
						if(bQuantities[dwIndex] < bQuantity)
						{
							bQuantities[dwIndex] = bQuantity;
							bModified = TRUE;
						}
						break;
					}
				}

				// find an empty item slot, and add the item
				if(!bFound)
				{
					for(dwIndex = 0; dwIndex < 10; ++dwIndex)
					{
						if(wItems[dwIndex] == 0 || bQuantities[dwIndex] == 0)
						{
							wItems[dwIndex] = wItem;
							bQuantities[dwIndex] = bQuantity;
							bModified = TRUE;
							break;
						}
					}
				}

				// if the item is added, write the modified items and quantities
				if(bModified)
				{
					bResult = m_vbah.PtrWrite(dwBasePointer, 0x00000E40, 10 * sizeof(WORD), wItems);
					if(bResult)
						bResult = m_vbah.PtrWrite(dwBasePointer, 0x00000E5E, 10 * sizeof(BYTE), bQuantities);
				}
			}
			break;

		default:
			bResult = FALSE;
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::SetBattlePyramidViewRadius(BYTE bRadius)
{
	BOOL	bResult = FALSE;

	bResult = GetVersion();
	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_em_jp:
			if(bRadius < 0x20)
				bRadius = 0x20;
			else if(bRadius > 0x78)
				bRadius = 0x78;
			bResult = m_vbah.PtrWrite(0x03005AF0, 0x00000E68, sizeof(BYTE), &bRadius);
			break;

		case mem_em_en:
			if(bRadius < 0x20)
				bRadius = 0x20;
			else if(bRadius > 0x78)
				bRadius = 0x78;
			bResult = m_vbah.PtrWrite(0x03005D90, 0x00000E68, sizeof(BYTE), &bRadius);
			break;

		default:
			bResult = FALSE;
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::AdjustClock()
{
	BOOL	bResult = FALSE;
	PmClockAdjustment	ca;

	bResult = GetVersion();

	if(bResult)
	{
		switch(m_dwVersion)
		{
		case mem_rs_jp:
		case mem_rs_en:
		case mem_em_jp:
		case mem_em_en:
			bResult = Read(CLOCK_ADJUSTMENT_ADDRESS[m_dwVersion], sizeof(PmClockAdjustment), &ca);
			if(bResult)
			{
				ca.bHour = 0;
				ca.bMinute = 0;
				ca.bSecond = 0;
				bResult = Write(CLOCK_ADJUSTMENT_ADDRESS[m_dwVersion], sizeof(PmClockAdjustment), &ca);
			}
			break;

		default:
			bResult = FALSE;
			break;
		}
	}

	return bResult;
}

BOOL CPokemonMemHackCore::GetXorMask(DWORD &dwXorMask)
{
	BOOL	bResult = TRUE;

	bResult = GetVersion();
	if(bResult)
	{
		dwXorMask = 0x00000000;
		bResult = Read(XOR_MASK_ADDRESS[m_dwVersion], sizeof(DWORD), &dwXorMask);
	}

	return bResult;
}

VOID CPokemonMemHackCore::MaskItem(PokemonItem &Item, DWORD dwXorMask)
{
	Item.wQuantity ^= LOWORD(dwXorMask);
}
