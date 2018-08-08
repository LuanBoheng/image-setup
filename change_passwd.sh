#edit /etc/ssh/sshd_config remote ssh passwd login is needed
sudo mount -o remount,rw /
sudo chmod 640 /etc/shadow
sudo passwd ubuntu
