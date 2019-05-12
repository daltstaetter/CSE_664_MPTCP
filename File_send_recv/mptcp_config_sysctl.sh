sudo sysctl -w net.mptcp.mptcp_checksum=0 net.mptcp.mptcp_debug=0 net.mptcp.mptcp_enabled=1 
sudo sysctl -w net.mptcp.mptcp_path_manager=fullmesh
sudo sysctl -w net.mptcp.mptcp_scheduler=redundant
