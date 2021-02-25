" Author        : Ruan E. Formigoni
" Created       : 23/12/2020
" License       : MIT

Plugin 'junegunn/fzf', { 'do': { -> fzf#install() } } " Fuzzy finder
Plugin 'junegunn/fzf.vim'

nmap <LEADER>nf :GFiles<CR>
let g:fzf_preview_window = ['right:50%', 'ctrl-/']
