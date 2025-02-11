import sys
import os

# Read input from stdin as binary data
body = sys.stdin.buffer.read()

# Find the boundary
boundary_start_index = body.find(b"boundary=") + 9
boundary_end_index = body.find(b"\r\n", boundary_start_index)
boundary = body[boundary_start_index:boundary_end_index].strip()

# Split into parts
parts = body.split(b'--' + boundary)

# Remove the final boundary marker
if parts[-1].endswith(b'--'):
    parts = parts[:-1]

# Get the part with the image data and headers
image_part = parts[1]

# Find where the headers end
header_end_index = image_part.find(b"\r\n\r\n") + 4
image_content = image_part[header_end_index:]

# ==============================================
# Extract the original filename from the headers
# ==============================================
headers = image_part[:header_end_index].decode('utf-8')  # Decode headers to a string
filename_start = headers.find('filename="') + 10  # Skip 'filename="'
filename_end = headers.find('"', filename_start)
original_filename = headers[filename_start:filename_end]

# Construct the full path with the original filename
image_filename = os.path.join("www/default/images", original_filename)  # Keep original name

# Ensure the directory exists
os.makedirs("www/default/images", exist_ok=True)

# Save the file
with open(image_filename, "wb") as img_file:
    img_file.write(image_content)

print(f"Image saved as {image_filename}!")