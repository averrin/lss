# Long Story Short

Yet another unfinished roguelike game.

![screenshot](https://raw.githubusercontent.com/averrin/lss/master/screenshot.png)

## Build
* `git clone --recursive git@github.com:averrin/lss.git`
* `cd ./lss; mkdir build; cd ./build`
* `conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan`
* `conan install .. --build=missing`
* `cmake ..; make -j4`

Yeah, LSS migrate from cinder to SDL2. Now its more lightweight, but lost some
rendering quality. And you may need install some deps.

## TODO
- [ ] Achieve same text quality as in `cinder` branch
- [X] Game over mode
- [X] Fix attributes and effects
- [X] Fields of view and torches
- [X] Creature traits (for skills and enemy tricks)
- [X] Armor
- [X] Enemy with torches
- [X] Map generator
- [X] Cell & location features
- [X] Light/shadow-related traits
- [X] Better event log
- [X] After death stats
- [X] Debug menus for summoning enemies/items & location creator
- [X] Ranged selection
- [X] Randomized monsters and loot
- [ ] Refactor AI for optimization, using magic/throwing and different behaviors
      like stealth
- [X] Hero status page
- [ ] Refactor effects system
- [ ] Weights
- [ ] Passive, patrol and "hero search" AI, stealth
- [ ] Chests and containers
- [ ] Throwing
- [ ] Passthrough seed
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
* @ — status page [`/hero`]
* shift+d — drop [`/drop`, `/dr`]
* shift+u — use [`/use`, `/u`]
* shift+l — [un]equip light [`/light`, `/l`]
* i — inventory [`/inventory`, `/i`]
* . — wait [`/wait`]
* z — cast spell [`/zap`, `/z`]
* t — throw item [`/throw`, `/t`]
* &lt; — go up [`/up`]
* &gt; — go down [`/down`]

### Debug controls
* F1 — debug mode (show enemy pathes, allow uber spells)
* shift+i — inspect mode
* s — spawn item
* shift+s — spawn enemy

## More screenshots
![screenshot_cavern](https://raw.githubusercontent.com/averrin/lss/master/screenshot_cavern.png)
![screenshot_torches](https://raw.githubusercontent.com/averrin/lss/master/screenshot_torches.png)

