#!/usr/bin/env python3

import os
import sys

print("Content-Type: text/plain\n")

# Read input from stdin
body = sys.stdin.read()

print(f"Hello, {body}!")
