# Blueprint Work Order — CapsDept

> Philosophy: **get something testable as fast as possible, then layer on top.**
> Each milestone ends with something you can actually run and poke at.
> The `showdebug abilitysystem` console command will be your best friend throughout.

---

## Milestone 1 — Get a Character Moving

*Goal: open a test level and walk around*

- [ ] **Project Settings housekeeping**
	- Set `UCapsGameInstance` as the Game Instance class (`Project Settings → Maps & Modes`)
	- Set `ACapsPlayerController` (or your BP child) as the default controller
	- Create an `Input Mapping Context` + 5 `Input Action` assets: **Move, Attack1, Attack2, Dodge, DrinkPotion**
	- Point the controller's properties at those input action assets

- [ ] **`BP_CapsCharacter`** — child of `ACapsCharacter`
	- Assign a skeletal mesh, animation Blueprint, capsule size
	- This is your placeholder until proper character select is wired

- [ ] **`BP_CapsPlayerController`** — child of `ACapsPlayerController`
	- Assign the mapping context + all 5 input actions in the Details panel

- [ ] **Test level** — drop `BP_CapsCharacter` + `BP_CapsPlayerController` in, hit Play, walk around ✅

---

## Milestone 2 — First GAS Ability + Death

*Goal: attack something, take damage, die*

- [ ] **`GE_BaseStats`** — `UGameplayEffect`, Infinite duration
	- Modifiers: Health=100, MaxHealth=100, AttackDamage=10, MoveSpeed=600
	- Apply this to the player in `BeginPlay` via `ApplyGameplayEffectToSelf` in Blueprint (temporary until character data assets are set up)

- [ ] **`BP_GA_BasicAttack`** — child of `UCapsWeaponAbility`
	- Override `BP_ActivateAbility`: sphere trace forward, call `ApplyFoodEffectsOnHit` on anything hit, call `FinishAbility`
	- For damage: apply a `GE_Damage_Instant` (Instant duration, modifies Health by -10) to the hit target

- [ ] Add `BP_GA_BasicAttack` to `DefaultAbilities` on `BP_CapsCharacter`, set its `InputID` to `Attack1`

- [ ] **`BP_CapsCharacter::BP_OnDeath`** — print "dead" to screen for now, confirm the health → death pipeline works ✅

---

## Milestone 3 — Ingredients + Cooking

*Goal: drag ingredients into slots, eat a meal, see a GAS effect apply*

- [ ] **Create 5 `UIngredientDataAsset` instances** (e.g. `DA_Ingredient_Pepper`)
	- Set `IngredientID` FName (e.g. `"Pepper"`)
	- Set `FoodTags` (e.g. `Food.Category.Spice`, `Food.Category.Heat`)
	- For each dish slot you care about, set a `GameplayEffect` in `SlotEffects`

- [ ] **Create corresponding `GE_Ingredient_X` GameplayEffects** — Infinite duration
	- Example: `GE_Pepper_Attack` boosts `AttackDamage` by +5
	- One GE per ingredient per slot (you won't need all of them right away)

- [ ] **`BP_CookingStationWidget`** — child of `UCapsCookingStationWidget`
	- Override `OnInventoryRefreshed(BaseStock)`: build ingredient list buttons from the map
	- Override `BP_OnMealEaten`: print "meal eaten!" to screen
	- Override `BP_OnEatFailed`: print "nothing slotted" to screen

- [ ] **`BP_SubSlotWidget`** — child of `UCapsSubSlotWidget`
	- Override `OnIngredientSet(IngredientID)`: show the ingredient name/icon
	- Override `OnSlotCleared`: clear the display

- [ ] **`BP_CapsIngredientCardWidget`** — child of `UCapsIngredientCardWidget`
	- Override `OnIngredientSet(IngredientID)`: show icon/name on the card
	- This is the draggable card; assign it as the drag visual too

- [ ] Manually call `AddToBaseStock("Pepper", 3)` in your character `BeginPlay` to seed test stock

- [ ] Show the widget, drag into slots, click Eat, confirm `AttackDamage` changed in `showdebug abilitysystem` ✅

---

## Milestone 4 — Hub Level + Dungeon Transition

*Goal: walk through a door, load into the dungeon level*

- [ ] **Create a Hub level** — flat plane is fine for now

- [ ] **`BP_CapsGameMode`** — child of `ACapsGameMode`
	- Override `BP_OnHubReady`: print "hub ready" or show character select
	- Override `BP_OnDungeonReady`: print "dungeon ready"
	- Override `BP_OnTransitionToDungeon`: screen fade or just pass through
	- Override `BP_OnTransitionToHub`: screen fade or just pass through
	- Set this as the Game Mode in **World Settings for both levels**

- [ ] In **Project Settings → Caps Dept** (the `UCapsGameSettings` DeveloperSettings panel):
	- Set `HubLevelName` = name of your hub level
	- Set `DungeonLevelName` = name of your dungeon level

- [ ] **`BP_CapsHubDoorActor`** — child of `ACapsHubDoorActor`
	- Override `BP_OnPlayerEntered`: for now immediately call `CommitTransition()`
	- Override `BP_OnSealed`: hide/swap the door mesh to a sealed state

- [ ] Place `BP_CapsHubDoorActor` in the hub, walk through it → dungeon level loads ✅

---

## Milestone 5 — Dungeon Rooms Streaming

*Goal: rooms load, you can walk East through doors, rooms stream in/out*

- [ ] **`BP_CapsDoorActor`** — child of `ACapsDoorActor`
	- Override `BP_OnStateChanged(NewState)`: swap mesh or material for Open/Sealed/Locked states

- [ ] **Create 3+ room Level Instances** — each contains:
	- One `ACapsRoomTemplate` actor
		- Set `RoomType` (Combat, Safe, etc.)
		- Drag your two door actors into the `Doors` array
	- Two `BP_CapsDoorActor` instances placed at East/West exits
	- Some floor/wall geometry
	- A few `EnemySpawnPoints` transforms on the room template

- [ ] **Create a Dungeon level**
	- Place `ACapsDungeonGenerator`
	- Assign your room Level Instances to `RoomPool`
	- Tweak `RoomSpacing` (default 5000 units) until rooms line up

- [ ] Walk East through doors, verify new rooms load. Walk West, verify old ones unload ✅

- [ ] **Wire room-clear (temporary):** add a test keybind or timer in the Level Instance that calls `ACapsRoomTemplate::NotifyRoomCleared()` so you can verify the East door opens

---

## Milestone 6 — Enemies + Loot Drops

*Goal: enemy spawns, chases you, dies, drops an ingredient you can pick up*

- [ ] **`DA_LootTable_BasicEnemy`** — `UCapsLootTableDataAsset`
	- Add entries: `IngredientID = "Pepper"`, Weight=1, MinCount=1, MaxCount=1
	- Set `MinDrops=1`, `MaxDrops=2`

- [ ] **`BP_CapsEnemyBase`** — child of `ACapsEnemyBase`
	- Assign mesh, animation Blueprint
	- Assign `LootTable` = `DA_LootTable_BasicEnemy`
	- Set `AIControllerClass` = `BP_CapsEnemyAIController`
	- Override `BP_OnDeath`: play death animation, call `Destroy()` after a short delay
	- Override `BP_OnDropLoot(IngredientIDs, DropLocation)`:
		- For each ID in `IngredientIDs`: spawn `BP_IngredientPickup` at `DropLocation + small random offset`
		- Set `IngredientID` on the spawned actor

- [ ] **`BP_CapsEnemyAIController`** — child of `ACapsEnemyAIController`
	- Override `TriggerAttack`: apply a `GE_EnemyDamage` Instant GE to the player directly for now
	- Tune `AggroRange` and `AttackRange` in the Details panel

- [ ] **`BP_IngredientPickup`** — child of `ACapsIngredientPickupActor`
	- Give it a static mesh or billboard component
	- Override `BP_OnPickedUp(Picker)`: play collect sound/particle (actor destroys itself after)

- [ ] Place enemy in a room, kill it, watch loot drop, walk over it, check `RunPickups` in the inventory ✅

---

## Milestone 7 — Character Select

*Goal: pick a character at the hub, enter dungeon with their stats and abilities applied*

- [ ] **`GE_CharacterStats_Chef`** — Infinite duration (one per character)
	- Modifiers: override base stats with that character's values (Health, AttackDamage, MoveSpeed, etc.)

- [ ] **`BP_GA_ChefMainWeapon`** — child of `UCapsWeaponAbility` (one per character)
	- This is their unique primary attack — implement it however fits the character

- [ ] **`DA_Character_Chef`** — `UCapsCharacterDataAsset`
	- Set `CharacterName`, `Description`, `Portrait`, `Pros`, `Cons`
	- Set `Mesh`, `AnimClass`
	- Set `CharacterStatEffect` = `GE_CharacterStats_Chef`
	- Set `MainWeaponAbility` = `BP_GA_ChefMainWeapon`
	- Set `DefaultSecondaryAbility` (can reuse `BP_GA_BasicAttack` for now)

- [ ] Add all `DA_Character_*` assets to `UCapsGameSettings::AvailableCharacters`

- [ ] **`BP_CapsCharacterSelectWidget`** — child of `UCapsCharacterSelectWidget`
	- Override `OnCharactersLoaded(Characters)`: create a card widget per character
	- Override `OnCharacterPreviewed(Data)`: populate name/portrait/pros-cons panel
	- Call `PreviewCharacter()` on card hover, `ConfirmSelection()` on "Start Run" button
	- Bind `OnSelectionConfirmed` → `GameMode->BeginDungeonTransition()`

- [ ] In `BP_CapsGameMode::BP_OnHubReady`: create and show `BP_CapsCharacterSelectWidget`

- [ ] Select a character, enter dungeon, verify stats and abilities from the data asset are applied ✅

---

## Milestone 8 — Boss Door + Modifier Reveal

*Goal: walk up to the boss door, see modifiers, commit or retreat, fight*

- [ ] **Create 4-6 `UCapsBossModifierDataAsset` instances** (e.g. `DA_Modifier_DoubleDamage`)
	- Set `ModifierName`, `Description`
	- Set `PlayerEffect` — a `GameplayEffect` that debuffs/modifies the player for the fight
	- Set `BossEffect` if you want to buff the boss too

- [ ] **`DA_BossModifierPool`** — `UCapsBossModifierPoolDataAsset`
	- Add all your modifier assets to the pool
	- Set `ModifiersToShow = 2`

- [ ] **`BP_CapsBossDoorActor`** — child of `ACapsBossDoorActor`
	- Assign `ModifierPool`
	- Set `RevealWidgetClass` = `BP_CapsModifierRevealWidget`
	- Override `BP_OnRevealWidgetShown`: push camera in, darken screen, whatever feels right
	- Override `BP_OnPlayerEntered`: start the boss fight (spawn boss, open arena, etc.)

- [ ] **`BP_CapsModifierRevealWidget`** — child of `UCapsModifierRevealWidget`
	- Override `OnModifiersReady(Modifiers, Door)`: build modifier card layout from the array
	- Wire "Enter Anyway" button → `CommitToEntry()`
	- Wire "Retreat" button → `Retreat()`

- [ ] **`BP_CapsBossBase`** — child of `ACapsBossBase`
	- Assign mesh, animation BP, `Phase2HealthThreshold` (e.g. 0.4 = phase 2 at 40% health)
	- Set `BossModifierPool`
	- Override `BP_OnPhaseChanged(Phase)`: trigger phase 2 attack pattern / animation changes
	- Override `BP_OnDeath`: play death sequence, trigger win condition

- [ ] Walk to boss door → modifier reveal appears → commit → fight starts ✅

---

## Milestone 9 — Extraction + Death + Save

*Goal: complete a run (extract or die), return to hub, inventory persists across the reload*

- [ ] **`BP_CapsExtractionTriggerActor`** — child of `ACapsExtractionTriggerActor` (or just place the C++ one)
	- Override `OnExtractionActivated`: show "You extracted!" screen, wait a beat
	- Bind `OnPlayerExtracted` → `GameMode->ReturnToHub()`

- [ ] **`BP_CapsCharacter::BP_OnDeath`** (update from the stub in Milestone 2)
	- Show a death screen / play death animation
	- After a delay, call `GameMode->ReturnToHub()`

- [ ] **Test the full extraction loop:**
	- Run → pick up loot → step on extraction trigger → hub loads → ingredients are in stock ✅

- [ ] **Test the death loop:**
	- Run → get killed → hub loads → ~50% of items are gone ✅

- [ ] **Test persistence:**
	- Close the editor entirely → reopen → Play → hub loads → ingredients still there ✅

---

## Milestone 10 — HUD + Minimap

*Goal: health bar, room counter, enemy dots, and minimap all visible during a run*

- [ ] **`BP_CapsHUDWidget`** — child of `UCapsHUDWidget`
	- Override `OnHealthChanged(CurrentHealth, MaxHealth)`: update health bar fill
	- Override `OnCurrentRoomChanged(RoomIndex)`: update room counter text
	- Override `OnWaveIncoming`: flash a "Reinforcements incoming!" warning widget

- [ ] **`BP_CapsMinimapWidget`** — child of `UCapsMinimapWidget`
	- Override `OnRoomDataUpdated(Entries)`: for each entry, draw a room cell on a canvas
	- Use these flags to pick your icon style:
		- `bIsCurrent` → highlight
		- `bVisited` → solid icon
		- `bRevealedByProximity` → "?" icon (safe room nearby)
		- `RoomType == Hub` → draw chains icon at the far left
		- `EnemyPositions` → draw small dots inside the room cell
	- Tune `RoomCellSize` and `RoomCellGap` on the widget

- [ ] Add both to your HUD and show them when the dungeon level starts via `BP_OnDungeonReady` ✅

---

## Quick Reference — All Blueprint Subclasses

| Blueprint to Create | Extends (C++ class) | Key thing to implement |
|---|---|---|
| `BP_CapsCharacter` | `ACapsCharacter` | Mesh, anim BP, `BP_OnDeath` |
| `BP_CapsPlayerController` | `ACapsPlayerController` | Input assets in Details |
| `BP_CapsGameMode` | `ACapsGameMode` | `BP_OnHubReady`, `BP_OnDungeonReady` |
| `BP_CapsHubDoorActor` | `ACapsHubDoorActor` | `BP_OnPlayerEntered`, `BP_OnSealed` |
| `BP_CapsDoorActor` | `ACapsDoorActor` | `BP_OnStateChanged` |
| `BP_CapsEnemyBase` | `ACapsEnemyBase` | `BP_OnDeath`, `BP_OnDropLoot` |
| `BP_CapsEnemyAIController` | `ACapsEnemyAIController` | `TriggerAttack` |
| `BP_CapsBossBase` | `ACapsBossBase` | `BP_OnPhaseChanged`, `BP_OnDeath` |
| `BP_CapsBossDoorActor` | `ACapsBossDoorActor` | `BP_OnRevealWidgetShown`, `BP_OnPlayerEntered` |
| `BP_IngredientPickup` | `ACapsIngredientPickupActor` | Mesh, `BP_OnPickedUp` |
| `BP_CapsHUDWidget` | `UCapsHUDWidget` | `OnHealthChanged`, `OnCurrentRoomChanged`, `OnWaveIncoming` |
| `BP_CapsMinimapWidget` | `UCapsMinimapWidget` | `OnRoomDataUpdated` |
| `BP_CookingStationWidget` | `UCapsCookingStationWidget` | `OnInventoryRefreshed`, `BP_OnMealEaten` |
| `BP_SubSlotWidget` | `UCapsSubSlotWidget` | `OnIngredientSet`, `OnSlotCleared` |
| `BP_CapsIngredientCardWidget` | `UCapsIngredientCardWidget` | `OnIngredientSet` |
| `BP_CapsModifierRevealWidget` | `UCapsModifierRevealWidget` | `OnModifiersReady` |
| `BP_CapsCharacterSelectWidget` | `UCapsCharacterSelectWidget` | `OnCharactersLoaded`, `OnCharacterPreviewed` |

---

## Quick Reference — Data Assets to Create

These are plain `UDataAsset` subclasses — no Blueprint subclassing, just fill in the fields.

| Asset | Class | Notes |
|---|---|---|
| `DA_Ingredient_X` | `UIngredientDataAsset` | One per ingredient; set FoodTags + SlotEffects |
| `DA_LootTable_X` | `UCapsLootTableDataAsset` | One per enemy type; set weighted entries |
| `DA_Character_X` | `UCapsCharacterDataAsset` | One per playable character |
| `DA_BossModifier_X` | `UCapsBossModifierDataAsset` | One per modifier; 4-6 to start |
| `DA_BossModifierPool` | `UCapsBossModifierPoolDataAsset` | One pool; holds all modifiers |
| `DA_DungeonScaling` | `UDungeonScalingDataAsset` | Difficulty tiers per room count |
| `DA_Tileset_X` | `UCapsTilesetDataAsset` | One per visual theme |

---

## Quick Reference — GameplayEffects to Create

| GE | Duration | Purpose |
|---|---|---|
| `GE_BaseStats` | Infinite | Starting health, damage, speed for testing |
| `GE_CharacterStats_X` | Infinite | Per-character stat overrides (one per character) |
| `GE_Ingredient_X_Slot` | Infinite | Effect of ingredient X in a given dish slot |
| `GE_Damage_Instant` | Instant | Generic -N health hit |
| `GE_EnemyDamage` | Instant | Enemy melee hit on player |
| `GE_BossModifier_X` | Infinite | Applied to player when they commit to boss entry |
| `GE_PotionEffect` | Duration | Applied when DrinkPotion ability fires |
| `GE_SetPotionCharges` | Infinite | Sets `PotionCharges` attribute at run start |

---

## Useful Console Commands

```
showdebug abilitysystem       — GAS attribute/ability/tag debugger
showdebug AI                  — AI state and perception
ce [EventName]                — Fire a Gameplay Cue for testing
stat game                     — General performance overlay
```

---

*Generated with Claude — sync this file if the C++ layer changes significantly.*
