<?php
/**
 * PEBL Simple Data Server - File Upload Handler
 *
 * Handles file uploads from PEBL experiments.
 * Authenticates using username and upload_password,
 * then saves uploaded files to task/subject directories.
 *
 * Usage from PEBL upload.json:
 * {
 *   "server": "yourdomain.com",
 *   "page": "/path/to/upload.php",
 *   "port": "80",
 *   "username": "admin",
 *   "upload_password": "your-password",
 *   "taskname": "stroop"
 * }
 */

require_once(__DIR__ . '/config.php');

// Main execution
try {
    // Authenticate user
    if (!authenticate()) {
        respondAndExit("Authentication failed", false);
    }

    debugLog("Authentication successful");

    // Handle file upload
    $result = handleFileUpload();
    respondAndExit($result['message'], $result['success']);

} catch (Exception $e) {
    respondAndExit("Server error: " . $e->getMessage(), false);
}

/**
 * Authenticate user with username and password
 */
function authenticate() {
    $username = $_POST['user_name'] ?? '';
    $password = $_POST['upload_password'] ?? '';

    debugLog("Attempting auth for user: $username");

    if (empty($username) || empty($password)) {
        debugLog("Username or password empty");
        return false;
    }

    if ($username !== ADMIN_USERNAME) {
        debugLog("Username mismatch");
        return false;
    }

    if (!password_verify($password, ADMIN_PASSWORD_HASH)) {
        debugLog("Password verification failed");
        return false;
    }

    return true;
}

/**
 * Handle the file upload process
 */
function handleFileUpload() {
    // Check if file was uploaded
    if (!isset($_FILES['fileToUpload'])) {
        return ['success' => false, 'message' => 'No file uploaded'];
    }

    $file = $_FILES['fileToUpload'];

    // Check for upload errors
    if ($file['error'] !== UPLOAD_ERR_OK) {
        return ['success' => false, 'message' => 'Upload error code: ' . $file['error']];
    }

    // Get task name and subject number from POST
    $taskname = sanitizeFilename($_POST['taskname'] ?? 'data');
    $subnum = sanitizeFilename($_POST['subnum'] ?? '000');

    debugLog("Task: $taskname, Subject: $subnum");

    // Validate file
    $validation = validateFile($file);
    if (!$validation['valid']) {
        return ['success' => false, 'message' => $validation['error']];
    }

    // Create directory structure
    $taskDir = UPLOAD_DIR . $taskname . '/';
    $subDir = $taskDir . $subnum . '/';

    if (!createDirectories($subDir)) {
        return ['success' => false, 'message' => 'Failed to create upload directories'];
    }

    // Generate unique filename if file exists
    $filename = basename($file['name']);
    $filename = ensureUniqueFilename($filename, $subDir);
    $targetPath = $subDir . $filename;

    // Move uploaded file
    if (move_uploaded_file($file['tmp_name'], $targetPath)) {
        chmod($targetPath, 0644);
        debugLog("File uploaded successfully: $targetPath");
        return [
            'success' => true,
            'message' => "File uploaded successfully as: $filename"
        ];
    } else {
        return ['success' => false, 'message' => 'Failed to move uploaded file'];
    }
}

/**
 * Validate uploaded file
 */
function validateFile($file) {
    // Check file size
    if ($file['size'] > MAX_FILE_SIZE) {
        return [
            'valid' => false,
            'error' => 'File too large. Maximum size: ' . formatBytes(MAX_FILE_SIZE)
        ];
    }

    if ($file['size'] == 0) {
        return ['valid' => false, 'error' => 'File is empty'];
    }

    // Check file extension
    $extension = strtolower(pathinfo($file['name'], PATHINFO_EXTENSION));
    if (!in_array($extension, ALLOWED_EXTENSIONS)) {
        return [
            'valid' => false,
            'error' => 'Invalid file type. Allowed: ' . implode(', ', ALLOWED_EXTENSIONS)
        ];
    }

    return ['valid' => true];
}

/**
 * Create directory structure
 */
function createDirectories($path) {
    if (!file_exists($path)) {
        return mkdir($path, 0755, true);
    }
    return true;
}

/**
 * Ensure filename is unique by adding number if needed
 */
function ensureUniqueFilename($filename, $dir) {
    $originalName = pathinfo($filename, PATHINFO_FILENAME);
    $extension = pathinfo($filename, PATHINFO_EXTENSION);
    $counter = 1;
    $newFilename = $filename;

    while (file_exists($dir . $newFilename)) {
        $newFilename = $originalName . '_' . $counter . '.' . $extension;
        $counter++;
    }

    return $newFilename;
}

/**
 * Sanitize filename/directory name
 */
function sanitizeFilename($name) {
    // Remove any character that's not alphanumeric, dash, underscore, or dot
    $name = preg_replace('/[^a-zA-Z0-9\-_\.]/', '_', $name);
    // Remove leading/trailing dots and underscores
    $name = trim($name, '._');
    // Limit length
    return substr($name, 0, 100);
}

/**
 * Format bytes to human-readable size
 */
function formatBytes($bytes) {
    $units = ['B', 'KB', 'MB', 'GB'];
    $i = 0;
    while ($bytes >= 1024 && $i < count($units) - 1) {
        $bytes /= 1024;
        $i++;
    }
    return round($bytes, 2) . ' ' . $units[$i];
}

/**
 * Debug logging
 */
function debugLog($message) {
    if (DEBUG_MODE) {
        error_log("[PEBL Upload] " . $message);
    }
}

/**
 * Send response and exit
 */
function respondAndExit($message, $success = true) {
    $status = $success ? "SUCCESS" : "ERROR";
    echo "$status: $message\n";
    debugLog("$status: $message");
    exit($success ? 0 : 1);
}

?>
