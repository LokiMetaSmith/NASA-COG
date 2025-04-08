#download  Ubuntu Server 22.04.3 LTS                                                       
#copy to usb                                                                     
#plug in keyboard , display and usb to sbc
#power up sbc                                       
#when splash screen hit F12                                                      
#boot to usb

#install ubuntu server name it 'pubinv-sbc-#' where # is the serial number of the Single board computer                                                      th
#   (you can choose minimal install)                                             
#   use entire disk                                                              
#     (you can turn off lvm - don't need it)                                     
#   install open ssh server                                                      
#   no need for any extra programs (at this time)                                
#   set username and password   
#   username: user password:(strong password, record for later use)                                                  
#   add GitHub Keys to SSH account

#   let install finish                                                           
#   unplug usb flash drive
#   reboot                                                                       
#   add personal ssh key, use the password recorded previously 
#   ssh-copy-id user@pubinv-sbc-#

#ssh into sbc, you must be on a computer that has valid github repo keys already
#the -A forwards your local ssh key agent to the remote
#(where # is the S/N of the sbc)
#ssh -A  user@pubinv-sbc-# 
#run the following
#sudo apt install git
#git clone git@github.com:PubInv/NASA-MCOG.git

#chmod +x ./NASA-COG/'SBC setup instructions'
#run ./NASA-COG/'SBC setup instructions'

#//install your favorite editor (sudo apt install emacs-nox)                 
#install tmux https://github.com/tmux/tmux/wiki                                          
#edit /etc/dpkg/dpkg.cfg.d/excludes                                              
sudo apt update
sudo apt-get dist-upgrade -y
sudo apt install -y avahi-daemon bash-completion emacs-nox nano vim less build-essential python3-venv python3-pip git tmux

#   comment out the excludes for man and docs                                    
sudo apt install -y man-db manpages manpages-dev manpages-posix manpages-posix-dev                                                                            
sudo mv /usr/bin/man.REAL /usr/bin/man
sudo mandb -c

#use one of these
#wget https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -O get-platformio.py
curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
python3 get-platformio.py
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
sudo mkdir -p /usr/local/bin
sudo ln -s ~/.platformio/penv/bin/platformio /usr/local/bin/platformio
sudo ln -s ~/.platformio/penv/bin/pio /usr/local/bin/pio
sudo ln -s ~/.platformio/penv/bin/piodebuggdb /usr/local/bin/piodebuggdb

#install the local mcogserver
sudo apt install -y  build-essential git libssl-dev isc-dhcp-server procps dnsmasq hostapd iptables  iproute2 resolvconf firewalld fail2ban wormhole
git clone --recurse-submodules https://github.com/PubInv/mcogserver.git
# use following if recurse failed: git submodule init / git submodule update
#
cd ~/mcogserver 
make iotserver
sudo ln -s ~/mcogserver/iotserver /usr/local/bin/iotserver

#or maybe https://github.com/garywill/linux-router
#https://arstechnica.com/gadgets/2016/04/the-ars-guide-to-building-a-linux-router-from-scratch/
#cp dhcpd.conf /etc/dhcp/dhcpd.conf
#/etc/default/isc-dhcp-server INTERFACESv4="eth4"
#sudo systemctl restart isc-dhcp-server.service
#sudo systemctl start mcogs.service
#sudo systemctl enable mcogs.service

#use wormhole send ~/path/to/file
#and wormhole receive codeXYZ

