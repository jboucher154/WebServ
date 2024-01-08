

#!/bin/bash

query_string="${QUERY_STRING}"

query_string="QUERY_STRING: $QUERY_STRING"

decoded_text=$(echo -e "${query_string//%/\\x}")

decoded_text="Decoded QUERY_STRING: $decoded_text"


rm -f $QUERY_STRING

# echo "Status: 200 OK"
# echo '{"status": "success"}'

formatted_date=$(date -u +"Date: %a, %d %b %Y %H:%M:%S GMT")
echo "HTTP/1.1 200 OK" 
echo "$formatted_date"
echo "Content-Type: text/html"
echo "Content-Length: 27"
echo 
echo "<h2>file deleted</h2>"



