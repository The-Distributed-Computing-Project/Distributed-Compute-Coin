let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/Code/Distributed-Compute-Coin/dcc_client/dcc_client
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +940 P2PClient.cpp
badd +27 SettingsConsts.h
badd +134 Main.cpp
badd +107 P2PClient.h
badd +790 Blockchain.cpp
badd +41 strops.h
badd +0 indicators.hpp
badd +97 Console.h
badd +0 ~/Code/Distributed-Compute-Coin/dcc_client/CMakeLists.txt
badd +15 CMakeLists.txt
badd +25 Blockchain.h
badd +0 Miner.cpp
argglobal
%argdel
$argadd P2PClient.cpp
set stal=2
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabrewind
edit P2PClient.cpp
argglobal
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
48
normal! zo
56
normal! zo
82
normal! zo
90
normal! zo
94
normal! zo
152
normal! zo
155
normal! zo
158
normal! zo
178
normal! zo
184
normal! zo
187
normal! zo
192
normal! zo
202
normal! zo
253
normal! zo
304
normal! zo
317
normal! zo
338
normal! zo
349
normal! zo
356
normal! zo
375
normal! zo
393
normal! zo
395
normal! zo
415
normal! zo
440
normal! zo
443
normal! zo
452
normal! zo
456
normal! zo
458
normal! zo
471
normal! zo
483
normal! zo
188
normal! zo
192
normal! zo
198
normal! zo
208
normal! zo
259
normal! zo
310
normal! zo
323
normal! zo
344
normal! zo
355
normal! zo
362
normal! zo
381
normal! zo
399
normal! zo
401
normal! zo
421
normal! zo
446
normal! zo
449
normal! zo
458
normal! zo
462
normal! zo
464
normal! zo
477
normal! zo
489
normal! zo
539
normal! zo
542
normal! zo
553
normal! zo
588
normal! zo
597
normal! zo
645
normal! zo
685
normal! zo
698
normal! zo
719
normal! zo
730
normal! zo
737
normal! zo
755
normal! zo
772
normal! zo
774
normal! zo
794
normal! zo
819
normal! zo
821
normal! zo
829
normal! zo
832
normal! zo
834
normal! zo
847
normal! zo
857
normal! zo
591
normal! zo
603
normal! zo
621
normal! zo
659
normal! zo
699
normal! zo
712
normal! zo
733
normal! zo
744
normal! zo
751
normal! zo
769
normal! zo
786
normal! zo
788
normal! zo
808
normal! zo
833
normal! zo
835
normal! zo
843
normal! zo
846
normal! zo
848
normal! zo
861
normal! zo
871
normal! zo
921
normal! zo
926
normal! zo
943
normal! zo
944
normal! zo
944
normal! zo
945
normal! zo
961
normal! zo
978
normal! zo
1046
normal! zo
1063
normal! zo
1079
normal! zo
1090
normal! zo
1091
normal! zo
1117
normal! zo
1120
normal! zo
1140
normal! zo
1148
normal! zo
1156
normal! zo
1171
normal! zo
1180
normal! zo
1195
normal! zo
1210
normal! zo
1223
normal! zo
1233
normal! zo
1244
normal! zo
1255
normal! zo
1266
normal! zo
1286
normal! zo
1296
normal! zo
1299
normal! zo
1304
normal! zo
1338
normal! zo
let s:l = 924 - ((16 * winheight(0) + 28) / 56)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 924
normal! 0
tabnext
edit Blockchain.cpp
argglobal
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
36
normal! zo
51
normal! zo
53
normal! zo
55
normal! zo
69
normal! zo
71
normal! zo
251
normal! zo
318
normal! zo
405
normal! zo
435
normal! zo
510
normal! zo
514
normal! zo
515
normal! zo
541
normal! zo
543
normal! zo
613
normal! zo
628
normal! zo
628
normal! zo
628
normal! zo
641
normal! zo
643
normal! zo
647
normal! zo
663
normal! zo
702
normal! zo
713
normal! zo
727
normal! zo
736
normal! zo
783
normal! zo
789
normal! zo
796
normal! zo
798
normal! zo
845
normal! zo
869
normal! zo
903
normal! zo
1045
normal! zo
1170
normal! zo
let s:l = 950 - ((26 * winheight(0) + 28) / 56)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 950
normal! 017|
tabnext
edit Miner.cpp
argglobal
balt Blockchain.cpp
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
16
normal! zo
30
normal! zo
161
normal! zo
172
normal! zo
195
normal! zo
218
normal! zo
228
normal! zo
266
normal! zo
364
normal! zo
let s:l = 397 - ((45 * winheight(0) + 28) / 56)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 397
normal! 05|
tabnext
edit Blockchain.h
argglobal
balt Blockchain.cpp
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 38 - ((37 * winheight(0) + 28) / 56)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 38
normal! 0
tabnext
edit Main.cpp
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
61
normal! zo
118
normal! zo
129
normal! zo
210
normal! zo
213
normal! zo
223
normal! zo
325
normal! zo
382
normal! zo
438
normal! zo
444
normal! zo
556
normal! zo
563
normal! zo
634
normal! zo
655
normal! zo
731
normal! zo
754
normal! zo
let s:l = 561 - ((45 * winheight(0) + 28) / 56)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 561
normal! 013|
tabnext
edit P2PClient.h
argglobal
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
51
normal! zo
let s:l = 31 - ((10 * winheight(0) + 28) / 56)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 31
normal! 017|
tabnext
edit SettingsConsts.h
argglobal
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 27 - ((26 * winheight(0) + 28) / 56)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 27
normal! 037|
tabnext 3
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
