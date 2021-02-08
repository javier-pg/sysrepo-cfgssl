# sysrepo-cfgssl
Mesh scenario for SDN-based TLS management framework

TLS servers based on Apache

### About the project
To be done

Requirements:
* ```Unix-based system``` (tested in Ubuntu 18.04)
* ```bash```
* ```docker```
* ```docker-compose```


### Mesh network

1. Execute the script ```mesh.sh```:
```
sudo ./mesh.sh number_nodes number_experiments
```
* ```number_nodes```: Number of nodes of the mesh
* ```number_experiments```: Number of experiments to launch

2. Experimental measurements are generated in ```experimental_data``` folder as follows:
* ```mesh_times.txt```: Time for controller to configure the mesh network of nodes. Each line presents a experiment, the first column reflects the number of nodes, the second column presents the associated configuration time (in seconds).
