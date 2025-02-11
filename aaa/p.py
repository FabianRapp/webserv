#!/usr/bin/env python3
import os

# Template for the HTML file
TEMPLATE = """<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>{code} - {title}</title>
	<style>
		* {{
			margin: 0;
			padding: 0;
			font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
		}}

		body {{
			background: linear-gradient(135deg, #2d2d2d, #1a1a1a);
			height: 100vh;
			display: flex;
			align-items: center;
			justify-content: center;
			color: #ffffff;
		}}

		.container {{
			text-align: center;
			padding: 2rem;
		}}

		.error-code {{
			font-size: 8rem;
			font-weight: bold;
			color: #e74c3c;
			text-shadow: 5px 5px 4px rgba(0, 0, 0, 0.3);
			margin-bottom: 1rem;
		}}

		.error-message {{
			font-size: 1.5rem;
			margin-bottom: 2rem;
			color: #cccccc;
			letter-spacing: 2px;
			text-transform: uppercase;
		}}

		.explanation {{
			max-width: 600px;
			margin: 0 auto 2rem;
			color: #999999;
			line-height: 1.6;
		}}
	</style>
</head>
<body>
	<div class="container">
		<div class="error-code">{code}</div>
		<div class="error-message">{title}</div>
		<p class="explanation">
			{description}
		</p>
	</div>
</body>
</html>
"""

# Dictionary of all 4xx status codes with their titles and explanations
STATUS_CODES_5XX = {
    521: ("Web Server Is Down", "The origin server refused the connection."),
    522: ("Connection Timed Out", "The connection to the origin server timed out."),
    523: ("Origin Is Unreachable", "The origin server is unreachable."),
    525: ("SSL Handshake Failed", "The SSL handshake between Cloudflare and the origin server failed."),
    530: ("Site Is Frozen", "The site is frozen due to a billing issue or other administrative action."),
    599: ("Network Connect Timeout Error", "The network connection timed out beyond the server's control.")
}



# Function to generate HTML files
def generate_error_pages(output_dir="4xx_errors"):
    os.makedirs(output_dir, exist_ok=True)
    for code, (title, description) in STATUS_CODES_5XX.items():
        content = TEMPLATE.format(code=code, title=title, description=description)
        with open(f"{output_dir}/{code}.html", "w") as f:
            f.write(content)
        print(f"Generated {code}.html")  # Add this line

if __name__ == "__main__":
    generate_error_pages()
