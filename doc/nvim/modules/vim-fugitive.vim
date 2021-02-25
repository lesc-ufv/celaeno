" Author        : Ruan E. Formigoni
" Created       : 23/12/2020
" License       : MIT

Plugin 'tpope/vim-fugitive'                " Git wrapper for vim

nnoremap <silent> <leader>gs :Gstatus<CR><C-W><S-T>
nnoremap <silent> <leader>gc :Gcommit<CR>
nnoremap <silent> <leader>gm :Gmerge<CR>
nnoremap <silent> <leader>gp :Gpush<CR>
nnoremap <silent> <leader>gf :Gfetch<CR>
nnoremap <silent> <leader>gb :Git checkout
nnoremap <silent> <leader>gt :GV<CR>
