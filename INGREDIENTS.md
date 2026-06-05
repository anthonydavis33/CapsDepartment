# Ingredient Reference — CapsDept

> This file covers three things:
> 1. The **dish & sub-slot structure** (what goes where)
> 2. The **food category taxonomy** (all `Food.Category.*` tags and what they mean)
> 3. The **MVP ingredient list** (10 concrete starting ingredients with tags + slot effects)
> 4. **Starter reactions** to wire up early

---

## Dish & Sub-slot Structure

Each cooking slot corresponds to one player action. The sub-slots inside that dish are the individual drag-drop positions.

| Input | Dish | Sub-slots |
|---|---|---|
| Attack1 | **Main Dish** | Protein · Carb · Extra · Topping · Spice ×4 |
| Attack2 | **Appetizer** | Protein · Carb · Topping · Spice ×4 |
| Dodge | **Dessert** | Base · Sweetener · Texture · Garnish |
| Movement | **Side** | Base · Spice ×2 |
| Potion | **Drink** | Liquid · Flavor · Enhancement |

**Key rule:** Reactions are **dish-scoped**. Dairy in Main Dish + Acid in Appetizer = no reaction. Both in Main Dish = Curdled fires. Players can avoid reactions by splitting ingredients across dishes, or trigger them deliberately.

---

## Food Category Tags

These are the `Food.Category.*` gameplay tags. Every `UIngredientDataAsset` carries one or more of these in its `FoodTags` array. The reaction system checks these tags at meal-eat time.

| Tag | What it represents | Real examples |
|---|---|---|
| `Food.Category.Protein` | Meat, eggs, legumes — structural ingredient | Chicken, Beef, Egg, Fish, Tofu |
| `Food.Category.Carb` | Sugars and simple carbohydrates | Sugar, Flour, Bread, Pasta |
| `Food.Category.Starch` | Complex starches — heavier carbs | Potato, Rice, Corn, Cassava |
| `Food.Category.Fat` | Oils, animal fats, rich emulsifiers | Butter, Olive Oil, Lard, Cream |
| `Food.Category.Dairy` | Milk-derived ingredients | Milk, Cheese, Cream, Butter |
| `Food.Category.Acid` | Sourness — breaks down proteins, reacts with dairy | Vinegar, Wine, Tamarind |
| `Food.Category.Citrus` | Bright acid with aromatic oils — subset of acid | Lemon, Lime, Orange, Yuzu |
| `Food.Category.Spice` | Dried aromatic seeds, bark, pods | Pepper, Cumin, Cinnamon, Cardamom |
| `Food.Category.Heat` | Capsaicin heat — chili family | Chili, Jalapeño, Ghost Pepper, Wasabi |
| `Food.Category.Herb` | Fresh green aromatics | Basil, Rosemary, Thyme, Parsley |
| `Food.Category.Allium` | Onion family — sulfurous aromatics | Garlic, Onion, Shallot, Chive |
| `Food.Category.Umami` | Deep savory glutamates | Mushroom, Soy Sauce, Anchovy, Parmesan |
| `Food.Category.Fungi` | Mushrooms and related | Mushroom, Truffle, Shiitake |
| `Food.Category.Sweet` | Sugars and natural sweeteners | Honey, Maple Syrup, Sugar, Agave |
| `Food.Category.Bitter` | Alkaloids — dark, complex, sometimes harsh | Coffee, Dark Chocolate, Arugula, Hops |
| `Food.Category.Fermented` | Aged / cultured — complex flavour depth | Kimchi, Miso, Soy Sauce, Vinegar |
| `Food.Category.Alcohol` | Fermented liquids — volatile, dissolves fats | Wine, Beer, Whiskey, Sake |
| `Food.Category.Fruit` | Fleshy plant fruits | Apple, Tomato, Berry, Mango |
| `Food.Category.Floral` | Delicate aromatic petals/essences | Lavender, Rose, Elderflower |
| `Food.Category.Mineral` | Salt and mineral-forward ingredients | Salt, Seaweed, Mineral Water |

---

## Food Effect Tags (Combat)

These are the `Food.Effect.*` tags applied to the **character's ASC** when a meal GE is active. `UCapsWeaponAbility::ApplyFoodEffectsOnHit` reads these and applies the corresponding GE to the **hit target**.

| Tag | Combat effect on hit target |
|---|---|
| `Food.Effect.Fire` | Burning DoT (damage over time) |
| `Food.Effect.Frost` | Slow / chill |
| `Food.Effect.Acid` | Armor reduction |
| `Food.Effect.Lightning` | Short AoE burst around the target |
| `Food.Effect.Poison` | Stacking DoT |
| `Food.Effect.Heal` | Small heal applied to the **attacker** on hit |

Wire-up: when you create a `GE_Ingredient_X` for a slot, add a **Gameplay Tag** grant of the relevant `Food.Effect.*` tag to the character's ASC. Then create a matching entry in `UCapsGameSettings::FoodHitEffects` mapping that tag to the actual damage/effect GE.

---

## MVP Ingredient List

Ten ingredients that cover a good spread of categories and set up interesting reactions with each other. Start here before expanding.

---

### 1. Pepper
| Field | Value |
|---|---|
| `IngredientID` | `"Pepper"` |
| `FoodTags` | `Food.Category.Spice` |
| **Main Dish effect** | `+5 AttackDamage` — hot, direct, punchy |
| **Appetizer effect** | `+3 AttackDamage` — secondary attack hits harder |
| **Dessert effect** | `+10% Dodge distance` |
| **Side effect** | `+15 MoveSpeed` |
| **Drink effect** | Grants `Food.Effect.Fire` for 1 use |
| **Drops from** | Basic melee enemy |

---

### 2. Chili
| Field | Value |
|---|---|
| `IngredientID` | `"Chili"` |
| `FoodTags` | `Food.Category.Spice`, `Food.Category.Heat` |
| **Main Dish effect** | Grants `Food.Effect.Fire` — attacks apply burning DoT |
| **Appetizer effect** | Grants `Food.Effect.Fire` (weaker duration) |
| **Dessert effect** | Shorter cooldown on dodge |
| **Side effect** | `+20 MoveSpeed`, brief speed burst |
| **Drink effect** | Grants `Food.Effect.Fire` + `Food.Effect.Poison` |
| **Drops from** | Ranged enemy |

---

### 3. Lemon
| Field | Value |
|---|---|
| `IngredientID` | `"Lemon"` |
| `FoodTags` | `Food.Category.Citrus`, `Food.Category.Acid` |
| **Main Dish effect** | Grants `Food.Effect.Acid` — reduces target armor |
| **Appetizer effect** | Grants `Food.Effect.Acid` (shorter duration) |
| **Dessert effect** | Shorter dodge cooldown |
| **Side effect** | `-10 MoveSpeed` (tart, limbs tense up) — balanced by reactions |
| **Drink effect** | Cleanse one debuff on use |
| **Drops from** | Fast/skirmisher enemy |

---

### 4. Garlic
| Field | Value |
|---|---|
| `IngredientID` | `"Garlic"` |
| `FoodTags` | `Food.Category.Allium`, `Food.Category.Herb` |
| **Main Dish effect** | `+8 AttackDamage`, `+5% crit` (amplifier) |
| **Appetizer effect** | `+5 AttackDamage` |
| **Dessert effect** | Bonus dodge iframe duration |
| **Side effect** | `+25 MoveSpeed` |
| **Drink effect** | Grants `Food.Effect.Heal` — lifesteal on hits |
| **Drops from** | Elite/armored enemy |

---

### 5. Mushroom
| Field | Value |
|---|---|
| `IngredientID` | `"Mushroom"` |
| `FoodTags` | `Food.Category.Fungi`, `Food.Category.Umami` |
| **Main Dish effect** | `+15 MaxHealth` |
| **Appetizer effect** | Small regen per room cleared |
| **Dessert effect** | `+20 MaxHealth` |
| **Side effect** | `+10 MaxHealth`, `-5 MoveSpeed` (earthy, heavy) |
| **Drink effect** | Instantly restore 20 Health on use |
| **Drops from** | Slow/tank enemy |

---

### 6. Butter
| Field | Value |
|---|---|
| `IngredientID` | `"Butter"` |
| `FoodTags` | `Food.Category.Fat`, `Food.Category.Dairy` |
| **Main Dish effect** | `+10 AttackDamage` (rich, coating) |
| **Appetizer effect** | `+6 AttackDamage` |
| **Dessert effect** | Silky dodge — reduced friction, longer slide distance |
| **Side effect** | `+30 MoveSpeed` (slippery) |
| **Drink effect** | +1 PotionCharges |
| **Drops from** | Basic melee enemy |

---

### 7. Honey
| Field | Value |
|---|---|
| `IngredientID` | `"Honey"` |
| `FoodTags` | `Food.Category.Sweet` |
| **Main Dish effect** | Grants `Food.Effect.Heal` — heal on hit |
| **Appetizer effect** | Grants `Food.Effect.Heal` (weaker) |
| **Dessert effect** | Regen HP on dodge |
| **Side effect** | `-5 MoveSpeed` (sticky feet) |
| **Drink effect** | Restore 30 Health on use |
| **Drops from** | Ranged enemy |

---

### 8. Egg
| Field | Value |
|---|---|
| `IngredientID` | `"Egg"` |
| `FoodTags` | `Food.Category.Protein`, `Food.Category.Dairy` |
| **Main Dish effect** | `+10 MaxHealth`, `+3 AttackDamage` (well-rounded) |
| **Appetizer effect** | `+8 MaxHealth` |
| **Dessert effect** | Absorb 1 hit per room (eggshell barrier) |
| **Side effect** | `+10 MaxHealth` |
| **Drink effect** | Restore 15 Health, grant a small shield |
| **Drops from** | Any enemy |

---

### 9. Salt
| Field | Value |
|---|---|
| `IngredientID` | `"Salt"` |
| `FoodTags` | `Food.Category.Mineral` |
| **Main Dish effect** | Amplifies the dish — `+15%` to all other Main Dish GE magnitudes |
| **Appetizer effect** | Same amplifier for Appetizer |
| **Dessert effect** | Amplifies Dessert effects |
| **Side effect** | Amplifies Side effects |
| **Drink effect** | Amplifies Drink effects, +1 PotionCharges |
| **Drops from** | All enemies (common drop) |
| **Note** | Salt's amplifier effect requires custom BP logic reading SlottedIngredients and scaling GE magnitudes. It's a power multiplier, not a raw stat — save it for after basic effects work. |

---

### 10. Vinegar
| Field | Value |
|---|---|
| `IngredientID` | `"Vinegar"` |
| `FoodTags` | `Food.Category.Acid`, `Food.Category.Fermented` |
| **Main Dish effect** | Grants `Food.Effect.Acid` + `Food.Effect.Poison` |
| **Appetizer effect** | Grants `Food.Effect.Poison` |
| **Dessert effect** | `-5% Dodge cooldown`, sharp recovery |
| **Side effect** | `+10 MoveSpeed`, small incoming damage reduction |
| **Drink effect** | Remove all debuffs on use |
| **Drops from** | Fast/skirmisher enemy |

---

## Starter Reactions

Wire these first — they all fire with the MVP ingredients above.

| Reaction | Required Tags (same dish) | Blocking Tags | Effect idea |
|---|---|---|---|
| **Curdled** | `Dairy` + `Acid` or `Citrus` | `Heat` | Negative: dish effects reduced 30% |
| **Caramelized** | `Sweet` + `Heat` | — | Positive: `Food.Effect.Fire` + small heal on hit |
| **Emulsified** | `Fat` + `Acid` | — | Positive: all dish GE durations +25% |
| **Aromatic Burst** | `Allium` + `Herb` | — | Positive: `+10 AttackDamage`, `+15 MoveSpeed` |
| **Maillard** | `Protein` + `Heat` | `Acid` | Positive: `+20 AttackDamage`, looks/feels cooked right |
| **Pickle** | `Acid` + `Fermented` | `Dairy` | Mixed: Grants `Food.Effect.Poison` to attacker's hits, but `-10 MaxHealth` |
| **Smoke & Char** | `Heat` + `Protein` + `Fat` | — | Positive: `Food.Effect.Fire` + `Food.Effect.Poison` |
| **Oxidized** | `Alcohol` + `Acid` | `Fat` | Negative: halves AttackDamage for that dish |
| **Fortified** | `Alcohol` + `Fat` | — | Positive: `+25 MaxHealth`, `Food.Effect.Heal` |
| **Bitter Finish** | `Bitter` + `Sweet` | — | Mixed: big `AttackDamage` boost, `-15 MoveSpeed` |

---

## Notes for Adding More Ingredients

When you're ready to expand past the MVP 10:

- **Each ingredient should behave differently in at least 2 slots** — otherwise it's just a flat stat and isn't interesting
- **Rare drops** (boss loot, late rooms) should have stronger effects OR unlock otherwise-inaccessible reactions
- **Prepared ingredients** (`bIsPreparedIngredient = true`) can't drop from enemies — they're crafted at the hub. Good for endgame power items like Marinated Steak, Reduction Sauce, etc.
- **Multiple tags** on one ingredient = more reaction surface area = more interesting build choices. Butter is both Fat AND Dairy, so it triggers both Emulsified (with Acid) and Curdled (with Acid). That tension is the fun.

---

*Sync this file if slot structure or reaction rules change.*
