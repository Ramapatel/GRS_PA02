MT25061
Update system
sudo apt update

Install compiler, perf, python tools:

sudo apt install -y build-essential linux-tools-common linux-tools-$(uname -r) python3 python3-pandas python3-matplotlib


Create Network Namespaces (ONE-TIME SETUP)

Installation:

sudo ip netns add ns_server
sudo ip netns add ns_client

Create virtual cable:

sudo ip link add veth_s type veth peer name veth_c

Attach to namespaces:

sudo ip link set veth_s netns ns_server
sudo ip link set veth_c netns ns_client

Assign IPs:

sudo ip netns exec ns_server ip addr add 10.0.0.1/24 dev veth_s
sudo ip netns exec ns_client ip addr add 10.0.0.2/24 dev veth_c

Bring interfaces up:

sudo ip netns exec ns_server ip link set veth_s up
sudo ip netns exec ns_client ip link set veth_c up

Enable loopback:

sudo ip netns exec ns_server ip link set lo up
sudo ip netns exec ns_client ip link set lo up

Verify connectivity
sudo ip netns exec ns_client ping 10.0.0.1


compile Code (Part A)
make clean
make


Manual Test (VERY IMPORTANT)
Terminal 1 – Start Server

sudo ip netns exec ns_server ./server_a1 1024 4


Terminal 2 – Run Client

sudo ip netns exec ns_client ./client_a1 1024 4


Run Client with PERF (Part B)

Keep server running (Terminal 1).

In Terminal 2:

sudo perf stat \
  -e cycles,cache-misses,context-switches \
  ip netns exec ns_client ./client_a1 1024 4

Stop Server


Run AUTOMATED SCRIPT (Part C)
Make script executable

chmod +x MT25061_Part_C_ExperimentScript.sh

Run script

sudo ./MT25061_Part_C_ExperimentScript.sh

MT25061_Part_C_Results.csv