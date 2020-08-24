let SessionLoad = 1
let s:so_save = &so | let s:siso_save = &siso | set so=0 siso=0
let v:this_session=expand("<sfile>:p")
silent only
cd ~/Dev/evol
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +33 CMakeLists.txt
badd +61 Application/World/World.h
badd +16 Application/World/World.c
badd +1 Application/Game/Game.h
badd +1 Application/Game/Game.c
badd +26 Application/World/flecs/examples/c/01_helloworld/src/main.c
badd +1 Application/World/flecs/examples/c/01_helloworld/include/helloworld/bake_config.h
badd +1 Application/World/flecs/examples/c/01_helloworld/include/helloworld.h
badd +38 Application/World/flecs/examples/c/02_simple_system/src/main.c
badd +30 Application/World/flecs/examples/c/04_simple_module/src/simple_module.c
badd +14 Application/World/flecs/examples/c/04_simple_module/src/main.c
badd +36 Application/World/modules/gameobject.h
badd +1 Application/World/modules/gameobject.c
badd +43 Application/World/modules/transform.h
badd +11 Application/World/modules/transform.c
badd +44 Application/World/modules/transform_module.h
badd +11 Application/World/modules/transform_module.c
badd +614 Application/World/flecs/include/flecs.h
badd +1 term://.//20686:/usr/bin/fish
badd +1 .gitmodules
badd +1 .git/modules/Application/World/flecs/ORIG_HEAD
badd +1 .git/modules/Application/World/flecs/HEAD
badd +1 .git/modules/Application/World/flecs/FETCH_HEAD
badd +1 .git/modules/Application/World/flecs/index
badd +1 .git/modules/Application/World/flecs/packed-refs
badd +1 .git/HEAD
badd +7 Application/World/modules/rendering_module.c
badd +16 Application/World/modules/rendering_module.h
argglobal
%argdel
edit Application/Game/Game.c
set splitbelow splitright
wincmd _ | wincmd |
split
1wincmd k
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
wincmd w
set nosplitbelow
set nosplitright
wincmd t
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 43 + 24) / 48)
exe 'vert 1resize ' . ((&columns * 96 + 96) / 192)
exe '2resize ' . ((&lines * 43 + 24) / 48)
exe 'vert 2resize ' . ((&columns * 95 + 96) / 192)
exe '3resize ' . ((&lines * 1 + 24) / 48)
argglobal
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 15 - ((14 * winheight(0) + 21) / 43)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
15
normal! 011|
lcd ~/Dev/evol
wincmd w
argglobal
if bufexists("~/Dev/evol/.git/modules/Application/World/flecs/HEAD") | buffer ~/Dev/evol/.git/modules/Application/World/flecs/HEAD | else | edit ~/Dev/evol/.git/modules/Application/World/flecs/HEAD | endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 1 - ((0 * winheight(0) + 21) / 43)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
1
normal! 0
lcd ~/Dev/evol
wincmd w
argglobal
if bufexists("term://.//20686:/usr/bin/fish") | buffer term://.//20686:/usr/bin/fish | else | edit term://.//20686:/usr/bin/fish | endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 3 - ((0 * winheight(0) + 0) / 1)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
3
normal! 0
lcd ~/Dev/evol
wincmd w
2wincmd w
exe '1resize ' . ((&lines * 43 + 24) / 48)
exe 'vert 1resize ' . ((&columns * 96 + 96) / 192)
exe '2resize ' . ((&lines * 43 + 24) / 48)
exe 'vert 2resize ' . ((&columns * 95 + 96) / 192)
exe '3resize ' . ((&lines * 1 + 24) / 48)
tabnext 1
if exists('s:wipebuf') && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 winminheight=1 winminwidth=1 shortmess=filnxtToOFc
let s:sx = expand("<sfile>:p:r")."x.vim"
if file_readable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &so = s:so_save | let &siso = s:siso_save
let g:this_session = v:this_session
let g:this_obsession = v:this_session
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
