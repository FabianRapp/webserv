#!/usr/bin/env python3
import os
import cgi
import cgitb
import html
import sys

cgitb.enable()

UPLOAD_DIR = "uploads"

if not os.path.isdir(UPLOAD_DIR):
    os.makedirs(UPLOAD_DIR, exist_ok=True)

form = cgi.FieldStorage()

if "userfile" not in form or not form["userfile"].filename:
    body = """
    <html><body>
    <h2 style="color:#E74C3C;">No file was uploaded</h2>
    <a href="post.html" style="color:#4A90E2;">Back to upload page</a>
    </body></html>
    """
    headers = [
        "HTTP/1.1 200 OK",
        "Content-Type: text/html",
        f"Content-Length: {len(body.encode('utf-8'))}",
        ""
    ]
    sys.stdout.write("\r\n".join(headers))
    sys.stdout.write("\r\n" + body)
    sys.exit(0)

fileitem = form["userfile"]
filename = os.path.basename(fileitem.filename)
save_path = os.path.join(UPLOAD_DIR, filename)

# Save file
with open(save_path, 'wb') as f:
    while True:
        chunk = fileitem.file.read(1024 * 1024)
        if not chunk:
            break
        f.write(chunk)

body = f"""
<html>
<head>
    <title>Upload Success</title>
    <style>
        body {{
            background: #181818;
            color: #fff;
            text-align: center;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            padding-top: 5rem;
        }}
        img {{
            margin-top: 2rem;
            max-width: 360px;
            border-radius: 12px;
            box-shadow: 0 2px 12px #0008;
        }}
        a {{
            color: #4A90E2;
            text-decoration: underline;
        }}
    </style>
</head>
<body>
    <h1 style="color:#4A90E2;">Upload Successful!</h1>
    <p>File <strong>{html.escape(filename)}</strong> uploaded.</p>
    <img src="uploads/{html.escape(filename)}" alt="Uploaded image preview">
    <br><br>
    <a href="post.html">Back to upload page</a>
</body>
</html>
"""

headers = [
    "HTTP/1.1 200 OK",
    "Content-Type: text/html",
    f"Content-Length: {len(body.encode('utf-8'))}",
    ""
]
sys.stdout.write("\r\n".join(headers))
sys.stdout.write("\r\n" + body)

