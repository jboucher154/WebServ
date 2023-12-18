#!/bin/bash

# Print the CGI Content-Type header
echo "Content-Type: text/plain"
echo ""

# Print a blank line to separate headers from the content

# Print the metavariables
echo "Metavariables:"

# Iterate through all the metavariables and print them
while IFS='=' read -r name value; do
    echo "$name=$value"
done < <(env | grep -E '^HTTP_|^QUERY_STRING|^REQUEST_METHOD|^CONTENT_TYPE|^CONTENT_LENGTH|^REMOTE_ADDR')

# End of the script
