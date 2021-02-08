#!/bin/bash

# arguments
number_nodes=$1
number_experiments=$2
echo "Running $number_experiments experiments configuring a mesh network with $number_nodes nodes"

# run the specified number of experiments
containers=()
for round in `seq 1 $number_experiments`;
do
	echo "Experiment $round..."
	docker-compose up -d --build --scale netopeer=$number_nodes

	# wait until controller and nodes are running
	echo "Launching containers..."
	running=-1
	while (($running < $(($number_nodes + 1))))
	do
		sleep 1
		running=$(docker ps | grep -e "(healthy)" | wc -l)
	done
	echo "Launching containers... OK"


	# names of NSF's containers and controller's container #
	if [ $round -eq 1 ]; then
		containers=($(docker-compose ps | tail -n +4 | cut -d" " -f1 | sort -V))
		controller=$(docker-compose ps | head -n+3 | tail -n1 | cut -d" " -f1)
	fi

	echo "Launching mesh configuration..."
	# launch mesh configuration
	for nsf in `seq 0 $(($number_nodes + -1))`;
	do
		docker exec -dt ${containers[$nsf]} /etc/netopeer-config.sh
	done

	sleep 20 # need to be improved, for example, check when all nsfs have been completely configured
	echo "Launching mesh configuration... DONE"


# calculate the time for mesh configuration
	echo "Measuring times..."
	docker cp $controller:/capture.pcap .

	tcpdump -r capture.pcap > capture.txt

	first_message=$(cat capture.txt | grep "10.0.1.200.* > 10.0.1.*.830" | head -n 1) # first NETCONF message to NSF
	last_message=$(cat capture.txt | grep "10.0.1.200.* > 10.0.1.*.830" | tail -n 1)   # last ack from controller to NSF

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
	echo $number_nodes";"$duration >> "./experimental_data/"mesh_times.txt
	echo "Measuring times...DONE"

	rm capture.pcap capture.txt

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

echo "Experiments finished"
