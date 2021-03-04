#!/bin/bash

# experiment parameters
max_mesh_size=25
number_experiments_per_mesh=30

echo "Running $number_experiments experiments for each mesh network, from 2 node to $max_mesh_size nodes"

# run the specified number of experiments for each mesh size
for mesh in `seq 2 $max_mesh_size`;
do
	echo "Experiments for mesh with $mesh nodes..."
	./mesh.sh $mesh $number_experiments_per_mesh
done
echo "Experiments finished"
