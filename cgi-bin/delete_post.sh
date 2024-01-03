#!/bin/bash
formatted_date=$(date -u +"Date: %a, %d %b %Y %H:%M:%S GMT")
echo "HTTP/1.1 202 OK $(formatted_date)
Content-Length: 1758
Content-Location: /dog
Content-Type: text/html"