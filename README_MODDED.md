# Modded Features Guide

This document describes custom content added to this pokeemerald fork and where to edit each configurable piece.

---

## Cat League Challenge (Cosme Reward)

Beat the Pokémon League and enter the Hall of Fame with a party made entirely of cat Pokémon (including evolutions). Then talk to the NPC beside your house in Littleroot Town to receive a special **Cosme** with custom moves.

### How it works

1. When you become Champion, `CheckLeagueWonWithCatsOnly` runs during the Hall of Fame sequence.
2. It inspects `gPlayerParty` — the same party recorded in the Hall of Fame.
3. Every non-empty slot must be a cat species (eggs fail the check).
4. If valid, `FLAG_LEAGUE_WON_CATS_ONLY` is set permanently.
5. The Littleroot NPC checks that flag and gives Cosme once (`FLAG_RECEIVED_COSME_REWARD`).

### Cat species definition

**File:** `src/data/cat_species.h`

```c
static const u16 sBaseCatSpecies[] =
{
    SPECIES_MEOWTH,
    SPECIES_SKITTY,
    SPECIES_ZANGOOSE,
    SPECIES_ABSOL,
    SPECIES_COSME,
    SPECIES_TITA,
};
```

- Add or remove **base** cat species here.
- Evolutions are resolved automatically (e.g. `SPECIES_PERSIAN`, `SPECIES_DELCATTY`).
- Any species in the same evolution chain as a listed base cat counts as a cat.

### Reward Pokémon: level and moves

**File:** `src/league_cat_challenge.c` (top of file)

| Define | Current value | Description |
|--------|---------------|-------------|
| `COSME_REWARD_LEVEL` | `60` | Level of the gifted Cosme |
| `COSME_REWARD_MOVE_1` | `MOVE_EXPLOSION` | Slot 1 (250 power) |
| `COSME_REWARD_MOVE_2` | `MOVE_SELF_DESTRUCT` | Slot 2 (200 power) |
| `COSME_REWARD_MOVE_3` | `MOVE_HYPER_BEAM` | Slot 3 (150 power) |
| `COSME_REWARD_MOVE_4` | `MOVE_ERUPTION` | Slot 4 (150 power) |

Move IDs are in `include/constants/moves.h`. The gift ignores learnsets; any Gen 3 move constant works.

To change the reward species, edit `SPECIES_COSME` inside `GiveCosmeLeagueReward()` in the same file.

### NPC dialogues

**File:** `data/scripts/cat_league_reward.inc`

| Label | When shown |
|-------|------------|
| `CatLeagueReward_Text_WaitingForCatTrainer` | Before becoming Champion |
| `CatLeagueReward_Text_ChampionNotEligible` | Champion, but party wasn't all cats |
| `CatLeagueReward_Text_EligibleIntro` | Eligible for reward |
| `CatLeagueReward_Text_ObtainedCosme` | Fanfare message |
| `CatLeagueReward_Text_AfterReward` | After receiving Cosme |
| `CatLeagueReward_Text_AlreadyReceived` | Reward already claimed |

Edit the `.string` blocks directly. Follow existing `\p` / `\l` / `\n` formatting.

### NPC location and sprite

**Map object:** `data/maps/LittlerootTown/map.json`  
- Object: `LOCALID_LITTLEROOT_CAT_TRAINER`  
- Default position: `(3, 9)` beside Brendan's house  

**Position script:** `data/maps/LittlerootTown/scripts.inc`  
- `LittlerootTown_EventScript_SetCatTrainerNpcPos`  
- Male player: `(3, 9)`, faces right  
- Female player: `(16, 9)`, faces left (beside May's house)  

**Sprite:** `OBJ_EVENT_GFX_OLD_WOMAN` in `map.json`. Change `graphics_id` to any `OBJ_EVENT_GFX_*` from `include/constants/event_objects.h`.

### Flags

**File:** `include/constants/flags.h`

| Flag | Purpose |
|------|---------|
| `FLAG_LEAGUE_WON_CATS_ONLY` | Set when HoF party is all cats |
| `FLAG_RECEIVED_COSME_REWARD` | Set after Cosme is received |

### Engine hooks

| File | Role |
|------|------|
| `src/league_cat_challenge.c` | Core logic |
| `include/league_cat_challenge.h` | Public API |
| `data/specials.inc` | Registers `CheckLeagueWonWithCatsOnly`, `GiveCosmeLeagueReward` |
| `data/scripts/hall_of_fame.inc` | Calls check on game clear |
| `data/event_scripts.s` | Includes `cat_league_reward.inc` |

### Rebuild

```sh
make -j$(nproc)
```

---

## Adding more challenge paradigms

Use this pattern for future rewards:

1. Define challenge logic in `src/` with a `Check*` special.
2. Hook the check at the right story moment (HoF, battle end, etc.).
3. Add flags in `include/constants/flags.h`.
4. Register specials in `data/specials.inc`.
5. Place an NPC with dialogue branches in `data/scripts/`.
6. Document editable values in this file.
