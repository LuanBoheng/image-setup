sudo add-apt-repository ppa:jonathonf/vim
sudo apt update
sudo apt install vim
sudo apt install cmake

sudo apt-get install vim-gtk exuberant-ctags git
rm -rf ~/.vim ~/.vimrc
git clone https://github.com/ruchee/vimrc.git ~/vimrc
mv ~/vimrc/vimfiles ~/.vim
mv ~/vimrc/_vimrc ~/.vimrc
mv monaco.ttf ~/.fonts
wget https://github.com/todylu/monaco.ttf/blob/master/monaco.ttf\?raw\=true
mv monaco.ttf\?raw=true monaco.ttf
mv monaco.ttf ~/.fonts
