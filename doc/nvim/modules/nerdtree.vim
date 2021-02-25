" Author        : Ruan E. Formigoni
" Created       : 23/12/2020
" License       : MIT


Plugin 'scrooloose/nerdtree'               " Sidebar file browser
Plugin 'jistr/vim-nerdtree-tabs'           " Keep nerdtree in sync with all tabs

nmap <LEADER>. :NERDTreeTabsToggle<CR>
let g:NERDTreeWinSize = 31
let g:NERDTreeWinSizeMax = 31
let g:NERDTreeDirArrowExpandable = ''
let g:NERDTreeDirArrowCollapsible = ''
" Fix for square brackets around icons
" src: https://github.com/ryanoasis/vim-devicons/issues/154
if exists('g:loaded_webdevicons')
  call webdevicons#refresh()
endif
