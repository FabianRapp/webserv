<?php
// Set the content-type and other headers
header('Content-Type: text/html');
header('Set-Cookie: theme=dark; Max-Age=30');

// HTML content for the page
$body = <<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Color Toggle</title>
    <link rel="stylesheet" href="../css/style.css">
    <style>
        button {
            background: green;
        }
        button:hover {
            background: #43a047;
        }
    </style>
</head>
<body>

    <h1>CGI PHP</h1>

    <div class="button-container">
        <form action="../scripts/get_color.php" method="POST">
            <button id="shortButton" type="submit" name="color" value="original">Trigger CGI</button>
        </form>
    </div>

</body>
</html>
HTML;

// Output the content
echo $body;
?>
