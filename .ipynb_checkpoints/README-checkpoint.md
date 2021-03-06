# sysrepo-cfgssl
Mesh scenario for SDN-based TLS management framework

Requirements:
* ```Unix-based system``` (tested on Ubuntu 18.04)
* ```bash```
* ```docker```
* ```docker-compose```


## About the project


The Transport Layer Security (TLS) protocol is widely employed to protect end-to-end communications between network peers (applications or nodes). However, the administrators usually have to configure parameters (e.g. cryptography algorithms or authentication credentials) for the establishment of TLS connections manually. However, this way of managing security associations becomes infeasible when the number of network peers is high.

Our TLS management framework allows configuring and managing TLS security associations in a dynamic and an autonomous manner. The proposed solution, which is based on well-known standardized protocols, models the necessary configuration parameters to establish a secure TLS connection between network peers. Nowadays, this is required in several application scenarios such as virtual private networks (VPNs), virtualized network functions (VNFs) or service function chains (SFCs). Our framework is based on standard elements of the Software Defined Networking (SDN) paradigm, which is widely adopted to provide flexibility to network management, and it is being deployed for the aforementioned particular scenarios.

This repository contains an implemented proof of concept, not only to validate the suitability of the proposed solution but also to evaluate its capacity to afford the dynamic configuration of TLS connections. In particular, we evaluate the framework to configure (1) a mesh network launching N nodes at the same time, (2) a mesh network through resizing from the first node to the Nth node (in which nodes could arrive following a Poisson process), and (3) a star network launching N nodes at the same time.

We have executed experiments for the aforementioned tasks (```mesh_experiments.sh```, ```mesh_resizing.sh```, and ```star_experiments.sh```), and results reside in ```experimental_data```: ```mesh_times.txt```, ```mesh_resizing_times.txt```, and ```star_times.txt```. The performance analysis (```/experimental_data/performance_analysis.ipynb```) confirms that the implementation of this framework enables a fast and flexible procedure to manage TLS security associations between network peers.

## TASK 1: To form a mesh of n nodes

1. Execute the script ```mesh.sh```:
```bash
sudo ./mesh.sh number_nodes number_experiments
```
* ```number_nodes```: Number of nodes of the mesh
* ```number_experiments```: Number of experiments to launch

2. Experimental measurements are generated in ```experimental_data``` folder as follows:
* ```mesh_times.txt```: Time for controller to configure the mesh network of nodes. Each line is generated by a new experiment: the first column reflects the number of nodes, the second and third columns present the initial and final timestamp of the configuration process, and the fourth contains the associated configuration time (the difference of timestamps, in seconds).
```csv
number_of_nodes ; initial_timestamp ; final_timestamp ; configuration_duration
```

3. To automatize experiments with different number of nodes, edit the following variables in the script ```mesh_experiments.sh```:
```bash
initial_mesh_size=X
mesh_step=Y
max_mesh_size=Z
number_experiments_per_mesh=T
```

4. Execute to configure mesh networks of size from ```X``` nodes to ```Z``` nodes, with increments of ```Y``` nodes. Each configuration is launched ```T``` times each one (experimental data is also saved in ```/experimental_data/mesh_times.txt```):
```bash
sudo ./mesh_experiments.sh
```

5. The notebook ```/experimental_data/performance_analysis.ipynb``` implements the code to analyze the experimental data


## TASK 2: To add a new node to an existing mesh

1. Execute the script ```mesh_resizing.sh```:
```bash
sudo ./mesh_resizing.sh number_nodes number_experiments
```
* ```number_nodes```: Final number of nodes of the mesh
* ```number_experiments```: Number of experiments to launch

  By default, the script supposes an arrival process of the nodes based on a Poisson process. In particular, we propose different scenario conditions with averaged inter arrival times of ```10.0, 5.0, 3.0, 2.0,``` and ```1.0``` seconds. Specially, we denote with the value ```-1.0``` an specific scenario in which a new node is configured when the previous one has just been set up.

2. To edit the inter-arrival times of the nodes, play with the _```time_between_nsfs```_ variable in ```mesh_experiments.sh```:
```bash
for time_between_nsfs in 10.0 5.0 3.0 2.0 1.0 -1.0;
do
    ...
done
```

3. Experimental measurements are generated in ```experimental_data``` folder as follows:
* ```mesh_resizing_times.txt```: Time for controller to configure each node of the mesh network. Each line presents the time of configuration of each node for each experiment, the first column reflects the number of nodes, the second column presents the associated configuration time (in seconds).

```csv
scenario_inter_arrival_time ; ith_node ; node_initial_timestamp ; node_final_timestamp ; node_configuration_duration
```

4. The notebook ```/experimental_data/performance_analysis.ipynb``` implements the code to analyze the experimental data


## TASK 3: To form a star topology of n nodes

1. Execute the script ```star.sh```:
```bash
sudo ./star.sh number_nodes number_experiments
```
* ```number_nodes```: Number of nodes of the star
* ```number_experiments```: Number of experiments to launch

2. Experimental measurements are generated in ```experimental_data``` folder as follows:
* ```star_times.txt```: Time for controller to configure the star network of nodes. Each line is generated by a new experiment: the first column reflects the number of nodes, the second and third columns present the initial and final timestamp of the configuration process, and the fourth contains the associated configuration time (the difference of timestamps, in seconds).
```csv
number_of_nodes ; initial_timestamp ; final_timestamp ; configuration_duration
```

3. To automatize experiments with different number of nodes, edit the following variables in the script ```star_experiments.sh```:
```bash
initial_star_size=X
star_step=Y
max_mesh_size=Z
number_experiments_per_star=T
```

4. Execute to configure star networks of size from ```X``` nodes to ```Z``` nodes, with increments of ```Y``` nodes. Each configuration is launched ```T``` times each one (experimental data is also saved in ```/experimental_data/star_times.txt```):
```bash
sudo ./star_experiments.sh
```

5. The notebook ```/experimental_data/performance_analysis.ipynb``` implements the code to analyze the experimental data
