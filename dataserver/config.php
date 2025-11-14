<?php
/**
 * PEBL Simple Data Server - Configuration
 *
 * This is a simplified single-admin PEBL data upload server.
 * Edit these values for your setup.
 *
 * IMPORTANT: Change the default password after setup!
 */

// Admin Credentials
// Default username: admin
// Default password: admin123
// CHANGE THIS PASSWORD IMMEDIATELY!
define('ADMIN_USERNAME', 'admin');
define('ADMIN_PASSWORD_HASH', '$2y$10$/Yn2yJTbEOwA2D7lOZ1yGO8pqz8p5uDnSjoftHalOGhnc/ZPlmKPG');

// Directory Settings
define('UPLOAD_DIR', __DIR__ . '/uploads/');
define('COUNTER_FILE', __DIR__ . '/counter.txt');

// Upload Limits
define('MAX_FILE_SIZE', 10485760); // 10MB in bytes
define('ALLOWED_EXTENSIONS', ['txt', 'csv', 'tsv', 'dat', 'log']);

// Timezone
date_default_timezone_set('UTC');

// Error Reporting (set to false in production)
define('DEBUG_MODE', false);

if (DEBUG_MODE) {
    error_reporting(E_ALL);
    ini_set('display_errors', '1');
} else {
    error_reporting(0);
    ini_set('display_errors', '0');
}

/**
 * Initialize counter file if it doesn't exist
 */
function initializeCounter() {
    if (!file_exists(COUNTER_FILE)) {
        file_put_contents(COUNTER_FILE, '0');
        chmod(COUNTER_FILE, 0644);
    }
}

/**
 * Ensure upload directory exists
 */
function initializeUploadDir() {
    if (!file_exists(UPLOAD_DIR)) {
        mkdir(UPLOAD_DIR, 0755, true);
    }
}

// Auto-initialize on config load
initializeCounter();
initializeUploadDir();

?>
