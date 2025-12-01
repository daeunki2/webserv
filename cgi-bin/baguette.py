#!/usr/bin/env python3

import sys

data = sys.stdin.read().strip()

try:
    euros = float(data)
    baguettes = int(euros / 0.1)
    print(f"{euros} euros can buy {baguettes} baguettes")
except:
    print("Invalid input")