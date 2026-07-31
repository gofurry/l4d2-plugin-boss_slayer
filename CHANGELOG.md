# Changelog

## 0.6.0

- Personal death now retains the campaign build; mission loss restores the chapter-start snapshot.
- Increased Melee Fury from 6% to 8% attack speed per level.
- Added Fate Reroll, Field Rescue, Ammo Reclamation, First Aid Feedback and sniper-only Precision Hunter.
- Added one Supply Conditioning stack on the first successful supply use of each chapter, up to five stacks.
- Added two-level low-health temporary regeneration below 40 effective health.
- Persisted perk choices when closing and reopening the menu so Fate Reroll cannot be bypassed for free.
- Expanded development commands and the regression checklist for all fourteen perks.

## 0.5.0

- Replaced Boss Hunter, Special Hunter and Last Stand with Melee Fury, Healing Boost, Medical Supply and Throwable Supply.
- Firepower now grants 10% direct damage per level and never increases friendly fire.
- Toughness now reduces combat, fire, acid and friendly-fire damage while excluding non-combat world damage.
- Boss Healing restores 10 real and 10 temporary health per level.
- Added melee attack-speed scaling and medkit, pills and adrenaline healing bonuses.
- Added 180-second empty-slot medical and throwable supply abilities.
- Added a max-build exchange menu for 20+20 health, full reserve ammo, medical items or throwables.
- Added targeted perk and max-build development commands.

## 0.4.2

- Every 500 common-infected kills now grants one perk choice.
- Added `!bsr_testcommonkills [amount]` for development testing.
- Added initial open-source repository documentation and MIT licensing.

## 0.4.1

- Boss killers receive two perk choices.
- Other living human Boss participants receive one perk choice.
- Every 30 normal special-infected kills grants one choice.
- Entering each chapter after the first grants one choice.
- Split the single source file into maintainable modules without changing gameplay behavior.
