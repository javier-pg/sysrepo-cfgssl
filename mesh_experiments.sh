#!/bin/bash

# experiment parameters
initial_mesh_size=5
mesh_step=5
max_mesh_size=80
number_experiments_per_mesh=1

#echo "Running $number_experiments experiments for each mesh network, from 2 node to $max_mesh_size nodes"

# run the specified number of experiments for each mesh size
for mesh in `seq $initial_mesh_size $mesh_step $max_mesh_size`;
do
	echo "Experiments for mesh with $mesh nodes..."
	./mesh.sh $mesh $number_experiments_per_mesh
done
echo "Experiments finished"
