#include <iostream>
#include <cstdlib>
#include <string>

int main() {
    // Read environment variables
    const char *method = getenv("REQUEST_METHOD");
    const char *query = getenv("QUERY_STRING");

    // Output HTTP headers
    std::cout << "Content-Type: text/html\n\n";

    // Output HTML content
    std::cout << "<html><head><title>Custom CGI</title></head><body>";
    std::cout << "<h1>Hello from Custom CGI!</h1>";

    if (method) {
        std::cout << "<p>Request Method: " << method << "</p>";
    }
    if (query) {
        std::cout << "<p>Query String: " << query << "</p>";
    }

    std::cout << "</body></html>";

    return 0;
}
