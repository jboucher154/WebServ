import sys
from datetime import datetime , timezone
from email.utils import formatdate
import os

def save_binary_to_file():
    output_file_path = "../website/new_file.png"
    chunk_size = 4194304
    try:
        binary_data = b''
        chunk = b'...'
        while True:
            chunk = sys.stdin.buffer.read(chunk_size)
            with open(output_file_path, 'wb') as output_file:
                output_file.write(binary_data)
            if not chunk:
                break
            binary_data += chunk

        html_content = "<h2>file created</h2>"
        current_datetime = datetime.now(timezone.utc)
        formatted_date = formatdate(timeval=current_datetime.timestamp(), localtime=False, usegmt=True)
        http_version = os.environ.get('SERVER_PROTOCOL')
        header=f"{http_version} 201 Created\r\nDate: {formatted_date}\r\nContent-Type: text/html\r\nContent-Length: {len(html_content)}\r\n\r\n"
        sys.stdout.write(header + html_content)
    except Exception as e:
        html_content = f"<h2>failed to save the file{e}</h2>"
        current_datetime = datetime.now(timezone.utc)
        formatted_date = formatdate(timeval=current_datetime.timestamp(), localtime=False, usegmt=True)
        http_version = os.environ.get('SERVER_PROTOCOL')
        header=f"{http_version} 424 Created\r\nDate: {formatted_date}\r\nContent-Type: text/html\r\nContent-Length: {len(html_content)}\r\n\r\n"
        sys.stdout.write(header + html_content)

if __name__ == '__main__':
   save_binary_to_file()