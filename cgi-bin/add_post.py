import argparse
import sys
import os
   
## Check if stdin is provided
   # if not sys.stdin.isatty():
        # Read input strings from stdin
     #   input_strings = [s.strip() for s in sys.stdin.readlines()]
def main():
    path_translated = os.environ.get('PATH_TRANSLATED')
    query_string = os.environ.get("QUERY_STRING")
    file_path = "website/dog/index.html" #the path has to be obtained from path translated + the query string,
    indicator = "<!-- last post ended here -->"
    to_be_added_content =  """
            <div class="container default post">
                <aside class="image">
                    <img src="../dog/Elvis.jpeg" alt="test png" alt="cute doggo photo"/>
                </aside>
                <div class="description">
                    <div class="name">doggo's name: Elvis</div>
                    <div class="name">doggo's age: 1 year 8 months</div>
                    <div class="name">doggo's parent: mama Azzar and mama Sara</div>
                    <article>
                        <h4>about doggo</h4>
                        <p>Very very noice doggo.</p>
                    </article>
                    <input id="commentInput" class="comment_box comment_text" placeholder="Write a temporary comment here"></input>
                    <div>
                        <button type="button" class="comment_button" onclick="addComments()">Comment</button>
                    </div>
                </div>
            </div>
    """
    # sys.stdout.write(to_be_added_content)
    try:
        with open(file_path, 'r') as file:
            file_content = file.read()
            index = file_content.find(indicator)
        if index != -1:
                new_content = file_content[:index] + to_be_added_content + indicator + file_content[index + len(indicator):]
                with open(file_path, 'w') as write_file:
                    write_file.write(new_content)
        sys.stdout.write(new_content)
    except Exception as e:
         print(f'Error processing {file_path}: {e}')
if __name__ == '__main__':
    main()
#how about generating an error page if open/read/write is unsuccessful?
