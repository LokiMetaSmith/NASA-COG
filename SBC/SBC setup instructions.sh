#SBC requirements recommendations
#Ethernet Ports |  one   | two
#WiFi           | none |yes 
#boot on power cycle | yes | yes
#

#download  Ubuntu Server 24.10.0 LTS                                                       
#copy to usb                                                                     
#plug in keyboard , display and usb to sbc
#power up sbc                                       
#when splash screen hit F12 or DEL                                                     
#boot to usb, and make sure the SBC powers on automatically

#install ubuntu server name it 'pubinv-sbc-#' where # is the serial number of the Single board computer                                                      th
#   (don't choose minimal install, unless more testing is done)                                             
#   use entire disk                                                              
#   (you can turn off lvm - don't need it)                                     
#   install open ssh server                                                      
#   no need for any extra programs (at this time)                                
#   set username and password   
#   username: user password:(strong password, record for later use)                                                  
#   add GitHub Keys to SSH account by providing your github user name (case sensitive)

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
#cd /usr/local/etc/
#git clone git@github.com:PubInv/NASA-MCOG.git

#chmod +x /usr/local/etc/NASA-COG/'SBC setup instructions'
#run /usr/local/etc/NASA-COG/'SBC setup instructions'

#//install your favorite editor (sudo apt install emacs-nox)                 
#install tmux https://github.com/tmux/tmux/wiki                                          
#edit /etc/dpkg/dpkg.cfg.d/excludes                                              
sudo apt update
sudo apt-get dist-upgrade -y
#check for firmware updates
sudo fwupdmgr get-upgrades
sudo fwupdmgr update -y

sudo apt install -y avahi-daemon bash-completion emacs-nox nano vim less build-essential python3-venv python3-pip git tmux

#   comment out the excludes for man and docs                                    
sudo apt install -y man-db manpages manpages-dev manpages-posix manpages-posix-dev                                                                            
sudo mv /usr/bin/man.REAL /usr/bin/man
sudo mandb -c
cd ~
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
sudo mkdir -p /var/www/mcogs
sudo apt install -y  build-essential git libssl-dev isc-dhcp-server procps dnsmasq hostapd iptables  iproute2 resolvconf firewalld fail2ban wormhole
cd /usr/local/etc/
sudo git clone --recurse-submodules https://github.com/PubInv/mcogserver.git
sudo ln -s /usr/local/etc/mcogserver /var/www/mcogs
# use following if recurse failed: git submodule init / git submodule update
#
cd /usr/local/etc/mcogserver 
sudo make iotserver
sudo ln -s /usr/local/etc/mcogserver/iotserver /usr/local/bin/iotserver
cd /usr/local/etc/NASA-MCOG/SBC
sudo systemctl start mcogs.service
sudo systemctl enable mcogs.service

#or maybe https://github.com/garywill/linux-router
# enp1s0 (ethernet port closest to USB-C power inlet) (OEDCS connection)
# enp3s0 (ethernet port next to enp1s0) (Internet source)
# wlp2s0 (wifi device) (Internet source)
cd /usr/local/etc/
sudo git clone https://github.com/garywill/linux-router
sudo chmod 755 /usr/local/etc/linux-router/lnxrouter
sudo ln -s /usr/local/etc/linux-router/lnxrouter /usr/local/bin/lnxrouter
cd /usr/local/etc/NASA-MCOG/SBC/
sudo systemctl enable linux-router.service
sudo systemctl start linux-router.service



#sudo lnxrouter -i enp1s0 -o enp3s0 wlp2s0 \
#--no-dns \
#--dhcp-dns 1.1.1.1  
#-6 \
#--dhcp-dns6  [2606:4700:4700::1111] \
#-g 192.168.5.254
#--daemon

sudo ufw allow ssh
sudo ufw allow 57575 /udp
sudo ufw status

#echo "" >> 

#table ip mangle
#delete table ip mangle

#table ip mangle {
#    chain prerouting {
#        type filter hook prerouting priority mangle; policy accept;
#        iifname "enp1s0" udp dport 57575 \
#      dup to 127.0.0.1 device lo udp dport set 57575 notrack     
#    }#
#
#    chain input {
#        type filter hook input priority mangle; policy accept;
#        iifname lo udp dport 57575 ip daddr set 127.0.0.1 notrack
#        
#    }
#}
#clear old tables if present
#sudo rm /etc/nftables.conf
sudo cp /usr/local/etc/NASA-MCOG/SBC/nftables.conf /etc/nftables.conf
#copy new tables to 
#echo "" >> /etc/nftables.conf
#echo "table inet nat {" >> /etc/nftables.conf
#echo "  chain prerouting { type nat hook prerouting priority dstnat; policy accept; }" >> /etc/nftables.conf
#echo "  chain postrouting { type nat hook postrouting priority masquerade; policy accept; }" >> /etc/nftables.conf
#echo "}" >> /etc/nftables.conf
#echo "" >> /etc/nftables.conf
#echo "table inet filter {" >> /etc/nftables.conf
#echo "  chain forward { type filter hook forward priority filter; policy accept; }" >> /etc/nftables.conf
#echo "}" >> /etc/nftables.conf
#echo "" >> /etc/nftables.conf
#echo "add rule inet nat prerouting iifname \"enp1s0\" udp dport 57575 daddr 192.168.5.254 tee to 127.0.0.1:57575" >> /etc/nftables.conf
#echo "add rule inet nat prerouting iifname \"enp1s0\" udp dport 57575 daddr 192.168.5.254 dnat to mcogs.coslabs.com:57575" >> /etc/nftables.conf
#echo "add rule inet filter forward iifname \"enp1s0\" oifname != \"enp1s0\" udp dport 57575 daddr mcogs.coslabs.com accept" >> /etc/nftables.conf
#echo "add rule inet nat postrouting oifname != \"enp1s0\" masquerade" >> /etc/nftables.conf


#sudo nft list rules 
#sudo systemctl enable nftables
#sudo systemctl start nftables
sudo nft -f /etc/nftables.conf

#should resolve remote server ip address correctly
dig +short "mcogs.coslabs.com"

#use wormhole send ~/path/to/file
#and wormhole receive codeXYZ
#or use scp such as winscp or scp on linux

# 
