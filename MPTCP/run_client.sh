#!/bin/bash

echo "Configure MPTCP Socket Options via sysctl"
echo "sudo ./mptcp_config_sysctl.sh"
sudo ./mptcp_config_sysctl.sh
echo "----------------------------"

echo "Running sudo ./HelloClient_MPTCP: "
echo ""
sudo ./HelloClient_MPTCP
