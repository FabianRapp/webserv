# #!/usr/bin/env python3

import sys
import os
import html

try:
	body = sys.stdin.buffer.read()

	boundary_prefix = b"------WebKitFormBoundary"

	boundary_start_index = body.find(boundary_prefix)

	if boundary_start_index != -1:

		boundary_end_index = body.find(b"\r\n", boundary_start_index)
		boundary_id = body[boundary_start_index + len(boundary_prefix):boundary_end_index].strip()
		start_boundary = boundary_prefix + boundary_id + b"\r\n"
		end_boundary = b"------WebKitFormBoundary" + boundary_id + b"--\r\n"
		part_start_index = body.find(start_boundary) + len(start_boundary)
		part_end_index = body.find(end_boundary, part_start_index)
		content = body[part_start_index:part_end_index]
		headersIdxEnd = content.find(b"\r\n\r\n") + 4
		headers = content[:headersIdxEnd]
		content = content[headersIdxEnd:]
		fileNameIdxStart = headers.find(b"filename=") + len(b'filename="')
		fileNameIdxEnd = headers.find(b'"', fileNameIdxStart)
		fileName = headers[fileNameIdxStart:fileNameIdxEnd].decode("utf-8")
		os.makedirs("uploads/", exist_ok=True)

		with open(f"uploads/{fileName}", "wb") as img_file:
			img_file.write(content)

	else:
		throw(1)

	success_page = f"""<!DOCTYPE html>
	<html>
	<head><title>C</title></head>
	<style>
		body {{
			background: #191a1d;
			color: #90EE90;
			padding: 20px;
		}}
		h1 {{
			text-align: center;

		}}
	</style>
	<body>
		<h1>Upload was successful!</h1>
	</body>
	</html>"""

	headers1 = [
		"HTTP/1.1 201 OK",
		"Content-Type: text/html",
		f"Content-Length: {len(success_page)}",
		""
	]

	sys.stdout.write("\r\n".join(headers1))
	sys.stdout.write("\r\n" + success_page)
except Exception as e:
	print("HTTP/1.1 500 Internal Server Error\r\n\r")
