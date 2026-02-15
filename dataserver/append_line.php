<?php
/**
 * PEBL Simple Data Server - Line Append Handler
 *
 * Appends a single line of CSV data to a file on the server.
 * If the file doesn't exist yet, writes the header first.
 * Uses file locking (flock) for concurrency safety.
 *
 * Two modes controlled by 'pooled' parameter:
 *   pooled=1 (default): Task-level shared file
 *     Path: uploads/{taskname}/{filename}
 *   pooled=0: Per-participant file
 *     Path: uploads/{taskname}/{subnum}/{filename}
 *
 * POST Parameters:
 *   - user_name: Username for authentication
 *   - upload_password: Password for authentication
 *   - taskname: Test name
 *   - subnum: Participant ID
 *   - filename: Target filename (e.g., "grit-pooled.csv")
 *   - header: CSV header line (written only if file is new)
 *   - line: CSV data line to append
 *   - pooled: "1" for task-level (default), "0" for per-participant
 */

require_once(__DIR__ . '/config.php');

// Main execution
try {
    if (!authenticate()) {
        respondAndExit("Authentication failed", false);
    }

    $result = handleAppend();
    respondAndExit($result['message'], $result['success']);

} catch (Exception $e) {
    respondAndExit("Server error: " . $e->getMessage(), false);
}

function authenticate() {
    $username = $_POST['user_name'] ?? '';
    $password = $_POST['upload_password'] ?? '';

    if (empty($username) || empty($password)) {
        return false;
    }

    if ($username !== ADMIN_USERNAME) {
        return false;
    }

    return password_verify($password, ADMIN_PASSWORD_HASH);
}

function handleAppend() {
    $taskname = sanitizeFilename($_POST['taskname'] ?? '');
    $filename = sanitizeFilename($_POST['target_file'] ?? '');
    $header = $_POST['header'] ?? '';
    $line = $_POST['line'] ?? '';
    $subnum = sanitizeFilename($_POST['subnum'] ?? '');
    $pooled = ($_POST['pooled'] ?? '1') === '1';

    if (empty($taskname) || empty($filename) || empty($line)) {
        return ['success' => false, 'message' => 'Missing required parameters (taskname, filename, line)'];
    }

    if (!$pooled && empty($subnum)) {
        return ['success' => false, 'message' => 'subnum is required for per-participant mode'];
    }

    // Validate extension
    $extension = strtolower(pathinfo($filename, PATHINFO_EXTENSION));
    if (!in_array($extension, ALLOWED_EXTENSIONS)) {
        return ['success' => false, 'message' => 'Invalid file extension'];
    }

    // Reject newlines in data
    if (strpos($line, "\n") !== false || strpos($line, "\r") !== false) {
        return ['success' => false, 'message' => 'Line must not contain newlines'];
    }

    if (!empty($header) && (strpos($header, "\n") !== false || strpos($header, "\r") !== false)) {
        return ['success' => false, 'message' => 'Header must not contain newlines'];
    }

    // Build target directory based on mode
    $taskDir = UPLOAD_DIR . $taskname . '/';
    if (!file_exists($taskDir)) {
        if (!mkdir($taskDir, 0755, true)) {
            return ['success' => false, 'message' => 'Failed to create task directory'];
        }
    }

    if ($pooled) {
        // Task-level: uploads/{taskname}/{filename}
        $targetDir = $taskDir;
    } else {
        // Per-participant: uploads/{taskname}/{subnum}/{filename}
        $subDir = $taskDir . $subnum . '/';
        if (!file_exists($subDir)) {
            if (!mkdir($subDir, 0755, true)) {
                return ['success' => false, 'message' => 'Failed to create participant directory'];
            }
        }
        $targetDir = $subDir;
    }

    $targetPath = $targetDir . $filename;
    $isNew = !file_exists($targetPath);

    $fh = fopen($targetPath, 'a');
    if (!$fh) {
        return ['success' => false, 'message' => 'Could not open file for writing'];
    }

    if (flock($fh, LOCK_EX)) {
        // Write header if file is new/empty
        if ($isNew && !empty($header)) {
            $stat = fstat($fh);
            if ($stat['size'] == 0) {
                fwrite($fh, $header . "\n");
            }
        }

        fwrite($fh, $line . "\n");
        fflush($fh);
        flock($fh, LOCK_UN);
        fclose($fh);

        chmod($targetPath, 0644);

        $mode = $pooled ? 'pooled' : 'participant';
        debugLog("Append ($mode): $filename (task=$taskname, sub=$subnum, new=$isNew)");

        return [
            'success' => true,
            'message' => "Line appended to $filename"
        ];
    } else {
        fclose($fh);
        return ['success' => false, 'message' => 'Could not acquire file lock'];
    }
}

function sanitizeFilename($name) {
    $name = basename($name);
    $name = preg_replace('/[^a-zA-Z0-9\-_\.]/', '_', $name);
    $name = trim($name, '._');
    return substr($name, 0, 100);
}

function debugLog($message) {
    if (DEBUG_MODE) {
        error_log("[PEBL Append] " . $message);
    }
}

function respondAndExit($message, $success = true) {
    $status = $success ? "SUCCESS" : "ERROR";
    echo "$status: $message\n";
    exit($success ? 0 : 1);
}
?>
