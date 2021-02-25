" Author        : Ruan E. Formigoni
" Created       : 23/12/2020
" License       : MIT

Plugin 'mcchrish/nnn.vim'

" Disable default mappings
let g:nnn#set_default_mappings = 0
" Then set your own
nnoremap <silent> <leader>nn :NnnPicker<CR>
" Floating window (neovim only for now)
let g:nnn#layout = { 'window': { 'width': 0.9, 'height': 0.6, 'highlight': 'Debug' } }
" Open in tabs or splits
let g:nnn#action = {
      \ '<c-t>': 'tab split',
      \ '<c-s>': 'split',
      \ '<c-v>': 'vsplit' }
