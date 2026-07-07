#include "global.h"
#include "event_data.h"
#include "pokedex.h"
#include "pokemon.h"
#include "constants/flags.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "data/cat_species.h"
#include "league_cat_challenge.h"

extern const struct Evolution gEvolutionTable[][EVOS_PER_MON];

// Edit reward level and moves here (also documented in README_MODDED.md).
#define COSME_REWARD_LEVEL 60
#define COSME_REWARD_MOVE_1 MOVE_EXPLOSION
#define COSME_REWARD_MOVE_2 MOVE_SELF_DESTRUCT
#define COSME_REWARD_MOVE_3 MOVE_HYPER_BEAM
#define COSME_REWARD_MOVE_4 MOVE_ERUPTION

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

    result = GiveMonToPlayer(&mon);
    nationalDexNum = SpeciesToNationalPokedexNum(SPECIES_COSME);

    if (result == MON_GIVEN_TO_PARTY || result == MON_GIVEN_TO_PC)
    {
        GetSetPokedexFlag(nationalDexNum, FLAG_SET_SEEN);
        GetSetPokedexFlag(nationalDexNum, FLAG_SET_CAUGHT);
    }

    return result;
}
