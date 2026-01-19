#!/bin/bash

# Start PEBLDataServer test server
# This script starts the PHP built-in server for testing

cd "$(dirname "$0")/.."

echo "=== Starting PEBLDataServer Test Server ==="
echo ""
echo "Server will start on: http://localhost:8080"
echo "Document root: $(pwd)"
echo ""
echo ""
echo "Press Ctrl+C to stop the server"
echo ""
echo "Starting server..."
echo ""

php -S localhost:8080
