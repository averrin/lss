# Long Story Short

![screenshot](https://raw.githubusercontent.com/averrin/lss/master/screenshot.png)

## Build
* git clone --recursive git@github.com:cinder/Cinder.git
* cd ./Cinder; mkdir build; cd ./build; cmake ..; make -j4
* cd ../..
* git clone --recursive git@github.com:averrin/lss.git
* cd ./lss; mkdir build; cd ./build
* export cinder_DIR=../../Cinder/lib/linux/x86_64/ogl/Debug
* cmake ../linux; make -j4

## Controls
* q — quit [/quit, /q]
* hjkl — move [/m, /move, /n, /e, /s, /w, /nw, /ne, /se, /sw]
* / — insert mode
* d[hjkl] — dig [/dig, /d]
* a[hjkl] — attack [/attack, /a]
* w[hjkl] — walk until obstacle [/walk]
* p — pick [/pick, /p]
* e — equip [/equip, /eq]

