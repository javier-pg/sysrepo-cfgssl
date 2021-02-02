#!/bin/bash

export DOCKER_CLIENT_TIMEOUT=200
export COMPOSE_HTTP_TIMEOUT=200


### PARAMETERS ###
number_nodes=3
#window_time=30
### PARAMETERS ###


service_times=()
reference_times=()
mean_arrival_times=()

for time_between_nsfs in "3.0"; # "1" "2";

	do

	echo "Running $1 experiments with inter arrival time of $time_between_nsfs seconds"


	#### Initialize arrays #####
	limit=$(echo "$number_nodes - 1" | bc -l)
	for nsf in `seq 0 $limit`;
	do
		service_times[$nsf]=0.0
		reference_times[$nsf]=0.0
		mean_arrival_times[$nsf]=0.0
	done
	#### Initialize arrays #####


	#### Calculate exponential inter-arrival times (the mean of 10 interactions) #####
	for i in `seq 1 10`;
	do
		arrival_times=()
		for nsf in `seq 0 $limit`;
		do
			s=$(python -c"from random import expovariate; print expovariate(1.0 / $time_between_nsfs)")
			arrival_times[$nsf]=$s
	done

	for nsf in `seq 0 $limit`;
	do
			acumulative=$(echo ${mean_arrival_times[$nsf]})
			sum=$(echo "$acumulative + ${arrival_times[$nsf]}" | bc -l)
			mean_arrival_times[$nsf]=$sum
		done
	done

	for nsf in `seq 0 $limit`;
	do
		acumulative=$(echo ${mean_arrival_times[$nsf]})
		mean_arrival_times[$nsf]=$(echo "$acumulative / 10.0" | bc -l)
	done
	#### Calculate exponential inter-arrival times (the mean of 10 interactions) #####



	#### Run the experiment $1 (argument of the script) times #####
	containers=()
	for round in `seq 1 $1`;
	do
		echo "EXPERIMENT ROUND $round..."
		sleep 0.3
		docker-compose up -d --build --scale netopeer=$number_nodes


	# Names of NSF's containers and controller's container #
	if [ $round -eq 1 ]; then
		containers=($(docker-compose ps | tail -n +4 | cut -d" " -f1 | sort -V))
		controller=$(docker-compose ps | head -n+3 | tail -n1 | cut -d" " -f1)
	fi

	docker exec -dt $controller python /py-sc/controller.py

	sleep 10

	nsf=0
	acumulative=0.00000000000
	for arrival_time in ${mean_arrival_times[@]};
	do
		sleep $arrival_time
		acumulative=$(echo $arrival_time + $acumulative | bc -l)
		reference_times[$nsf]=$acumulative  						### Absolute time when the NSF appeared
		docker exec -dt ${containers[$nsf]} /etc/netopeer-config.sh
		nsf=$(($nsf + 1))
	done

	sleep 10

	#### Calculate the controller's service time #####
	docker cp $controller:/capture.pcap .


	#docker-compose stop


	containers_ips=($(tcpdump -r capture.pcap 'tcp[tcpflags] & (tcp-syn) != 0' | grep "10.0.1.* > 10.0.1.200.*Flags \[S\]" | cut -d" " -f3 | cut -d"." -f1,2,3,4))
	tcpdump -r capture.pcap > capture.txt

	nsf=0
	for container in ${containers_ips[@]};
	do
		first_message=$(cat capture.txt | grep "$container\." | head -n1)
		second_message=$(cat capture.txt | grep "$container\." | tail -n1)

		begin_time=$(echo $first_message | cut -d" " -f1)
		b_hours=$(echo $begin_time | cut -d":" -f1)
		b_minutes=$(echo $begin_time | cut -d":" -f2)
		b_seconds=$(echo $begin_time | cut -d":" -f3)
		b_time=$(echo "$b_hours * 24 * 60 + $b_minutes * 60 + $b_seconds" | bc -l)

		end_time=$(echo $second_message | cut -d" " -f1)
		e_hours=$(echo $end_time | cut -d":" -f1)
		e_minutes=$(echo $end_time | cut -d":" -f2)
		e_seconds=$(echo $end_time | cut -d":" -f3)
		e_time=$(echo "$e_hours * 24 * 60 +$e_minutes * 60 + $e_seconds" | bc -l)


		duration=$(echo $e_time - $b_time | bc -l)
		acumulative=$(echo ${service_times[$nsf]})
		sum=$(echo "$acumulative + $duration" | bc -l)
		service_times[$nsf]=$sum

		nsf=$(($nsf + 1))
	done
	#### Calculate the controller's service time #####


	done
	#### Run the experiment with $1 (argument of the script)  ####


	# Mean of the results #
	for r in ${reference_times[@]};
	do
		echo $r >> reference_times.txt
	done

	for s in ${service_times[@]};
	do
		echo $s / $1 | bc -l >> service_times.txt
	done


	cat reference_times.txt | tr "." "," > "./experimental_data/"$time_between_nsfs"_reference_times.txt"
	cat service_times.txt | tr "." "," > "./experimental_data/"$time_between_nsfs$serv_time"_service_times.txt"

	mv capture.pcap "./experimental_data"
	mv "./experimental_data/capture.pcap" "./experimental_data/"$time_between_nsfs"_capture.pcap"

	mv capture.txt "./experimental_data"
	mv "./experimental_data/capture.txt" "./experimental_data/"$time_between_nsfs"_capture.txt"

	rm reference_times.txt service_times.txt
done
