sudo apt-get update
sudo apt-get install mosh
#-A INPUT -p udp --dport 60000:61000 -j ACCEPT
sudo ufw allow 60000:61000/udp
mosh server
