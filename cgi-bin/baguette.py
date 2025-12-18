#!/usr/bin/env python3

import os
import urllib.parse

# CGI 헤더 (필수)
print("Content-Type: text/plain")
print()

query = os.environ.get("QUERY_STRING", "")
params = urllib.parse.parse_qs(query)

def safe_float(value, default):
    try:
        return float(value)
    except:
        return default

euros_str = params.get("euro", [""])[0]
price_str = params.get("price", ["1.1"])[0]

euros = safe_float(euros_str, 0.0)
price = safe_float(price_str, 1.1)

if price <= 0:
    price = 1.1

baguettes = int(euros / price) if euros > 0 else 0
print(baguettes)
