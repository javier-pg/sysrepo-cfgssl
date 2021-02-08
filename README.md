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


#### Mesh network

1. Execute the script ```mesh.sh```:
```
sudo ./mesh.sh number_nodes number_experiments
```
* ```number_nodes```: Number of nodes of the mesh
* ```number_experiments```: Number of experiments to launch

2. Experimental data is generated in ```experimental_data/mesh``` folder as follows:
* ```service_times.csv```: Time for controller to configure the mesh network. Each line presents a experiment, and columns reflect the number of nodes and associated configuration time in seconds.
