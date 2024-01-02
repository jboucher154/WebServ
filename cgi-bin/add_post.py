import argparse
import sys
import os
import re
from urllib.parse import unquote
from email import message_from_string


def extract_value(name, msg):
    # Find the first occurrence of the specified name in the form data
    content = msg.get_payload()
    pattern = re.compile(fr'name="{name}"\s+value=(\S+)', re.IGNORECASE)
    match = pattern.search(content)

    if match:
        # Extract the matched value
        value = match.group(1)
        # URL decode the value
        return unquote(value)
    else:
        return None

## Check if stdin is provided
   # if not sys.stdin.isatty():
        # Read input strings from stdin
     #   input_strings = [s.strip() for s in sys.stdin.readlines()]
def main():
    path_translated = os.environ.get('PATH_TRANSLATED')
    query_string = os.environ.get("QUERY_STRING")
    decoded_string = unquote(query_string)
    msg = message_from_string(decoded_string)
    name = extract_value("name", msg)
    pet_type = extract_value("petType", msg)
    age = extract_value("age", msg)
    parents = extract_value("parents", msg)
    about = extract_value("about", msg)
    file_path = "website/dog/index.html" #the path has to be obtained from path translated + the query string,
    indicator = "<!-- last post ended here -->"
    to_be_added_content =  f"""
        <div class="container default post">
            <aside class="image">
                <img src="Elvis.jpeg" alt="test png" alt="cute doggo photo"/>
            </aside>
            <div class="description">
                <div class="name">doggo's name: {name}</div>
                <div class="name">doggo's age: {age}</div>
                <div class="name">doggo's parent: {parents}</div>
                <article>
                    <h4>about doggo</h4>
                    <p>{about}</p>
                </article>
                <input id="commentInput" class="comment_box comment_text" placeholder="Write a temporary comment here"></input>
                <div>
                    <button type="button" class="comment_button" onclick="addComments()">Comment</button>
                </div>
            </div>
        </div>
    """
    try:
        with open(file_path, 'r') as file:
            file_content = file.read()
            index = file_content.find(indicator)
        if index != -1:
                new_content = file_content[:index] + to_be_added_content + "    "+ indicator + file_content[index + len(indicator):]
                with open(file_path, 'w') as write_file:
                    write_file.write(new_content)
        sys.stdout.write(new_content)
    except Exception as e:
         print(f'Error processing {file_path}: {e}')
if __name__ == '__main__':
    main()
#how about generating an error page if open/read/write is unsuccessful?
