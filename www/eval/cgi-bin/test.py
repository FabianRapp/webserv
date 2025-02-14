#!/usr/bin/env python3
import os
import html
import sys
import math

env_vars = "\n".join([f"{k}: {html.escape(v)}" for k, v in os.environ.items()])
sum_result = 2 + 2 + 2;

body = f"""<!DOCTYPE html>
<html>
<head><title>CGI Demo</title></head>
<style>
	body {{
		background: linear-gradient(135deg, #2d2d2d, #1a1a1a);
		color: #90EE90;
		padding: 20px;
	}}
	pre {{
		background: #1a1a1a;
		padding: 15px;
		border-radius: 5px;
		border: 1px solid #90EE90;
		margin: 20px auto;
	}}
</style>
<body>
	<h1>Environment Variables - PYTHON CGI</h1>
	<p>Calculation using math library: {sum_result} </p>
	<pre>{env_vars}</pre>
</body>
</html>"""

content_length = len(body.encode('utf-8'))

headers = [
	"HTTP/1.1 200 OK",
	"Content-Type: text/html",
	f"Content-Length: {content_length}",
	"Set-Cookie: theme=dark; Max-Age=30",
	""
]

sys.stdout.write("\r\n".join(headers))
sys.stdout.write("\r\n" + body)
