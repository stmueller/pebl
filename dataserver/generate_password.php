<?php
/**
 * PEBL Simple Data Server - Password Hash Generator
 *
 * Run this script to generate a password hash for config.php
 *
 * Usage:
 *   php generate_password.php yourpassword
 *
 * Or run interactively:
 *   php generate_password.php
 *
 * Copy the generated hash to ADMIN_PASSWORD_HASH in config.php
 */

// Check if password provided as argument
if (isset($argv[1])) {
    $password = $argv[1];
} else {
    // Interactive mode
    echo "PEBL Data Server - Password Hash Generator\n";
    echo "==========================================\n\n";
    echo "Enter password: ";

    // Hide input on Unix-like systems
    if (strtoupper(substr(PHP_OS, 0, 3)) !== 'WIN') {
        system('stty -echo');
        $password = trim(fgets(STDIN));
        system('stty echo');
        echo "\n";
    } else {
        // Windows doesn't hide input easily
        $password = trim(fgets(STDIN));
    }
}

if (empty($password)) {
    echo "Error: Password cannot be empty\n";
    exit(1);
}

// Generate hash
$hash = password_hash($password, PASSWORD_DEFAULT);

// Display result
echo "\n";
echo "Password Hash Generated:\n";
echo "========================\n";
echo $hash . "\n\n";

echo "Copy this hash to config.php:\n";
echo "-----------------------------\n";
echo "define('ADMIN_PASSWORD_HASH', '$hash');\n\n";

echo "IMPORTANT: Keep this password secure and don't share the hash publicly!\n";

?>
