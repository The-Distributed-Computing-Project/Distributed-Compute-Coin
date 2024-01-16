let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd /mnt/d/Code/DC-Cryptocurrency/dcc_client/dcc_client
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
badd +1 term:///mnt/d/Code/DC-Cryptocurrency/dcc_client/dcc_client//725:\|:res\ 10
badd +236 term:///mnt/d/Code/DC-Cryptocurrency/dcc_client/dcc_client//731:/bin/bash
badd +421 P2PClient.cpp
badd +53 P2PClient.h
badd +924 term:///mnt/d/Code/DC-Cryptocurrency/dcc_client/dcc_client//189:/bin/bash
badd +0 term:///mnt/d/Code/DC-Cryptocurrency/dcc_client/dcc_client//222:/bin/bash
argglobal
%argdel
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
exe '1resize ' . ((&lines * 44 + 32) / 64)
exe '2resize ' . ((&lines * 16 + 32) / 64)
argglobal
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
79
normal! zo
94
normal! zo
let s:l = 188 - ((21 * winheight(0) + 22) / 44)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 188
normal! 080|
wincmd w
argglobal
if bufexists(fnamemodify("term:///mnt/d/Code/DC-Cryptocurrency/dcc_client/dcc_client//222:/bin/bash", ":p")) | buffer term:///mnt/d/Code/DC-Cryptocurrency/dcc_client/dcc_client//222:/bin/bash | else | edit term:///mnt/d/Code/DC-Cryptocurrency/dcc_client/dcc_client//222:/bin/bash | endif
if &buftype ==# 'terminal'
  silent file term:///mnt/d/Code/DC-Cryptocurrency/dcc_client/dcc_client//222:/bin/bash
endif
balt term:///mnt/d/Code/DC-Cryptocurrency/dcc_client/dcc_client//189:/bin/bash
setlocal fdm=manual
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 252 - ((15 * winheight(0) + 8) / 16)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 252
normal! 070|
wincmd w
2wincmd w
exe '1resize ' . ((&lines * 44 + 32) / 64)
exe '2resize ' . ((&lines * 16 + 32) / 64)
tabnext
edit P2PClient.cpp
argglobal
balt term:///mnt/d/Code/DC-Cryptocurrency/dcc_client/dcc_client//731:/bin/bash
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 609 - ((32 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 609
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
let s:l = 48 - ((47 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 48
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
