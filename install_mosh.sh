apt-get update
apt-get install mosh
-A INPUT -p udp --dport 60000:61000 -j ACCEPT
ufw allow 60000:61000/udp
mosh server
