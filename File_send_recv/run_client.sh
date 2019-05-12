#!/bin/bash

echo "Configure MPTCP Socket Options via sysctl"
echo "sudo ./mptcp_config_sysctl.sh"
sudo ./mptcp_config_sysctl.sh
echo "----------------------------"

echo "Running MPTCP Client"
echo ""
./file_send_client_args mptcp_2016_04_18.tar.gz

