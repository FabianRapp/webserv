#!/usr/bin/env python3
import sys

body = """<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Color Toggle</title>
    <link rel="stylesheet" href="../css/style.css">
    <style>
 	    button {
 		    background: green;
             
 	    }         
      button:hover {
        background: #43a047;
      }

  </style>
</head>
<body>

    <h1>CGI Python</h1>

    <div class="button-container">
        <form action="../scripts/get_color.py" method="POST">
            <button id="shortButton" type="submit" name="color" value="original">Trigger CGI</button>
        </form>
    </div>

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
