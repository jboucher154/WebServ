
import sys

def save_binary_to_file():
    output_file_path = "../website/new_bin_file.jpg"
    chunk_size = 4096
    try:
        binary_data = b''
        chunk = b'...'
        while True:
            chunk = sys.stdin.buffer.read(chunk_size)
            if not chunk:
                break
            binary_data += chunk

            # Write binary data to the output file
            with open(output_file_path, 'wb') as output_file:
                output_file.write(binary_data)

        # print(f'Successfully saved binary data to {output_file_path}')
    except Exception as e:
        print(f'Error saving binary data: {e}')

if __name__ == '__main__':
   save_binary_to_file()