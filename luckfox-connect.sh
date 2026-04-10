# luckfox-connect.sh
IFACE=$(ip link | grep "enx" | awk '{print $2}' | tr -d ':')
sudo ip addr add 172.32.0.100/24 dev $IFACE
sudo ip link set $IFACE up
sudo nmap -sn 172.32.0.0/24 -e $IFACE | grep "report for"