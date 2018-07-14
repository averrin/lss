# Long Story Short

Yet another unfinished roguelike game.

![screenshot](https://raw.githubusercontent.com/averrin/lss/master/screenshot.png)

## Build
* `git clone --recursive git@github.com:cinder/Cinder.git`
* `cd ./Cinder; mkdir build; cd ./build; cmake ..; make -j4`
* `cd ../..`
* `git clone --recursive git@github.com:averrin/lss.git`
* `cd ./lss; mkdir build; cd ./build`
* `export cinder_DIR=../../Cinder/lib/linux/x86_64/ogl/Debug`
* `cmake ../linux; make -j4`

## TODO
- [X] Game over mode
- [X] Fix attributes and effects
- [X] Fields of view and torches
- [X] Creature traits (for skills and enemy tricks)
- [X] Armor
- [X] Enemy with torches
- [X] Map generator
- [X] Cell & location features
- [X] Light/shadow-related traits
- [ ] Refactor effects system
- [ ] Better event log
- [ ] Weights
- [ ] Passive, patrol and "hero search" AI, stealth
- [ ] Chests and containers
- [ ] Ranged selection and weapons
- [X] Randomized monsters and loot
- [ ] Leveling
- [ ] Hundreds of other features...

## Controls
* / — insert mode
* q — quit [`/quit`, `/q`]
* ? — help [`/help`, `/h`]
* yuhjklbn — move [`/m`, `/move`, `/n`, `/e`, `/s`, `/w`, `/nw`, `/ne`, `/se`, `/sw`]
```
      y k u
       \|/ 
      h-•-l
       /|\ 
      b j n
```
* d[yuhjklbn] — dig [`/dig`, `/d`]
* a[yuhjklbn] — attack [`/attack`, `/a`]
* w[yuhjklbn] — walk until obstacle [`/walk`]
* p or , — pick [`/pick`, `/p`]
* e — equip [`/equip`, `/eq`]
* shift+d — drop [`/drop`, `/dr`]
* i — inventory [`/inventory`, `/i`]
* . — wait [`/wait`]
* z — cast spell [`/zap`, `/z`]
* &lt; — go up [`/up`]
* &gt; — go down [`/down`]

### Debug controls
* F1 — debug mode (show enemy pathes, allow uber spells)
* shift+i — inspect mode

