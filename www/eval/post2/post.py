#!/usr/bin/env python3
import os
import sys

# Read POST data
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
post_data = sys.stdin.read(content_length)

# Extract boundary from Content-Type header
content_type = os.environ.get('CONTENT_TYPE', '')
boundary = content_type.split("boundary=")[-1]

# Split POST data into parts
parts = post_data.split(f"--{boundary}")

# Iterate over parts to find the file part
for part in parts:
    if 'Content-Disposition: form-data; name="myfile";' in part:
        # Extract headers and content
        headers, file_content = part.split("\r\n\r\n", 1)
        file_content = file_content.rsplit(f"\r\n--{boundary}", 1)[0]  # Remove trailing boundary

        # Extract the filename from Content-Disposition header
        for line in headers.split("\r\n"):
            if line.startswith("Content-Disposition"):
                # Extract filename using string parsing
                filename = line.split("filename=")[-1].strip('"')
                break

        # Save file content to the extracted filename
        with open(filename, "w") as f:
            f.write(file_content)
        break

# Respond with success message
print("HTTP/1.1 200 OK\r")
print("Content-Type: text/html\r\n")
print("\r\n")
print(f"""
<html>
<body>
  <h1>File Uploaded Successfully</h1>
  <p>Saved as: {filename}</p>
</body>
</html>
""")
