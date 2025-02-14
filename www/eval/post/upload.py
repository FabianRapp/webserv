#!/usr/bin/env python3
import os
import sys
import cgi

# Debug: Print environment variables to stderr (check CONTENT_TYPE, CONTENT_LENGTH)
print("DEBUG - Environment:", file=sys.stderr)
for key in ['REQUEST_METHOD', 'CONTENT_TYPE', 'CONTENT_LENGTH']:
    print(f"{key}: {os.environ.get(key, 'NOT SET')}", file=sys.stderr)

# Parse form data
form = cgi.FieldStorage()

# Debug: List all received form keys
print("DEBUG - Form keys:", list(form.keys()), file=sys.stderr)

try:
    file_item = form["userfile"]
    filename = file_item.filename
    # Sanitize filename to prevent path traversal
    filename = os.path.basename(filename)
    with open(f"uploads/{filename}", "wb") as f:
        f.write(file_item.file.read())
except KeyError:
    print("ERROR: 'userfile' field not found in form", file=sys.stderr)
    sys.exit(1)
