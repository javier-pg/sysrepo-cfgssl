#!/bin/bash

# arguments
number_nodes=$1
number_experiments=$2
echo "Running $number_experiments experiments configuring a mesh network with $number_nodes nodes"

# run the specified number of experiments
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


	# names of NSF's containers and controller's container #
	if [ $round -eq 0 ]; then
		containers=($(docker-compose ps | tail -n +4 | cut -d" " -f1 | sort -V))
		controller=$(docker-compose ps | head -n+3 | tail -n1 | cut -d" " -f1)
	fi

	# launch mesh configuration
	echo "Registering nodes..."

	for nsf in `seq 0 $(($number_nodes - 1))`;
	do
		docker exec ${containers[$nsf]} /etc/register.sh
	done

	echo "Registering nodes... DONE"

	echo "Configuring mesh..."

	docker exec $controller /etc/configure_mesh.sh
	result=$?

	if [ $result -eq 0 ]; then
	#if [ $result -ge 0 ]; then
		echo "Configuring mesh... DONE"
		# calculate the time for mesh configuratio
		echo "Measuring times..."

		sleep 5

		docker cp $controller:/capture.pcap .

		tcpdump -r capture.pcap > capture.txt

		first_message=$(cat capture.txt | grep -E "10\.0\.1\.200\.[0-9]+ > 10\.0\.1\.[0-9]+\.830" | head -n1 ) # first NETCONF message to NSF
		last_message=$(cat capture.txt | grep -E "10\.0\.1\.[0-9]+\.830 > 10\.0\.1\.200\.[0-9]+" | tail -n1 )   # last ack from controller to NSF

		begin_time=$(echo $first_message | cut -d" " -f1)
		b_hours=$(echo $begin_time | cut -d":" -f1)
		b_minutes=$(echo $begin_time | cut -d":" -f2)
		b_seconds=$(echo $begin_time | cut -d":" -f3)
		b_time=$(echo "$b_hours * 24 * 60 + $b_minutes * 60 + $b_seconds" | bc -l)

		end_time=$(echo $last_message | cut -d" " -f1)
		e_hours=$(echo $end_time | cut -d":" -f1)
		e_minutes=$(echo $end_time | cut -d":" -f2)
		e_seconds=$(echo $end_time | cut -d":" -f3)
		e_time=$(echo "$e_hours * 24 * 60 + $e_minutes * 60 + $e_seconds" | bc -l)

		duration=$(echo $e_time - $b_time | bc -l)
		echo $number_nodes";"$begin_time";"$end_time";"$duration >> "./experimental_data/"big_mesh_times.txt
		echo "Measuring times...DONE"

		rm capture.txt #capture.pcap

		round=$(($round + 1))
	else
		echo "Configuring mesh... FAILED"
	fi


	echo "Stopping containers..."
	docker-compose stop
	exited=-1
	while (($exited < $(($number_nodes + 1))))
	do
		sleep 1
		exited=$(docker-compose ps | grep -Eiw 'Exit 0|Exit 137' | wc -l)
	done
	echo "Stopping containers... OK"

	#sleep 5
done
