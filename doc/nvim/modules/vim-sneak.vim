" Author        : Ruan E. Formigoni
" Created       : 23/12/2020
" License       : MIT

Plugin 'justinmk/vim-sneak'                         " Fast movements

let g:sneak#label = 1
let g:sneak#prompt = "> "

highlight Sneak guifg=black guibg=red ctermfg=black ctermbg=red
highlight SneakScope guifg=red guibg=yellow ctermfg=red ctermbg=yellow

nmap <LEADER>s <Plug>Sneak_s
nmap <LEADER>S <Plug>Sneak_S

hi! link Sneak Search
