<?php
$response = "No file uploaded.";
$debugMessages = [];

// Check request method
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $debugMessages[] = "Request method is POST.";

    // Check if $_FILES is empty
    if (empty($_FILES)) {
        $debugMessages[] = "The \$_FILES array is empty.";
    } else {
        $debugMessages[] = "The \$_FILES array is not empty.";
        $debugMessages[] = print_r($_FILES, true);
    }

    // Check if 'file' exists in $_FILES
    if (isset($_FILES['file'])) {
        $debugMessages[] = "'file' input found in \$_FILES.";

        // Check for upload errors
        switch ($_FILES['file']['error']) {
            case UPLOAD_ERR_OK:
                $debugMessages[] = "File uploaded successfully without errors.";
                break;
            case UPLOAD_ERR_INI_SIZE:
                $response = "The uploaded file exceeds the upload_max_filesize directive in php.ini.";
                break;
            case UPLOAD_ERR_FORM_SIZE:
                $response = "The uploaded file exceeds the MAX_FILE_SIZE directive specified in the HTML form.";
                break;
            case UPLOAD_ERR_PARTIAL:
                $response = "The uploaded file was only partially uploaded.";
                break;
            case UPLOAD_ERR_NO_FILE:
                $response = "No file was uploaded.";
                break;
            case UPLOAD_ERR_NO_TMP_DIR:
                $response = "Missing a temporary folder.";
                break;
            case UPLOAD_ERR_CANT_WRITE:
                $response = "Failed to write file to disk.";
                break;
            case UPLOAD_ERR_EXTENSION:
                $response = "A PHP extension stopped the file upload.";
                break;
            default:
                $response = "Unknown error occurred.";
        }
    } else {
        $debugMessages[] = "'file' input not found in \$_FILES.";
    }
} else {
    $debugMessages[] = "Request method is not POST.";
}

// Combine debug messages into response body
$responseBody = "$response\n\nDebug Information:\n" . implode("\n", $debugMessages);
$contentLength = strlen($responseBody);

// Output headers manually
echo "HTTP/1.1 200 OK\n";
echo "Content-Type: text/plain\n";
echo "Content-Length: $contentLength\n\n";

// Output response body with debug information
echo $responseBody;
?>
