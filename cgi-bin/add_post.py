import argparse
import sys
import os
from urllib.parse import unquote
from email import message_from_string


def return_value(key, text):
    key = 'name="{}"'.format(key) 
    text_list = text.split("Content-Disposition:")
    text_item = [item for item in text_list if key in item]
    if len(text_item) > 0 :
        text_item = text_item[0]
        value = text_item.split('value=')[-1]
    else:
        value = ''
    return(value.strip())

## Check if stdin is provided
   # if not sys.stdin.isatty():
        # Read input strings from stdin
     #   input_strings = [s.strip() for s in sys.stdin.readlines()]
def main():
    path_translated = os.environ.get('PATH_TRANSLATED')
    query_string = os.environ.get("QUERY_STRING")
    decoded_string = unquote(query_string)
    name = return_value("name", decoded_string)
    pet_type = return_value("petType", decoded_string)
    age = return_value("age", decoded_string)
    parents = return_value("parents", decoded_string)
    about = return_value("about", decoded_string)
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
