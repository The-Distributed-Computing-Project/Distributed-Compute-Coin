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
badd +449 P2PClient.cpp
badd +1 Console.cpp
badd +138 /usr/share/nvim/runtime/doc/various.txt
badd +1 term:///mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner//1187:/bin/bash
badd +1 FileManip.cpp
badd +17 FileManip.h
badd +1 strops.cpp
badd +1 /mnt/d/Code/DC-Cryptocurrency/DCC-Miner/run.sh
badd +27 P2PClient.h
badd +151 ~/.config/nvim/init.vim
badd +28 Network.cpp
badd +11 Network.h
badd +4 SettingsConsts.h
badd +64 /mnt/d/Code/DC-Cryptocurrency/README.md
argglobal
%argdel
$argadd Main.cpp
set stal=2
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
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
exe '1resize ' . ((&lines * 50 + 32) / 64)
exe '2resize ' . ((&lines * 10 + 32) / 64)
argglobal
balt SettingsConsts.h
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
119
normal! zo
304
normal! zo
346
normal! zo
350
normal! zo
366
normal! zo
372
normal! zo
374
normal! zo
429
normal! zo
467
normal! zo
491
normal! zc
544
normal! zo
546
normal! zc
574
normal! zc
593
normal! zo
612
normal! zo
626
normal! zo
723
normal! zo
725
normal! zo
745
normal! zo
747
normal! zo
770
normal! zo
772
normal! zo
795
normal! zo
815
normal! zo
817
normal! zo
823
normal! zo
833
normal! zo
855
normal! zo
857
normal! zo
874
normal! zo
915
normal! zo
929
normal! zo
857
normal! zc
855
normal! zc
795
normal! zc
1033
normal! zo
1043
normal! zo
1124
normal! zo
1135
normal! zo
1180
normal! zo
1196
normal! zo
1270
normal! zo
1290
normal! zc
1350
normal! zo
1352
normal! zc
1270
normal! zc
1377
normal! zo
1406
normal! zo
1408
normal! zc
1377
normal! zc
let s:l = 236 - ((10 * winheight(0) + 25) / 50)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 236
normal! 0
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
let s:l = 2760 - ((9 * winheight(0) + 5) / 10)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 2760
normal! 072|
wincmd w
2wincmd w
exe '1resize ' . ((&lines * 50 + 32) / 64)
exe '2resize ' . ((&lines * 10 + 32) / 64)
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
39
normal! zo
39
normal! zc
61
normal! zo
61
normal! zc
107
normal! zo
110
normal! zo
128
normal! zo
141
normal! zo
146
normal! zo
273
normal! zo
414
normal! zo
427
normal! zo
446
normal! zo
458
normal! zo
464
normal! zo
472
normal! zo
480
normal! zo
497
normal! zo
505
normal! zo
520
normal! zo
535
normal! zo
548
normal! zo
558
normal! zo
569
normal! zo
580
normal! zo
464
normal! zo
472
normal! zo
480
normal! zo
497
normal! zo
505
normal! zo
520
normal! zo
535
normal! zo
548
normal! zo
558
normal! zo
569
normal! zo
580
normal! zo
let s:l = 677 - ((50 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 677
normal! 0
tabnext
edit P2PClient.h
argglobal
balt P2PClient.cpp
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
19
normal! zo
let s:l = 26 - ((25 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 26
normal! 031|
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
let s:l = 56 - ((50 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 56
normal! 0
tabnext
edit SettingsConsts.h
argglobal
balt Console.cpp
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 7 - ((6 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 7
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
