#!/usr/bin/env bash
set -euo pipefail

rm ip.txt
curl ifconfig.co > ip.txt
