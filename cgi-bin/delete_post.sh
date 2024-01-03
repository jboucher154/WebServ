#!/bin/bash

id=$1
echo "QUERY_STRING: $QUERY_STRING" >&2
echo "PATH_TRANSLATED: $PATH_TRANSLATED" >&2
echo "Extracted ID: $id" >&2

echo "Status: 200 OK"
echo "Content-Type: application/json"
echo
echo '{"status": "success"}'
# formatted_date=$(date -u +"Date: %a, %d %b %Y %H:%M:%S GMT")
# echo "HTTP/1.1 202 OK $(formatted_date)
# Content-Length: 1758
# Content-Location: /dog
# Content-Type: text/html"