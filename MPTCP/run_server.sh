#!/bin/bash

echo "Configure MPTCP Socket Options via sysctl"
echo "sudo ./mptcp_config_sysctl.sh"
sudo ./mptcp_config_sysctl.sh
echo "----------------------------"

echo "Running sudo ./HelloServer4_MPTCP:"
echo ""
sudo ./HelloServer4_MPTCP
