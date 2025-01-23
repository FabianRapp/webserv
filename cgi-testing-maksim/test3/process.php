<?php
header("Content-Type: text/plain");

// Get POST data
$username = $_POST['username'] ?? 'No username provided';

echo "Hello, $username!";
?>
