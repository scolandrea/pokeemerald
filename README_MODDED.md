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

### Reward Pokémon: IVs and ribbon

Four IVs are fixed at 31; the other two stay random (`USE_RANDOM_IVS`).

| Define | Value | Stat |
|--------|-------|------|
| `COSME_REWARD_IV_HP` | `31` | HP |
| `COSME_REWARD_IV_ATK` | `31` | Attack |
| `COSME_REWARD_IV_DEF` | `USE_RANDOM_IVS` | Defense (random) |
| `COSME_REWARD_IV_SPEED` | `31` | Speed |
| `COSME_REWARD_IV_SPATK` | `31` | Sp. Attack |
| `COSME_REWARD_IV_SPDEF` | `USE_RANDOM_IVS` | Sp. Defense (random) |

| Trait | Value |
|-------|-------|
| Ribbon | `MON_DATA_WINNING_RIBBON` (Winning Ribbon) |

Use `0`–`31` for a fixed IV, or `USE_RANDOM_IVS` (`32`) to leave that stat random.

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

**Event script usage** (`data/scripts/cat_league_reward.inc`):

| Flag | Set by | Checked by |
|------|--------|------------|
| `FLAG_LEAGUE_WON_CATS_ONLY` | `CheckLeagueWonWithCatsOnly()` in `src/league_cat_challenge.c` (Hall of Fame) | NPC script: `goto_if_set` before giving reward |
| `FLAG_RECEIVED_COSME_REWARD` | NPC script: `setflag` after Cosme is received | NPC script: `goto_if_set` at start (already claimed branch) |
| `FLAG_IS_CHAMPION` | Engine (vanilla) | NPC script: `goto_if_set` to show “Champion but not eligible” dialogue |

To debug or skip the challenge, set/clear these flags with a save editor or a test script (`setflag` / `clearflag`).

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

## Cat Living Dex Challenge (Tita Reward)

Collect one of each wild Hoenn cat species—owned by the player, not traded—then talk to the collector NPC in Littleroot Town to receive a special **Tita** with custom moves.

### Required species (living dex)

These six must be present at once in the party and/or PC boxes:

| Species |
|---------|
| Meowth |
| Persian |
| Skitty |
| Delcatty |
| Zangoose |
| Absol |

**Cosme and Tita are not part of this list.** They are gift-only Pokémon and will eventually be removed from wild encounters.

Each qualifying Pokémon must match the player's OT ID **and** OT name (traded Pokémon do not count).

### How it works

1. When the player talks to the collector NPC, `CountPlayerCatLivingDexSpecies` scans party + all PC boxes.
2. For each of the six species above, it looks for at least one non-egg Pokémon with the player's OT.
3. If the count is 6, the NPC offers the reward; otherwise it shows progress (`X of 6`).
4. `GiveTitaLivingDexReward` creates the gift Tita and sets `FLAG_RECEIVED_TITA_REWARD`.

### Reward Pokémon: level and moves

**File:** `src/league_cat_challenge.c` (top of file)

| Define | Current value | Description |
|--------|---------------|-------------|
| `TITA_REWARD_LEVEL` | `60` | Level of the gifted Tita |
| `TITA_REWARD_MOVE_1` | `MOVE_REST` | Slot 1 |
| `TITA_REWARD_MOVE_2` | `MOVE_SLASH` | Slot 2 |
| `TITA_REWARD_MOVE_3` | `MOVE_PAY_DAY` | Slot 3 |
| `TITA_REWARD_MOVE_4` | `MOVE_WISH` | Slot 4 |

To change the living dex list, edit `sLivingDexCatSpecies[]` in the same file. To change the reward species, edit `SPECIES_TITA` inside `GiveTitaLivingDexReward()`.

### Reward Pokémon: IVs and ribbon

Four IVs are fixed at 31; the other two stay random (`USE_RANDOM_IVS`).

| Define | Value | Stat |
|--------|-------|------|
| `TITA_REWARD_IV_HP` | `31` | HP |
| `TITA_REWARD_IV_ATK` | `31` | Attack |
| `TITA_REWARD_IV_DEF` | `31` | Defense |
| `TITA_REWARD_IV_SPEED` | `31` | Speed |
| `TITA_REWARD_IV_SPATK` | `USE_RANDOM_IVS` | Sp. Attack (random) |
| `TITA_REWARD_IV_SPDEF` | `USE_RANDOM_IVS` | Sp. Defense (random) |

| Trait | Value |
|-------|-------|
| Ribbon | `MON_DATA_CUTE_RIBBON` = `4` (Cute Contest Master Rank) |

Use `0`–`31` for a fixed IV, or `USE_RANDOM_IVS` (`32`) to leave that stat random. Change `TITA_CUTE_RIBBON_RANK` to adjust the contest ribbon tier (`1`–`4`).

### NPC dialogues

**File:** `data/scripts/cat_living_dex_reward.inc`

| Label | When shown |
|-------|------------|
| `CatLivingDexReward_Text_InProgress` | Fewer than 6 qualifying species (shows count) |
| `CatLivingDexReward_Text_EligibleIntro` | All 6 species owned |
| `CatLivingDexReward_Text_ObtainedTita` | Fanfare message |
| `CatLivingDexReward_Text_AfterReward` | After receiving Tita |
| `CatLivingDexReward_Text_AlreadyReceived` | Reward already claimed |

### NPC location and sprite

**Map object:** `data/maps/LittlerootTown/map.json`  
- Object: `LOCALID_LITTLEROOT_CAT_COLLECTOR`  
- Position: `(8, 12)` on the path toward Professor Birch's lab  

**Sprite:** `OBJ_EVENT_GFX_GENTLEMAN` in `map.json`.

### Flags

**File:** `include/constants/flags.h`

| Flag | Purpose |
|------|---------|
| `FLAG_RECEIVED_TITA_REWARD` | Set after Tita is received |

**Event script usage** (`data/scripts/cat_living_dex_reward.inc`):

| Flag | Set by | Checked by |
|------|--------|------------|
| `FLAG_RECEIVED_TITA_REWARD` | NPC script: `setflag` after Tita is received | NPC script: `goto_if_set` at start (already claimed branch) |

There is no separate eligibility flag for Tita. Eligibility is evaluated live each time via `CountPlayerCatLivingDexSpecies` (special returns 0–6).

To debug or skip the challenge, set `FLAG_RECEIVED_TITA_REWARD` after giving yourself the six species, or use `setflag` / `clearflag` on that flag directly.

### Engine hooks

| File | Role |
|------|------|
| `src/league_cat_challenge.c` | Living dex check + `GiveTitaLivingDexReward()` |
| `include/league_cat_challenge.h` | Public API |
| `data/specials.inc` | Registers `CountPlayerCatLivingDexSpecies`, `GiveTitaLivingDexReward` |
| `data/event_scripts.s` | Includes `cat_living_dex_reward.inc` |

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
