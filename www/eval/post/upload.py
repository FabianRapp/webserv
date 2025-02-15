# #!/usr/bin/env python3

import sys
import os
import html

ERROR_500 = """<!DOCTYPE html>
<html>
<head><title>500 Error</title></head>
<body>
<h1>500 Internal Server Error</h1>
<p>Something went wrong during file upload.</p>
</body>
</html>
"""

try:
	# Read input from stdin as binary data
	body = sys.stdin.buffer.read()

	# Define the boundary marker (without identifier for now)
	boundary_prefix = b"------WebKitFormBoundary"

	# Find the first boundary (start of the part)
	boundary_start_index = body.find(boundary_prefix)

	# Ensure boundary_start_index is valid (meaning we found a boundary)
	if boundary_start_index != -1:

		# Find the boundary end (start of the identifier + CRLF)
		boundary_end_index = body.find(b"\r\n", boundary_start_index)

		# Extract the identifier
		boundary_id = body[boundary_start_index + len(boundary_prefix):boundary_end_index].strip()

		# Full start boundary (including identifier and \r\n)
		start_boundary = boundary_prefix + boundary_id + b"\r\n"

		# Full end boundary (including identifier and the '--' suffix)
		end_boundary = b"------WebKitFormBoundary" + boundary_id + b"--\r\n"

		# Find the start and end of the data part (content between boundaries)
		part_start_index = body.find(start_boundary) + len(start_boundary)
		part_end_index = body.find(end_boundary, part_start_index)

		# Extract the content between the boundaries (i.e., the file's data)
		content = body[part_start_index:part_end_index]

		# Find the end of headers by looking for "\r\n\r\n"
		headersIdxEnd = content.find(b"\r\n\r\n") + 4

		# Extract the headers
		headers = content[:headersIdxEnd]
		content = content[headersIdxEnd:]

		# Extract filename from headers
		fileNameIdxStart = headers.find(b"filename=") + len(b'filename="')
		fileNameIdxEnd = headers.find(b'"', fileNameIdxStart)

		# Extract the filename
		fileName = headers[fileNameIdxStart:fileNameIdxEnd].decode("utf-8")  # decode bytes to string

		# Ensure the directory exists
		os.makedirs("uploads/", exist_ok=True)

		# Save the file to the specified path
		with open(f"uploads/{fileName}", "wb") as img_file:
			img_file.write(content)

		# print(f"Image saved successfully as {fileName}!")

	else:
		throw(1)
		# print("Error: Boundary not found!")

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
