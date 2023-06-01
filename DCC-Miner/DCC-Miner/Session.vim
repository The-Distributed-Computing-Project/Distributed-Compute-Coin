let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd /mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +1 Main.cpp
badd +9 term://.//1585:/bin/bash
badd +1 Main.h
badd +424 P2PClient.cpp
badd +1 Console.cpp
badd +138 /usr/share/nvim/runtime/doc/various.txt
badd +1 term:///mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner//1187:/bin/bash
badd +1 FileManip.cpp
badd +17 FileManip.h
badd +1 strops.cpp
badd +1 /mnt/d/Code/DC-Cryptocurrency/DCC-Miner/run.sh
badd +30 P2PClient.h
argglobal
%argdel
$argadd Main.cpp
set stal=2
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabrewind
edit Main.cpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
split
1wincmd k
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 43 + 32) / 64)
exe '2resize ' . ((&lines * 17 + 32) / 64)
argglobal
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=7
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 541 - ((21 * winheight(0) + 21) / 43)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 541
normal! 011|
wincmd w
argglobal
if bufexists(fnamemodify("term:///mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner//1187:/bin/bash", ":p")) | buffer term:///mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner//1187:/bin/bash | else | edit term:///mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner//1187:/bin/bash | endif
if &buftype ==# 'terminal'
  silent file term:///mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner//1187:/bin/bash
endif
balt Main.cpp
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 17 - ((16 * winheight(0) + 8) / 17)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 17
normal! 0
wincmd w
exe '1resize ' . ((&lines * 43 + 32) / 64)
exe '2resize ' . ((&lines * 17 + 32) / 64)
tabnext
edit P2PClient.cpp
argglobal
balt Main.h
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=10
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 424 - ((30 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 424
normal! 010|
tabnext
edit Console.cpp
argglobal
balt P2PClient.cpp
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=2
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 1 - ((0 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
tabnext 1
set stal=1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
set hlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
