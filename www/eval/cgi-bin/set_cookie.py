#!/usr/bin/env python3
import os
import html
import sys


# # Build headers
# headers = [
#     "HTTP/1.1 302 Found",

# ]

# # Add Set-Cookie header if needed

# headers.append(f"Set-Cookie: theme={"black"}; Max-Age=30; Path=/")

# # Send response
# sys.stdout.write("\r\n".join(headers))
# sys.stdout.write("\r\n" + body)
print("HTTP/1.1 302 Found\r")
print("Location: /cgi-bin/cookies.py\r")
print("Set-Cookie: theme=#da211a; Max-Age=30; Path=/cgi-bin/cookies.py\r")
print("\r")
