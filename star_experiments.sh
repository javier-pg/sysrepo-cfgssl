#!/bin/bash

# experiment parameters
initial_star_size=5
star_step=5
max_star_size=80
number_experiments_per_star=20

#echo "Running $number_experiments experiments for each star network, from 2 node to $max_star_size nodes"

# run the specified number of experiments for each star size
for star in `seq $initial_star_size $star_step $max_star_size`;
do
	echo "Experiments for star with $star nodes..."
	./star.sh $star $number_experiments_per_star
done
echo "Experiments finished"
