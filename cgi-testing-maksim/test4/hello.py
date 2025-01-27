# hello.py
# Make sure this file is executable: chmod +x hello.py

#!/usr/bin/env python3

import os
import sys

# Print HTTP headers
print("Content-Type: text/plain\n")

# Get query string from environment variables
query = os.environ.get("QUERY_STRING", "")
print(f"Query String: {query}")

# Read POST data from stdin
post_data = sys.stdin.read()
print(f"POST Data: {post_data}")

print("Hello, this is python!")