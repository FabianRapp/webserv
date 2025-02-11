#!/usr/bin/env python3
import sys

# Your HTML content
body = """<!DOCTYPE html>
<html>
<head><title>CGI Demo</title></head>
<style>
	body {
		background: linear-gradient(135deg, #2d2d2d, #1a1a1a);
		color: #90EE90;
	}
</style>
<body>
  <h1>Hello CGI World!</h1>
  <p>This demonstrates HTTP headers in Python CGI</p>
  <h2>Hello h2</h2>
</body>
</html>"""

# Calculate content length in bytes
content_length = len(body.encode('utf-8'))

# Build headers
headers = [
    "HTTP/1.1 200 OK",
    "Content-Type: text/html",
    f"Content-Length: {content_length}",
    "Set-Cookie: theme=dark; Max-Age=30",
    ""  # Empty string for final CRLF
]

# Print headers and body
sys.stdout.write("\r\n".join(headers))
sys.stdout.write("\r\n" + body)
