#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

// Function to set up environment variables for CGI
char **setup_environment(const std::string &method, const std::string &query, const std::string &script_path, char *envp[]) {
    // Count existing environment variables
    int env_count = 0;
    while (envp[env_count] != nullptr) {
        env_count++;
    }

    // Allocate space for new environment variables
    char **new_envp = new char *[env_count + 5]; // +5 for CGI-specific variables
    int i = 0;

    // Copy existing environment variables
    for (; i < env_count; i++) {
        new_envp[i] = strdup(envp[i]); // Duplicate strings to avoid modifying the original envp
    }

    // Add CGI-specific environment variables
    new_envp[i++] = strdup(("REQUEST_METHOD=" + method).c_str());
    new_envp[i++] = strdup(("QUERY_STRING=" + query).c_str());
    new_envp[i++] = strdup(("SCRIPT_FILENAME=" + script_path).c_str());
    new_envp[i++] = strdup("REDIRECT_STATUS=200"); // Required by php-cgi
    new_envp[i] = nullptr; // Null-terminate the array

    return new_envp;
}

// Function to handle CGI execution
void handle_cgi(const std::string &script_path, const std::string &method, const std::string &query, char *envp[]) {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process: Execute CGI
        close(pipe_fd[0]); // Close read end of pipe
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to write end of pipe
        close(pipe_fd[1]);

        // Set up environment variables for CGI
        char **new_envp = setup_environment(method, query, script_path, envp);

        // Execute php-cgi
        char *args[] = {(char *)"php-cgi", NULL};
        // execve("/usr/bin/php-cgi", args, new_envp);
		execve("./custom_cgi", args, new_envp);

        // If execve fails
        perror("execve");
        exit(EXIT_FAILURE);
    } else {
        // Parent process: Read output from child
        close(pipe_fd[1]); // Close write end of pipe

        char buffer[1024];
        ssize_t bytes_read;

        std::cout << "HTTP/1.1 200 OK\r\n"; // Send HTTP headers
        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
            write(STDOUT_FILENO, buffer, bytes_read); // Send CGI output to client
        }

        close(pipe_fd[0]);
        waitpid(pid, NULL, 0); // Wait for child process to finish

        // Memory cleanup: Free dynamically allocated environment variables
        char **new_envp = setup_environment(method, query, script_path, envp);
        for (int i = 0; new_envp[i] != nullptr; i++) {
            free(new_envp[i]);
        }
        delete[] new_envp;
    }
}

// Main function to simulate an HTTP request and handle CGI execution
int main(int argc, char *argv[], char *envp[]) {
    // Simulated HTTP GET request for index.php with query parameters
    std::string method = "GET";
    std::string script_path = "index.php"; // Replace with the actual path to your PHP script
    std::string query = "name=John&age=30";

    std::cout << "Received request: " << method << " " << script_path << "\n";

    if (script_path.find(".php") != std::string::npos) {
        handle_cgi(script_path, method, query, envp); // Handle CGI request
    } else {
        std::cout << "HTTP/1.1 404 Not Found\r\n\r\n";
        std::cout << "<html><body><h1>File Not Found</h1></body></html>\r\n";
    }

    return 0;
}
