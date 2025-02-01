#!/usr/bin/php
<?php
// Tell PHP we're outputting HTML
header("Content-Type: text/html");

// Read the input from POST
$content_length = (int)$_SERVER['CONTENT_LENGTH'];
$post_data = file_get_contents("php://stdin");

// Process the input
parse_str($post_data, $parsed_data);
$username = isset($parsed_data['username']) ? $parsed_data['username'] : 'Anonymous';

// Generate response
echo "<html>\n";
echo "<head><title>PHP Test</title></head>\n";
echo "<body>\n";
echo "<h1>Hello from PHP!</h1>\n";
echo "<p>Received username: " . htmlspecialchars($username) . "</p>\n";
echo "</body>\n";
echo "</html>\n";
?>