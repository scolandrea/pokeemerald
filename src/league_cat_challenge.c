#include "global.h"
#include "event_data.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "string_util.h"
#include "constants/flags.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "data/cat_species.h"
#include "league_cat_challenge.h"
#include "tv.h"

extern const struct Evolution gEvolutionTable[][EVOS_PER_MON];

// Edit reward level and moves here (also documented in README_MODDED.md).
#define COSME_REWARD_LEVEL 60
#define COSME_REWARD_MOVE_1 MOVE_EXPLOSION
#define COSME_REWARD_MOVE_2 MOVE_SELF_DESTRUCT
#define COSME_REWARD_MOVE_3 MOVE_HYPER_BEAM
#define COSME_REWARD_MOVE_4 MOVE_ERUPTION

#define COSME_REWARD_IV_HP     31
#define COSME_REWARD_IV_ATK    31
#define COSME_REWARD_IV_DEF    USE_RANDOM_IVS
#define COSME_REWARD_IV_SPEED  31
#define COSME_REWARD_IV_SPATK  31
#define COSME_REWARD_IV_SPDEF  USE_RANDOM_IVS

#define TITA_REWARD_LEVEL 60
#define TITA_REWARD_MOVE_1 MOVE_REST
#define TITA_REWARD_MOVE_2 MOVE_SLASH
#define TITA_REWARD_MOVE_3 MOVE_PAY_DAY
#define TITA_REWARD_MOVE_4 MOVE_WISH

#define TITA_REWARD_IV_HP     31
#define TITA_REWARD_IV_ATK    31
#define TITA_REWARD_IV_DEF    31
#define TITA_REWARD_IV_SPEED  31
#define TITA_REWARD_IV_SPATK  USE_RANDOM_IVS
#define TITA_REWARD_IV_SPDEF  USE_RANDOM_IVS

#define TITA_CUTE_RIBBON_RANK 4

// Wild Hoenn cats for the living dex reward. Cosme and Tita are gift-only.
static const u16 sLivingDexCatSpecies[] =
{
    SPECIES_MEOWTH,
    SPECIES_PERSIAN,
    SPECIES_SKITTY,
    SPECIES_DELCATTY,
    SPECIES_ZANGOOSE,
    SPECIES_ABSOL,
};

static EWRAM_DATA bool8 sCatSpeciesCacheBuilt = FALSE;
static EWRAM_DATA bool8 sIsCatSpecies[NUM_SPECIES];

static void BuildCatSpeciesCache(void)
{
    u16 queue[NUM_SPECIES];
    u16 head;
    u16 tail;
    u32 i;
    u16 species;

    for (i = 0; i < NUM_SPECIES; i++)
        sIsCatSpecies[i] = FALSE;

    for (i = 0; i < ARRAY_COUNT(sBaseCatSpecies); i++)
    {
        species = sBaseCatSpecies[i];
        if (species == SPECIES_NONE || species >= NUM_SPECIES)
            continue;

        head = 0;
        tail = 0;
        queue[tail++] = species;
        sIsCatSpecies[species] = TRUE;

        while (head < tail)
        {
            u16 current = queue[head++];
            u16 s;
            u32 j;

            for (j = 0; j < EVOS_PER_MON; j++)
            {
                u16 evo = gEvolutionTable[current][j].targetSpecies;

                if (evo != SPECIES_NONE && evo < NUM_SPECIES && !sIsCatSpecies[evo])
                {
                    sIsCatSpecies[evo] = TRUE;
                    queue[tail++] = evo;
                }
            }

            for (s = 1; s < NUM_SPECIES; s++)
            {
                for (j = 0; j < EVOS_PER_MON; j++)
                {
                    if (gEvolutionTable[s][j].targetSpecies == current && !sIsCatSpecies[s])
                    {
                        sIsCatSpecies[s] = TRUE;
                        queue[tail++] = s;
                    }
                }
            }
        }
    }

    sCatSpeciesCacheBuilt = TRUE;
}

bool8 IsCatSpecies(u16 species)
{
    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return FALSE;

    if (!sCatSpeciesCacheBuilt)
        BuildCatSpeciesCache();

    return sIsCatSpecies[species];
}

static bool8 IsBoxMonOwnedByPlayer(struct BoxPokemon *boxMon)
{
    u32 otId;
    u8 otName[PLAYER_NAME_LENGTH + 1];

    if (GetBoxMonData(boxMon, MON_DATA_SPECIES) == SPECIES_NONE)
        return FALSE;

    if (GetBoxMonData(boxMon, MON_DATA_IS_EGG))
        return FALSE;

    otId = GetBoxMonData(boxMon, MON_DATA_OT_ID);
    if (otId != GetPlayerIDAsU32())
        return FALSE;

    GetBoxMonData(boxMon, MON_DATA_OT_NAME, otName);
    if (StringCompare(otName, gSaveBlock2Ptr->playerName) != 0)
        return FALSE;

    return TRUE;
}

static bool8 PlayerOwnsSpecies(u16 species)
{
    u32 i;
    u32 j;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) == SPECIES_NONE)
            continue;

        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) == species
            && IsBoxMonOwnedByPlayer(&gPlayerParty[i].box))
            return TRUE;
    }

    for (i = 0; i < TOTAL_BOXES_COUNT; i++)
    {
        for (j = 0; j < IN_BOX_COUNT; j++)
        {
            if (GetBoxMonData(&gPokemonStoragePtr->boxes[i][j], MON_DATA_SPECIES) == species
                && IsBoxMonOwnedByPlayer(&gPokemonStoragePtr->boxes[i][j]))
                return TRUE;
        }
    }

    return FALSE;
}

u8 CountPlayerCatLivingDexSpecies(void)
{
    u32 i;
    u8 count = 0;

    for (i = 0; i < ARRAY_COUNT(sLivingDexCatSpecies); i++)
    {
        if (PlayerOwnsSpecies(sLivingDexCatSpecies[i]))
            count++;
    }

    return count;
}

void CheckLeagueWonWithCatsOnly(void)
{
    u32 i;
    bool8 hasMon = FALSE;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG);

        if (species == SPECIES_NONE)
            continue;

        hasMon = TRUE;

        if (species == SPECIES_EGG || !IsCatSpecies(species))
            return;
    }

    if (hasMon)
        FlagSet(FLAG_LEAGUE_WON_CATS_ONLY);
}

static void SetRewardMonIv(struct Pokemon *mon, u8 stat, u8 ivConfig)
{
    u8 iv;

    if (ivConfig == USE_RANDOM_IVS)
        return;

    iv = ivConfig;
    SetMonData(mon, MON_DATA_HP_IV + stat, &iv);
}

static void ApplyRewardMonIvs(struct Pokemon *mon, u8 hp, u8 atk, u8 def, u8 speed, u8 spAtk, u8 spDef)
{
    SetRewardMonIv(mon, STAT_HP, hp);
    SetRewardMonIv(mon, STAT_ATK, atk);
    SetRewardMonIv(mon, STAT_DEF, def);
    SetRewardMonIv(mon, STAT_SPEED, speed);
    SetRewardMonIv(mon, STAT_SPATK, spAtk);
    SetRewardMonIv(mon, STAT_SPDEF, spDef);
    CalculateMonStats(mon);
}

static void ApplyCosmeRewardTraits(struct Pokemon *mon)
{
    u8 ribbon = TRUE;

    ApplyRewardMonIvs(mon,
        COSME_REWARD_IV_HP,
        COSME_REWARD_IV_ATK,
        COSME_REWARD_IV_DEF,
        COSME_REWARD_IV_SPEED,
        COSME_REWARD_IV_SPATK,
        COSME_REWARD_IV_SPDEF);
    SetMonData(mon, MON_DATA_WINNING_RIBBON, &ribbon);
}

static void ApplyTitaRewardTraits(struct Pokemon *mon)
{
    u8 cuteRibbon = TITA_CUTE_RIBBON_RANK;

    ApplyRewardMonIvs(mon,
        TITA_REWARD_IV_HP,
        TITA_REWARD_IV_ATK,
        TITA_REWARD_IV_DEF,
        TITA_REWARD_IV_SPEED,
        TITA_REWARD_IV_SPATK,
        TITA_REWARD_IV_SPDEF);
    SetMonData(mon, MON_DATA_CUTE_RIBBON, &cuteRibbon);
}

u16 GiveCosmeLeagueReward(void)
{
    struct Pokemon mon;
    u8 result;
    u16 nationalDexNum;

    CreateMon(&mon, SPECIES_COSME, COSME_REWARD_LEVEL, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    SetMonMoveSlot(&mon, COSME_REWARD_MOVE_1, 0);
    SetMonMoveSlot(&mon, COSME_REWARD_MOVE_2, 1);
    SetMonMoveSlot(&mon, COSME_REWARD_MOVE_3, 2);
    SetMonMoveSlot(&mon, COSME_REWARD_MOVE_4, 3);
    ApplyCosmeRewardTraits(&mon);

    result = GiveMonToPlayer(&mon);
    nationalDexNum = SpeciesToNationalPokedexNum(SPECIES_COSME);

    if (result == MON_GIVEN_TO_PARTY || result == MON_GIVEN_TO_PC)
    {
        GetSetPokedexFlag(nationalDexNum, FLAG_SET_SEEN);
        GetSetPokedexFlag(nationalDexNum, FLAG_SET_CAUGHT);
    }

    return result;
}

u16 GiveTitaLivingDexReward(void)
{
    struct Pokemon mon;
    u8 result;
    u16 nationalDexNum;

    CreateMon(&mon, SPECIES_TITA, TITA_REWARD_LEVEL, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    SetMonMoveSlot(&mon, TITA_REWARD_MOVE_1, 0);
    SetMonMoveSlot(&mon, TITA_REWARD_MOVE_2, 1);
    SetMonMoveSlot(&mon, TITA_REWARD_MOVE_3, 2);
    SetMonMoveSlot(&mon, TITA_REWARD_MOVE_4, 3);
    ApplyTitaRewardTraits(&mon);

    result = GiveMonToPlayer(&mon);
    nationalDexNum = SpeciesToNationalPokedexNum(SPECIES_TITA);

    if (result == MON_GIVEN_TO_PARTY || result == MON_GIVEN_TO_PC)
    {
        GetSetPokedexFlag(nationalDexNum, FLAG_SET_SEEN);
        GetSetPokedexFlag(nationalDexNum, FLAG_SET_CAUGHT);
    }

    return result;
}
