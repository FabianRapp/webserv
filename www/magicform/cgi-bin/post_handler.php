#!/usr/bin/php-cgi
<?php
// Output HTTP headers
header("Content-Type: text/html");

// Retrieve POST parameters
$username = isset($_POST['username']) ? htmlspecialchars($_POST['username']) : 'Unknown';
$email = isset($_POST['email']) ? htmlspecialchars($_POST['email']) : '';

// Output HTML content with the submitted data
echo "<html>";
echo "<head><title>POST Request Example</title></head>";
echo "<body>";
echo "<h1>Submitted Data</h1>";
if (!empty($username) && !empty($email)) {
	echo "<p><strong>Username:</strong> $username</p>";
	echo "<p><strong>Email:</strong> $email</p>";
} else {
	echo "<p style='color: red;'>Error: Missing username or email!</p>";
}
echo "</body>";
echo "</html>";
?>
