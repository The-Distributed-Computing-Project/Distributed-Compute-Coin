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
badd +275 Main.cpp
badd +9 term://.//1585:/bin/bash
badd +1 Main.h
badd +436 P2PClient.cpp
badd +1 Console.cpp
badd +138 /usr/share/nvim/runtime/doc/various.txt
badd +1 term:///mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner//1187:/bin/bash
badd +1 FileManip.cpp
badd +17 FileManip.h
badd +1 strops.cpp
badd +1 /mnt/d/Code/DC-Cryptocurrency/DCC-Miner/run.sh
badd +36 P2PClient.h
badd +151 ~/.config/nvim/init.vim
badd +28 Network.cpp
badd +11 Network.h
badd +4 SettingsConsts.h
argglobal
%argdel
$argadd Main.cpp
set stal=2
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
117
normal! zo
273
normal! zo
275
normal! zo
298
normal! zo
303
normal! zo
307
normal! zo
307
normal! zo
315
normal! zo
317
normal! zo
329
normal! zo
327
normal! zo
331
normal! zo
348
normal! zo
354
normal! zo
356
normal! zo
365
normal! zo
367
normal! zo
379
normal! zo
427
normal! zo
434
normal! zo
478
normal! zo
493
normal! zo
495
normal! zo
493
normal! zc
510
normal! zo
549
normal! zc
559
normal! zo
559
normal! zc
578
normal! zo
592
normal! zo
594
normal! zo
578
normal! zc
689
normal! zo
691
normal! zo
689
normal! zc
711
normal! zo
713
normal! zo
736
normal! zo
738
normal! zo
740
normal! zo
761
normal! zo
763
normal! zo
780
normal! zo
782
normal! zo
788
normal! zo
796
normal! zo
808
normal! zo
816
normal! zo
818
normal! zo
835
normal! zo
860
normal! zo
876
normal! zo
890
normal! zo
926
normal! zo
940
normal! zo
941
normal! zo
967
normal! zo
980
normal! zo
990
normal! zo
1009
normal! zo
1009
normal! zc
1082
normal! zo
990
normal! zc
980
normal! zc
1127
normal! zo
1143
normal! zo
1185
normal! zo
1199
normal! zo
1291
normal! zo
1291
normal! zc
1352
normal! zc
1360
normal! zo
1360
normal! zc
1377
normal! zo
1377
normal! zc
1402
normal! zo
1402
normal! zc
1418
normal! zo
1418
normal! zc
1431
normal! zc
let s:l = 275 - ((24 * winheight(0) + 25) / 50)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 275
normal! 029|
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
let s:l = 3718 - ((9 * winheight(0) + 5) / 10)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 3718
normal! 071|
wincmd w
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
134
normal! zo
147
normal! zo
152
normal! zo
279
normal! zo
420
normal! zo
429
normal! zo
505
normal! zo
509
normal! zo
520
normal! zo
528
normal! zo
536
normal! zo
553
normal! zo
561
normal! zo
576
normal! zo
591
normal! zo
604
normal! zo
614
normal! zo
625
normal! zo
636
normal! zo
650
normal! zo
let s:l = 436 - ((17 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 436
normal! 041|
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
let s:l = 36 - ((35 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 36
normal! 042|
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
