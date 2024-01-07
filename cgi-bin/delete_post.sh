

#!/bin/bash

query_string="${QUERY_STRING}"
echo "QUERY_STRING: $QUERY_STRING" >&2

query_string="QUERY_STRING: $QUERY_STRING"

decoded_text=$(echo -e "${query_string//%/\\x}")

extract_value() {
    key=$1
    json_string=$2
    value=$(awk -F"[,:] *" -v key="$key" '{for(i=1;i<NF;i++)if($i~key){gsub(/^[ \t,]+|[ \t,]+$/, "", $(i+1)); gsub(/["}]$/, "", $(i+1)); print $(i+1)}}' <<< "$json_string")
    echo "$value"
}

id=$(extract_value "id" "$decoded_text")
imgSrc=$(extract_value "imgSrc" "$decoded_text")

decoded_text="Decoded QUERY_STRING: $decoded_text"
extracted_id="Extracted ID: $id"
extracted_imgSrc="Extracted imgSrc: $imgSrc"

echo "$extracted_id" >&2
echo "$extracted_imgSrc" >&2

echo "Status: 200 OK"
echo '{"status": "success"}'

# echo "Status: 200 OK"
# echo '{"status": "success"}'
# formatted_date=$(date -u +"Date: %a, %d %b %Y %H:%M:%S GMT")
# echo "HTTP/1.1 202 OK $(formatted_date)
# Content-Length: 1758
# Content-Location: /dog
# Content-Type: text/html"


