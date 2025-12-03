#!/usr/bin/env python3

import os
import urllib.parse

# CGI 헤더 (필수)
print("Content-Type: text/plain")
print()

query = os.environ.get("QUERY_STRING", "")
params = urllib.parse.parse_qs(query)

try:
    euros = float(params.get("euro", [""])[0])
    baguettes = int(euros / 0.5)
    print(baguettes)
except:
    print("0")
