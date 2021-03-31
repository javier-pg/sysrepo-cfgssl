#!/bin/bash
export DOCKER_CLIENT_TIMEOUT=600
export COMPOSE_HTTP_TIMEOUT=600

# arguments
number_nodes=$1
number_experiments=$2

echo "Running $number_experiments experiments resizing a star network until $number_nodes nodes"


for time_between_nsfs in 10.0 5.0 3.0 2.0 1.0 -1.0;
do

	echo "Running $number_experiments experiments with $number_nodes nodes and inter arrival time of $time_between_nsfs seconds"

	containers=()

	round=0
	while [ $round -lt $number_experiments ]
	do
		echo "Experiment $round..."

		echo "Launching containers..."
		docker-compose up -d --force-recreate --build --scale netopeer=$number_nodes

		# wait until controller and nodes are running
		running=-1
		while (($running < $(($number_nodes + 2))))
		do
			sleep 1
			running=$(docker ps --filter "status=running" | wc -l)
		done
		echo "Launching containers... OK"

		# wait until controller and nodes are running
		#running=-1
		#while (($running < $(($number_nodes + 1))))
		#do
		#	sleep 1
		#	running=$(docker ps | grep -e "(healthy)" | wc -l)
		#done
		#echo "Launching containers... OK"


		# names of NSF's containers and controller's container #
		#if [ $round -eq 0 ]; then
		containers=($(docker-compose ps | tail -n +4 | cut -d" " -f1 | sort -V))
		controller=$(docker-compose ps | head -n+3 | tail -n1 | cut -d" " -f1)
		#fi

		echo "Registering nodes..."

		containers_ips=()
		for nsf in `seq 0 $(($number_nodes - 1))`;
		do
			docker exec ${containers[$nsf]} /etc/register.sh
			ipaddress=$(docker inspect -f '{{ .NetworkSettings.Networks.sysrepocfgssl_sdn_control.IPAddress }}' ${containers[$nsf]})
			containers_ips[$nsf]=$ipaddress
		done

		echo "Registering nodes... DONE"

		echo "Resizing network..."

		docker exec $controller /etc/star_resize.sh $time_between_nsfs

		echo "Resizing network... DONE"
		# calculate the time for star configuratio
		echo "Measuring times..."

		sleep 3

		docker cp $controller:/capture.pcap .

		#containers_ips=$(tcpdump -r capture.pcap 'tcp[tcpflags] & (tcp-syn) != 0' | grep -E  "10\.0\.1\.200\.[0-9]+ > 10\.0\.1\.[0-9]+\.830.*Flags \[S\]" | cut -d" " -f5 | cut -d"." -f1,2,3,4)
		#containers_ips=($containers_ips)

		tcpdump -r capture.pcap > capture.txt

		nsf=0
		for container in ${containers_ips[@]};
		do
			first_message=$(cat capture.txt | grep -E "10\.0\.1\.200\.[0-9]+ > $container\.830" | head -n1 ) # first NETCONF message to NSF
			last_message=$(cat capture.txt | grep -E "$container\.830 > 10\.0\.1\.200\.[0-9]+" | tail -n1 )   # last ack from controller to NSF

			begin_time=$(echo $first_message | cut -d" " -f1)
			b_hours=$(echo $begin_time | cut -d":" -f1)
			b_minutes=$(echo $begin_time | cut -d":" -f2)
			b_seconds=$(echo $begin_time | cut -d":" -f3)
			b_time=$(echo "$b_hours * 24 * 60 + $b_minutes * 60 + $b_seconds" | bc -l)

			end_time=$(echo $last_message | cut -d" " -f1)
			e_hours=$(echo $end_time | cut -d":" -f1)
			e_minutes=$(echo $end_time | cut -d":" -f2)
			e_seconds=$(echo $end_time | cut -d":" -f3)
			e_time=$(echo "$e_hours * 24 * 60 +$e_minutes * 60 + $e_seconds" | bc -l)

			duration=$(echo $e_time - $b_time | bc -l)

			echo $time_between_nsfs";"$nsf";"$begin_time";"$end_time";"$duration >> "./experimental_data/"star_resizing_times.txt
			nsf=$(($nsf + 1))
		done

		echo "Measuring times...DONE"

		rm capture.txt capture.pcap

		round=$(($round + 1))

		echo "Stopping containers..."
		docker-compose stop
		exited=-1
		while (($exited < $(($number_nodes + 1))))
		do
			sleep 1
			exited=$(docker-compose ps | grep -Eiw 'Exit 0|Exit 137' | wc -l)
		done
		echo "Stopping containers... OK"

	done
done
