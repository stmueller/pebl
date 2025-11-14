<?php
/**
 * PEBL Simple Data Server - Subject Number Generator
 *
 * Generates new unique subject/participant numbers.
 * Uses file-based counter with locking for concurrent access safety.
 *
 * Usage from PEBL with GetNewSubNum():
 * gSubNum <- GetNewSubNum(server, page, port, username, password)
 *
 * HTTP GET request:
 * getnewsubnum.php?user_name=admin&upload_password=yourpassword
 */

require_once(__DIR__ . '/config.php');

// Main execution
try {
    // Authenticate user
    if (!authenticate()) {
        // Return random number on auth failure (PEBL fallback behavior)
        echo rand(1000000, 9999999);
        debugLog("Auth failed, returned random number");
        exit(1);
    }

    debugLog("Authentication successful for subject number request");

    // Get and increment counter
    $subjectNumber = getNextSubjectNumber();

    if ($subjectNumber !== false) {
        echo $subjectNumber;
        debugLog("Issued subject number: $subjectNumber");
        exit(0);
    } else {
        // Fallback to random if counter fails
        $random = rand(1000000, 9999999);
        echo $random;
        debugLog("Counter failed, returned random: $random");
        exit(1);
    }

} catch (Exception $e) {
    // Fallback to random on any error
    echo rand(1000000, 9999999);
    debugLog("Exception: " . $e->getMessage());
    exit(1);
}

/**
 * Authenticate user with username and password
 */
function authenticate() {
    $username = $_GET['user_name'] ?? '';
    $password = $_GET['upload_password'] ?? '';

    debugLog("Subject number auth attempt for: $username");

    if (empty($username) || empty($password)) {
        return false;
    }

    if ($username !== ADMIN_USERNAME) {
        return false;
    }

    if (!password_verify($password, ADMIN_PASSWORD_HASH)) {
        return false;
    }

    return true;
}

/**
 * Get next subject number with file locking for concurrent access safety
 */
function getNextSubjectNumber() {
    $counterFile = COUNTER_FILE;

    // Ensure counter file exists
    if (!file_exists($counterFile)) {
        file_put_contents($counterFile, '0');
        chmod($counterFile, 0644);
    }

    // Open file for reading and writing
    $fp = fopen($counterFile, 'c+');
    if (!$fp) {
        debugLog("Failed to open counter file");
        return false;
    }

    // Acquire exclusive lock
    if (!flock($fp, LOCK_EX)) {
        fclose($fp);
        debugLog("Failed to acquire file lock");
        return false;
    }

    // Read current counter
    $currentCounter = (int)fread($fp, filesize($counterFile) ?: 1);

    // Increment
    $newCounter = $currentCounter + 1;

    // Write new value
    ftruncate($fp, 0);
    rewind($fp);
    fwrite($fp, (string)$newCounter);
    fflush($fp);

    // Release lock and close
    flock($fp, LOCK_UN);
    fclose($fp);

    debugLog("Counter incremented: $currentCounter -> $newCounter");

    return $newCounter;
}

/**
 * Debug logging
 */
function debugLog($message) {
    if (DEBUG_MODE) {
        error_log("[PEBL SubNum] " . $message);
    }
}

?>
