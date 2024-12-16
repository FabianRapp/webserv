from http.server import HTTPServer, BaseHTTPRequestHandler

class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        # Override to suppress default logging
        pass

    def save_http_message(self, method, body=b""):
        with open("http_message.bin", "wb") as f:
            f.write(f"{method} {self.path} {self.protocol_version}\r\n".encode('utf-8'))
            for header, value in self.headers.items():
                f.write(f"{header}: {value}\r\n".encode('utf-8'))
            f.write(b"\r\n")
            if body:
                f.write(body)

    def do_GET(self):
        self.save_http_message("GET")
        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.end_headers()
        self.wfile.write(b"Hello, HTTP/1.1!\n")

    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)

        self.save_http_message("POST", post_data)
        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.end_headers()
        response = f"You POSTed: {post_data.decode('utf-8')}\n"
        self.wfile.write(response.encode('utf-8'))

httpd = HTTPServer(('0.0.0.0', 8080), SimpleHTTPRequestHandler)
print("Serving on port 8080...")
httpd.serve_forever()
