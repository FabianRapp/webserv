#!/usr/bin/env python3
import os
import html
import sys

theme_val = os.getenv("theme")

headers = [
	"HTTP/1.1 200 OK",
	"Content-Type: text/html",
]

if theme_val == None:
	theme_val = "#706020;"

if theme_val == "#706020;":
	button = f"""
		<a href="/cgi-bin/set_cookie.py" class="big-button">START SESSION AND SET COOKIES</a>
	"""
else:
	button = "<h1 class='title'>SESSION IN PROGRESS!</h1>"

# Generate HTML body
body = f"""<!DOCTYPE html>
<html>
<head><title>CGI Demo</title></head>
<style>
	body {{
		display: flex;
		justify-content: center;
		background: {theme_val};
		margin: 0;
		height: 100vh;
		align-items: center;
		transition: background 0.3s ease;
	}}

	.big-button {{
		display: inline-block;
		padding: 30px 45px;
		font-size: 20px;
		font-weight: bold;
		color: #fff;
		background-color: #ec7d18;
		border-radius: 5px;
		text-align: center;
		text-decoration: none;
		border: 1px solid #feb1ff;
		transition: background-color 0.3s ease;
	}}

	.big-button:hover {{
		background-color: #d76f11e8;;
	}}
	.title {{
		font-size: 28px;
		color: #d2c63e;
	}}

</style>

<body>
	{button}
</body>
</html>
"""

headers.append(f"Content-Length: {len(body.encode('utf-8'))}")

headers.append("")

sys.stdout.write("\r\n".join(headers))
sys.stdout.write("\r\n" + body)
