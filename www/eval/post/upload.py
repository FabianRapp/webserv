# #!/usr/bin/env python3
# import os
# import sys
# import cgi

# # Debug: Print environment variables to stderr (check CONTENT_TYPE, CONTENT_LENGTH)
# # print("DEBUG - Environment:", file=sys.stderr)
# # for key in ['REQUEST_METHOD', 'CONTENT_TYPE', 'CONTENT_LENGTH']:
# #     print(f"{key}: {os.environ.get(key, 'NOT SET')}", file=sys.stderr)

# # Parse form data
# form = cgi.FieldStorage()

# # Debug: List all received form keys
# print("DEBUG - Form keys:", list(form.keys()), file=sys.stderr)

# try:
#     file_item = form["userfile"]
#     filename = file_item.filename
#     # Sanitize filename to prevent path traversal
#     filename = os.path.basename(filename)
#     with open(f"uploads/{filename}", "wb") as f:
#         f.write(file_item.file.read())
# except KeyError:
#     print("ERROR: 'userfile' field not found in form", file=sys.stderr)
#     sys.exit(1)


# ------------------------------------

# import sys
# import os

# # Read input from stdin as binary data
# body = sys.stdin.buffer.read()

# # Define the boundary marker (without identifier for now)
# boundary_prefix = b"------WebKitFormBoundary"

# # Find the first boundary (start of the part)
# boundary_start_index = body.find(boundary_prefix)

# # Ensure boundary_start_index is valid (meaning we found a boundary)
# if boundary_start_index != -1:

#     boundary_end_index = body.find(b"\r\n", boundary_start_index)
    
#     boundary_id = body[boundary_start_index + len(boundary_prefix):boundary_end_index].strip()
    
#     start_boundary = boundary_prefix + boundary_id + b"\r\n"
    
#     end_boundary = b"------WebKitFormBoundary" + boundary_id + b"--\r\n"
    
#     part_start_index = body.find(start_boundary) + len(start_boundary)
#     part_end_index = body.find(end_boundary, part_start_index)

#     content = body[part_start_index:part_end_index]

#     headersIdxEnd = content.find(b"\r\n\r\n") + 4

#     headers = content[:headersIdxEnd]
#     content = content[headersIdxEnd:]

#     fileNameIdxStart = headers.find(b"filename=") + 1
#     fileNameIdxEnd = headers.find(b"\"", fileNameIdxStart)

#     fileName = headers[fileNameIdxStart:fileNameIdxEnd]

#     with open("www/eval/" + fileName, "wb") as img_file:
#         img_file.write(content)

#     print("Image saved successfully!")

# else:
#     print("Error: Boundary not found!")


# -----------------------------

import sys
import os

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
    os.makedirs("www/eval/uploads/", exist_ok=True)

    # Save the file to the specified path
    with open(f"{fileName}", "wb") as img_file:
        img_file.write(content)

    print(f"Image saved successfully as {fileName}!")

else:
    print("Error: Boundary not found!")
