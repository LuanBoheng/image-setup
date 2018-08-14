#add on alias
echo "ZSH_THEME=\"junkfood\"">> /root/.zshrc
echo "alias hh=\"history | grep \"">> /root/.zshrc
echo "alias pp=\"ps -A | grep \"">> /root/.zshrc
echo "alias gs=\"git status\"">> /root/.zshrc
echo "alias hh=\"git commit -m 'fast commit'\"">> /root/.zshrc
source /root/.zshrc
