#!/usr/bin/env python3
import os
import html
import sys

print("HTTP/1.1 302 Found\r")
print("Location: /cgi-bin/cookies.py\r")
print("Set-Cookie: theme=#da211a; Max-Age=30; Path=/cgi-bin/cookies.py\r")
print("\r")
