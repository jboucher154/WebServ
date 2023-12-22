import argparse
import sys
import os

def main():
   
   print("add_file.py initiates running")
   file_path = "newfile"
   path_translated = os.environ.get('PATH_TRANSLATED')
   html_content =  "<html>\n\t<head>\n\t</head>\n\t<body>\n\t\t<ul>\n\t\t\t<li>" + path_translated + "</li>"
   query_string = os.environ.get("QUERY_STRING")
   html_content += "<li>" + query_string + "</li>" + "\n\t\t</ul>\n\t</body>\n</html>"
   try:
        with open(file_path, 'w') as file:
            file.write(html_content)
        print(f'Successfully wrote to {file_path}')
   except Exception as e:
        print(f'Error writing to {file_path}: {e}')	

    

if __name__ == '__main__':
    main()