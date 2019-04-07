my_path=""
ip_ports="dest_ip_ports.txt"

#Parses IP and Ports and stops respective services

function stopService()
{
	kill $(lsof -t -i:{port_num}) > /dev/null 2>&1
}

function parsePorts()
{
	while IFS="\n" read ip_port_combination;
	do
		IFS="," read ip_num port_num<<< "${ip_port_combination}"
		stopService
	done < "${ip_ports}"
}

parsePorts
