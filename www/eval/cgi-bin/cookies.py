#!/usr/bin/env python3
import os
import html
import sys


env_vars = "\n".join([f"{k}: {html.escape(v)}" for k, v in os.environ.items()])
theme_val = os.getenv("theme")

# Parse cookies from environment variables
cookies = {}
if "HTTP_COOKIE" in os.environ:
    cookies = dict(cookie.split("=") for cookie in os.environ["HTTP_COOKIE"].split("; "))

# Check if we need to set the cookie (only once per click)
set_cookie = False
if "HTTP_X_SET_THEME" in os.environ:
    if os.environ["HTTP_X_SET_THEME"] == "dark" and cookies.get("theme") != "dark":
        set_cookie = True

# Set initial state
theme_set = cookies.get("theme") == "dark"
show_button = not theme_set
theme_status = "Theme is set to dark!" if theme_set else "Theme not set"
test_var = "#1a1a1a" if theme_set else "red"

# Build headers
headers = [
    "HTTP/1.1 200 OK",
    "Content-Type: text/html",
]

# Add Set-Cookie header if needed
if theme_val == None:
	theme_val = "white"

#headers.append(f"Set-Cookie: theme={theme_val}; Max-Age=30; Path=/")
if theme_val == "white":
	button = f"""
		{'<button onclick="setDarkTheme()">Enable Dark Theme</button>' if show_button else ''}
		<a href="/cgi-bin/set_cookie.py">NAME</a>";
	"""
else:
	button = ""

# Generate HTML body
body = f"""<!DOCTYPE html>
<html>
<head><title>CGI Demo</title></head>
<style>
    body {{
        background: {theme_val};
        color: #90EE90;
        padding: 20px;
        transition: background 0.3s ease;
    }}
    button {{
        background: #90EE90;
        color: black;
        padding: 10px 20px;
        border: none;
        cursor: pointer;
    }}
</style>
<script>
function setDarkTheme() {{
    fetch(window.location.href, {{
        method: 'POST',
        headers: {{ 'X-Set-Theme': 'dark' }},
        credentials: 'same-origin'
    }}).then(() => window.location.reload());
}}
</script>
<body>
    <h1>Cookie Demo</h1>
	<p>theme_val: {theme_val}</p>
    <p>{theme_status}</p>
	<pre>{env_vars}</pre>
	{button}
</body>
</html>
"""

# Add final headers
headers.append(f"Content-Length: {len(body.encode('utf-8'))}")

headers.append("")

# Send response
sys.stdout.write("\r\n".join(headers))
sys.stdout.write("\r\n" + body)
