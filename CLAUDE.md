# Project: [NAME TBD] — Extraction Roguelike

> Move this file into your UE5 project root once the project folder is created.

## What This Game Is

Top-down extraction roguelike with a **food/cooking theme**. Inspirations: Binding of Isaac (procedural rooms, item synergies), Zelda (room-by-room traversal, boss structure), Noita (slot-based build system). Light extraction: you can leave a run with your loot or die and lose half.

**Engine:** Unreal Engine 5.7  
**Language:** C++ primary, Blueprints for designer iteration  
**Perspective:** Top-down 3D (3D assets, locked overhead camera — not Paper2D)

---

## Core Gameplay Loop

1. At hub (physical kitchen level): drag ingredients into cooking station slots → eat your meal → apply run loadout
2. Enter dungeon through procedural room system
3. Clear rooms, collect ingredient drops from enemies
4. Some cleared rooms respawn enemies over time (escalating pressure — incentivizes extraction)
5. **Extract** (keep everything) OR **die** (50% random item loss, survivors return to base)
6. Return to hub, experiment with new ingredient combinations
7. Repeat until ready to fight boss (available after first run)
8. Boss door shows 2 random modifiers before entry — player can retreat and adjust build
9. Beat boss → unlock more content

---

## Cooking Station System (Noita-style wand builder)

The core build mechanic. At the hub cooking station, the player has named **slots**:

| Slot | What it affects |
|------|----------------|
| Attack1 | Primary attack behavior |
| Attack2 | Secondary attack behavior |
| Dodge | Dodge roll behavior |
| Movement | Base movement stats |
| Potion | What your carried potion does |

- Each slot accepts 1+ ingredients dragged from inventory
- **Same ingredient behaves differently depending on which slot it occupies** (pepper in Attack1 = burn damage; pepper in Dodge = faster roll but shorter distance)
- No fixed recipes — emergent combinations are the game
- The full slot configuration = the "meal"; eating it applies GAS GameplayEffects for the run

### Future Polish: Procedural Dish Display
Track ingredient categories (protein, carb, fat, dairy, spice) → resolve to a dish archetype → display visual with tint variations. Not MVP, but architect so we don't block it later. Each `UIngredientDataAsset` should carry food category tags.

---

## Player Inputs

All four actions are modifiable by the meal loadout:

- **WASD** — Movement
- **Attack1** — Primary attack
- **Attack2** — Secondary attack  
- **Dodge** — Roll / dash
- **Drink Potion** — Use carried consumable

---

## Extraction & Death Rules

- **Extract:** Trigger in dungeon. Player keeps ALL items (ingredients found in run + what they brought in).
- **Die:** 50% of inventory lost permanently (random selection). Remaining 50% returns to base inventory.
- Future: items/upgrades that modify the death loss percentage.
- Items at base are **consumed** when placed into cooking slots — the base inventory is the resource pool.

---

## Potions

- Crafted at hub (have a potion slot in cooking station)
- Limited carry count per run — decided at base during loadout phase
- Found potions are NOT a thing in MVP (keep it simple)

---

## Procedural Dungeon

- Room templates built as **UE5 Level Instances**
- Rooms connect via **Door Actors** (two doors per room for MVP — establishes the procedural connection system)
- Some cleared rooms respawn enemies over time (tunable, creates extraction pressure)
- Boss room at end of dungeon path

---

## Boss System

- Available after first run completion (win or lose)
- **Boss door** shows modifiers before player commits to entering — player can retreat and adjust build
- **MVP:** 2 random modifiers per run
- **Future:** Player-selectable difficulty (3–4 modifiers for extra rewards)
- Modifier categories:
  - Player debuff (reduced stats, restricted ability)
  - Enemy spawns (adds to boss room during fight)
  - Environmental hazards (floor hazards, shrinking arena, etc.)
  - Boss enhancement (new attack pattern, resistance, extra phase)

---

## Tech Stack & Key Decisions

| Decision | Choice | Why |
|----------|--------|-----|
| Ability system | **Gameplay Ability System (GAS)** | Handles all powerup effects, attributes, synergies, boss modifiers at scale |
| Item data | **UDataAsset subclasses** | Type-safe, BP-extensible, no schema drift vs DataTables |
| Room templates | **Level Instances** | Best UE5 tool for composable procedural dungeons |
| Input | **Enhanced Input** | Required for rebindable, context-sensitive controls |
| Assets | **Git LFS** | All binary UE assets tracked via LFS |
| Recipes | **Emergent (no fixed recipes)** | Experimentation is the core fun loop |

---

## Source Folder Structure

```
Source/[ProjectName]/
  Core/         GameInstance, GameMode, GameState, PlayerController, SaveGame
  Abilities/    AbilitySystemComponent, AttributeSets, GameplayAbilities, GameplayEffects
  Characters/   PlayerCharacter, EnemyBase, BossBase
  Cooking/      CookingStationActor, CookingSlot, MealComponent, MealBuilder
  Items/        IngredientDataAsset, LootTableDataAsset, InventoryComponent
  Dungeon/      RoomTemplateActor, DungeonGenerator, DoorActor, RoomDataAsset
  Enemies/      Enemy types, AIControllers
  Boss/         BossBase, BossModifierDataAsset, ModifierPool
  Extraction/   ExtractionTriggerActor, DeathHandlerComponent
  UI/           HUD, CookingUI, BossModifierRevealUI, InventoryWidget
```

---

## MVP Scope (minimum complete loop)

- [ ] Hub level (physical kitchen space, walkable)
- [ ] Cooking station with 5 slots + drag-and-drop UI
- [ ] 5–10 ingredients with meaningful slot-dependent effects
- [ ] Potion system (craft at base, 1 type for MVP)
- [ ] One dungeon: rooms with 2-door procedural connection system
- [ ] 2–3 enemy types with loot tables
- [ ] Escalating pressure (enemy respawn in cleared rooms)
- [ ] Extraction trigger + death handler (50% loss)
- [ ] Boss with 2 random modifiers + door reveal UI
- [ ] GAS wired: at minimum Health, Speed, Damage attributes

---

## Design Decisions Still Open

- Combat feel for Attack1 / Attack2 (melee vs ranged, cooldowns, etc.) — TBD
- Enemy archetypes and thematic loot tables — TBD
- Narrative framing (who is this chef-warrior, why the dungeon) — TBD, not blocking MVP
- Hub layout and art direction — TBD
- Project name — TBD

---

## Build Order (recommended)

1. GAS framework (AttributeSet, AbilitySystemComponent on PlayerCharacter)
2. Enhanced Input + basic top-down movement
3. `UIngredientDataAsset` base class + cooking slot system
4. `UInventoryComponent` + hub cooking station (functional, no art)
5. Procedural room system (Level Instances + DoorActor)
6. Enemy base class + AI + loot table
7. Extraction trigger + death handler
8. Boss base + modifier system + door reveal
9. UI pass (cooking UI, HUD, boss modifier screen)
10. Polish / balancing pass
