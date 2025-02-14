#!/usr/bin/env python3
import cgi
import os
import sys

# Debugging: Output raw environment variables and headers
sys.stdout.write("Content-Type: text/plain\r\n\r\n")
sys.stdout.write("Environment Variables:\n")
for key, value in os.environ.items():
    sys.stdout.write(f"{key}: {value}\n")

sys.stdout.write("\nForm Data:\n")
form = cgi.FieldStorage()
for field in form.keys():
    sys.stdout.write(f"{field}: {form[field].value}\n")
