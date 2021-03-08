#!/bin/bash

# arguments
number_nodes=$1
number_experiments=$2
echo "Running $number_experiments experiments resizing a mesh network until $number_nodes nodes"


for time_between_nsfs in 0.05 0.1 0.25 0.5 1.0 2.0 5.0; #0.25 0.5 1.0 2.0 3.0 5.0 10.0 -1.0;
do
	arrival_times=()
	for nsf in `seq 0 $(($number_nodes - 1))`;
	do
		#if (( $(echo "$time_between_nsfs!=-1.0" | bc -l) )); then
		#	s=$(python -c"from random import expovariate; print(expovariate(1.0 / $time_between_nsfs))")
		#	arrival_times[$nsf]=$s
		#else
		#	arrival_times[$nsf]=-1.0
		#fi
		arrival_times[$nsf]=$time_between_nsfs
	done

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

		# names of NSF's containers and controller's container #
		#if [ $round -eq 0 ]; then
		containers=($(docker-compose ps | tail -n +4 | cut -d" " -f1 | sort -V))
		controller=$(docker-compose ps | head -n+3 | tail -n1 | cut -d" " -f1)
		#fi

		# launch mesh configuration
		echo "Registering nodes..."

		for nsf in `seq 0 $(($number_nodes - 1))`;
		do
			docker exec ${containers[$nsf]} /etc/register.sh
		done

		echo "Registering nodes... DONE"

		echo "Resizing network..."

		#arrival_times=()

		#for nsf in `seq 0 $(($number_nodes - 1))`;
		#do
			#if (( $(echo "$time_between_nsfs!=-1.0" | bc -l) )); then
			#	s=$(python -c"from random import expovariate; print(expovariate(1.0 / $time_between_nsfs))")
			#	arrival_times[$nsf]=$s
			#else
			#	arrival_times[$nsf]=-1.0
			#fi
		#done

		nsf=0
		for arrival_time in ${arrival_times[@]};
		do
			if (( $(echo "$arrival_time==-1.0" | bc -l) )); then
				docker exec $controller /etc/resize.sh
			else
				sleep $arrival_time
				docker exec -dt $controller /etc/resize.sh
			fi
			nsf=$(($nsf + 1))
		done

		echo "Resizing network... DONE"
		# calculate the time for mesh configuratio
		echo "Measuring times..."

		sleep 4

		docker cp $controller:/capture.pcap .

		containers_ips=$(tcpdump -r capture.pcap 'tcp[tcpflags] & (tcp-syn) != 0' | grep "10.0.1.* > 10.0.1.200.*Flags \[S\]" | cut -d" " -f3 | cut -d"." -f1,2,3,4 | sort | uniq)
		#echo $containers_ips
		containers_ips=($containers_ips)

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

			nsf=$(($nsf + 1))
			echo $time_between_nsfs";"$nsf";"$begin_time";"$end_time";"$duration >> "./experimental_data/"resizing_times_constant.txt

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
