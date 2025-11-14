# PEBL Simple Data Server

A minimal, single-admin PHP data upload server for PEBL (Psychology Experiment Building Language) experiments.

## Features

- **Simple Setup** - No database required, runs on any PHP 5.5+ server
- **Single Admin** - One set of credentials in a config file
- **Secure** - Password hashing, file validation, protected config files
- **Compatible** - Works with PEBL's built-in upload functions
- **cPanel Friendly** - Easy to deploy on shared hosting

## Requirements

- PHP 5.5 or higher
- Apache with .htaccess support (or nginx with equivalent rules)
- Write permissions for uploads directory

## Quick Start

### 1. Upload Files

Upload all files to your web server (e.g., via FTP, cPanel File Manager, etc.):

```
your-domain.com/pebl-data/
├── config.php
├── upload.php
├── getnewsubnum.php
├── generate_password.php
├── .htaccess
├── counter.txt (created automatically)
└── uploads/ (created automatically)
```

### 2. Set Permissions

Ensure the web server can write to these locations:

```bash
chmod 755 uploads/
chmod 644 counter.txt   # Will be created automatically if doesn't exist
chmod 644 config.php
chmod 644 upload.php
chmod 644 getnewsubnum.php
```

**In cPanel:**
- Right-click folder → Change Permissions → 755 for directories
- Right-click files → Change Permissions → 644 for files

### 3. Change Default Password

**IMPORTANT:** The default password is `admin123` - **change it immediately!**

#### Option A: Generate via Command Line
```bash
php generate_password.php yourNewPassword
```

#### Option B: Generate via Web
Run the password generator through your browser (temporarily):
```
http://your-domain.com/pebl-data/generate_password.php
```
Then copy the hash to `config.php`

#### Option C: Generate via PHP Interactive
```bash
php -r "echo password_hash('yourNewPassword', PASSWORD_DEFAULT);"
```

Edit `config.php` and replace:
```php
define('ADMIN_PASSWORD_HASH', 'your-new-hash-here');
```

### 4. Test the Server

#### Test Subject Number Generation:
```bash
curl "http://your-domain.com/pebl-data/getnewsubnum.php?user_name=admin&upload_password=yourpassword"
```

Should return: `1` (or next number in sequence)

#### Test File Upload:
Create a test file `test.csv` with some content, then:
```bash
curl -X POST \
  -F "user_name=admin" \
  -F "upload_password=yourpassword" \
  -F "taskname=test" \
  -F "subnum=001" \
  -F "fileToUpload=@test.csv" \
  http://your-domain.com/pebl-data/upload.php
```

Should return: `SUCCESS: File uploaded successfully as: test.csv`

### 5. Configure PEBL

Create or update your PEBL experiment's `upload.json` file:

```json
{
    "server": "your-domain.com",
    "page": "/pebl-data/upload.php",
    "port": "80",
    "username": "admin",
    "upload_password": "yourpassword",
    "taskname": "stroop"
}
```

**For HTTPS:**
```json
{
    "server": "your-domain.com",
    "page": "/pebl-data/upload.php",
    "port": "443",
    "username": "admin",
    "upload_password": "yourpassword",
    "taskname": "stroop"
}
```

### 6. Use in PEBL Script

#### Upload Files:
```pebl
# Configure via --upload argument when running PEBL
# OR use UploadFile() in script:
UploadFile(gSubNum, dataFileName)
```

#### Get Subject Number:
```pebl
gSubNum <- GetNewSubNum("your-domain.com", "/pebl-data/getnewsubnum.php", "80", "admin", "yourpassword")
Print("Subject number: " + gSubNum)
```

## File Structure

```
pebl-data/
├── config.php              # Configuration (credentials, settings)
├── upload.php              # File upload handler
├── getnewsubnum.php        # Subject number generator
├── generate_password.php   # Password hash generator utility
├── .htaccess               # Apache security rules
├── counter.txt             # Subject number counter (auto-created)
├── uploads/                # Data storage (auto-created)
│   ├── stroop/             # Task-specific directory
│   │   ├── 001/            # Subject-specific directory
│   │   │   ├── data.csv
│   │   │   └── data_1.csv  # Auto-numbered if duplicate
│   │   └── 002/
│   └── ant/
│       └── 001/
└── README.md               # This file
```

## Configuration

Edit `config.php` to customize:

```php
// Admin Credentials
define('ADMIN_USERNAME', 'admin');
define('ADMIN_PASSWORD_HASH', 'your-hash-here');

// Upload Limits
define('MAX_FILE_SIZE', 10485760); // 10MB
define('ALLOWED_EXTENSIONS', ['txt', 'csv', 'tsv', 'dat', 'log']);

// Timezone
date_default_timezone_set('America/New_York');

// Debug Mode (set to true for troubleshooting)
define('DEBUG_MODE', false);
```

## Security Best Practices

1. **Change default password immediately**
2. **Use HTTPS** - Configure SSL certificate on your server
3. **Restrict directory listing** - Already configured in .htaccess
4. **Set strong password** - Use 12+ characters, mix of types
5. **Monitor uploads directory** - Check periodically for unusual files
6. **Keep PHP updated** - Use PHP 7.4+ for better security
7. **Limit access** - Use IP restrictions if possible:

```apache
# In .htaccess, restrict to specific IPs:
<Files "upload.php">
    Require ip 192.168.1.0/24
    # Or specific IPs:
    # Require ip 1.2.3.4 5.6.7.8
</Files>
```

## Troubleshooting

### Problem: "Authentication failed"
- **Check:** Username and password in upload.json
- **Check:** Password hash in config.php matches your password
- **Test:** Regenerate password hash with generate_password.php

### Problem: "No file uploaded"
- **Check:** File upload settings in php.ini
- **Check:** File size limits (upload_max_filesize, post_max_size)
- **Try:** Increase limits in .htaccess or contact hosting provider

### Problem: "Failed to create upload directories"
- **Check:** Permissions on uploads/ directory (should be 755)
- **Check:** Parent directory permissions
- **Try:** Create uploads/ directory manually via cPanel

### Problem: "Permission denied" on counter.txt
- **Check:** counter.txt permissions (should be 644)
- **Check:** Parent directory is writable
- **Try:** Delete counter.txt and let it be recreated

### Problem: Subject numbers not incrementing
- **Check:** counter.txt file exists and is writable
- **Check:** File locking is working (flock)
- **Enable DEBUG_MODE** in config.php and check error logs

### Problem: Files not uploading
- **Check:** uploads/ directory exists and is writable
- **Check:** File extension is in ALLOWED_EXTENSIONS
- **Check:** File size is under MAX_FILE_SIZE
- **Check:** php.ini upload_max_filesize and post_max_size

### Enable Debug Mode

Edit config.php:
```php
define('DEBUG_MODE', true);
```

Then check your PHP error logs (usually in cPanel → Error Log)

## PHP Version Check

Check your PHP version:
```bash
php -v
```

Or create a file `phpinfo.php`:
```php
<?php phpinfo(); ?>
```

Visit: `http://your-domain.com/pebl-data/phpinfo.php`

**Delete phpinfo.php after checking!**

## Accessing Your Data

### Via FTP/SFTP
Connect to your server and navigate to `uploads/` directory

### Via cPanel File Manager
Navigate to: `public_html/pebl-data/uploads/`

### Via Command Line (SSH)
```bash
cd /path/to/pebl-data/uploads/
ls -R
```

### Download All Data
```bash
# Via command line:
cd /path/to/pebl-data/
tar -czf data-backup-$(date +%Y%m%d).tar.gz uploads/

# Via cPanel:
# Right-click uploads/ → Compress → Create Archive
```

## Advanced Configuration

### Custom Upload Directory Structure

You can modify upload.php to change the directory structure. Current structure:
```
uploads/taskname/subnum/file.csv
```

### Multiple Admins (Simple Approach)

For multiple admins without database:

Edit config.php:
```php
define('ADMIN_USERS', [
    'admin1' => '$2y$10$hash1...',
    'admin2' => '$2y$10$hash2...',
    'labuser' => '$2y$10$hash3...'
]);
```

Then modify authenticate() in upload.php and getnewsubnum.php:
```php
function authenticate() {
    $username = $_POST['user_name'] ?? $_GET['user_name'] ?? '';
    $password = $_POST['upload_password'] ?? $_GET['upload_password'] ?? '';

    $users = ADMIN_USERS;
    if (isset($users[$username])) {
        return password_verify($password, $users[$username]);
    }
    return false;
}
```

### Nginx Configuration

If using Nginx instead of Apache, add to your server block:

```nginx
location ~ /(config\.php|counter\.txt|generate_password\.php|.*\.md)$ {
    deny all;
}

location /pebl-data/uploads/ {
    location ~ \.php$ {
        deny all;
    }
}
```

## Upgrading from Multi-User Server

If migrating from the full PEBL Data Server:

1. **Export existing data** from uploads directory
2. **Flatten directory structure** if needed:
   ```bash
   # Move from username/task/sub/ to task/sub/
   ```
3. **Create single admin account**
4. **Update all upload.json files** with new credentials
5. **Test thoroughly** before going live

## Support

For issues with:
- **PEBL software:** https://github.com/stmueller/pebl/issues
- **This server:** Check troubleshooting section above
- **Hosting issues:** Contact your hosting provider

## License

This PEBL Simple Data Server is part of the PEBL project.

## Version

Version: 1.0
Last Updated: 2025-01-13
Compatible with: PEBL 2.0+
