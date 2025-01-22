#!/usr/bin/env python3

import os

# List of 1xx HTTP status codes and their descriptions
status_codes = {
    521: "Web Server Is Down",
    522: "Connection Timed Out",
    523: "Origin Is Unreachable",
    525: "SSL Handshake Failed",
    530: "Site Frozen",
    599: "Network Connect Timeout Error"
}





# Directory to save the generated HTML files
output_dir = "./"

# Ensure the output directory exists
os.makedirs(output_dir, exist_ok=True)

# Template for the HTML content
html_template = """<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{code} {description}</title>
    <link rel="stylesheet" href="./css/style.css">
</head>
<body>
    <div class="container">
        <img src="./img/{code}.jpg" alt="Error {code} Image">
    </div>
</body>
</html>
"""

# Generate an HTML file for each 1xx status code
for code, description in status_codes.items():
    # Replace placeholders in the template
    html_content = html_template.format(code=code, description=description)

    # Define the output file path
    file_path = os.path.join(output_dir, f"{code}.html")

    # Write the HTML content to the file
    with open(file_path, "w") as file:
        file.write(html_content)

print(f"Generated {len(status_codes)} HTML files in '{output_dir}'")
