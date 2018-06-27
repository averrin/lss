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
- [ ] Game over mode
- [ ] Fix attributes and effects
- [ ] Better event log
- [ ] Passive, patrol and "hero search" AI
- [ ] Map generator
- [ ] Creature traits (for skills and enemy tricks)
- [ ] Fields of view, torches, grue and other light-related things
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

