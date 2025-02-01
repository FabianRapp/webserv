#include <iostream>
#include <string>
#include <unistd.h>    // For fork, execve, pipe
#include <sys/wait.h>  // For waitpid
#include <cstring>     // For strerror
#include <fcntl.h>     // For O_NONBLOCK

// Function to check if a string ends with a specific suffix (C++17 compatible)
bool endsWith(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Function to handle CGI requests
void handleCGI(const std::string &uri) {
    // Step 1: Check if the URI ends with a CGI-related extension
    if (!(endsWith(uri, ".py") || endsWith(uri, ".php"))) {
        std::cerr << "Not a CGI request: " << uri << std::endl;
        return;
    }

    // Step 2: Set up pipes for communication
    int inputPipe[2], outputPipe[2];
    if (pipe(inputPipe) == -1 || pipe(outputPipe) == -1) {
        perror("pipe");
        return;
    }

    // Step 3: Fork the process
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) { // Child process
        // Step 4: Redirect stdin and stdout to pipes
        dup2(inputPipe[0], STDIN_FILENO);  // Read from input pipe
        dup2(outputPipe[1], STDOUT_FILENO); // Write to output pipe

        // Close unused pipe ends
        close(inputPipe[1]);
        close(outputPipe[0]);

        // Step 5: Prepare arguments and environment variables for execve
        const char *scriptPath = uri.c_str();
        char *args[] = {const_cast<char *>("/usr/bin/python3"), const_cast<char *>(scriptPath), NULL};
        char *env[] = {NULL};  // Set environment variables here if needed

        // Step 6: Execute the script
        execve(args[0], args, env);

        // If execve fails
        perror("execve");
        exit(EXIT_FAILURE);
    } else { // Parent process
        // Close unused pipe ends
        close(inputPipe[0]);
        close(outputPipe[1]);

        // Step 7: Write POST data to input pipe (if needed)
        std::string postData = "username=JohnDoe";
        write(inputPipe[1], postData.c_str(), postData.size());
        close(inputPipe[1]);  // Close after writing

        // Step 8: Read output from CGI script
        char buffer[1024];
        ssize_t bytesRead;
        std::cout << "CGI Output:\n";
        while ((bytesRead = read(outputPipe[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            std::cout << buffer;  // Send this back as HTTP response body
        }
        close(outputPipe[0]);

        // Step 9: Wait for the child process to finish
        waitpid(pid, NULL, 0);
    }
}

int main() {
    std::string uri = "hello.py";  // Example URI for testing

    handleCGI(uri);

    return 0;
}
