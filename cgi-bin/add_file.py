# import argparse
# import sys
# import os

# def main():
#     # parser = argparse.ArgumentParser(description='A simple Python script with arguments.')
#     # parser.add_argument('input_strings', nargs='*', help='Array of input strings')
#     # parser.add_argument('--output', '-o', help='Path to the output file (optional)')
#     # parser.add_argument('--verbose', '-v', action='store_true', help='Enable verbose mode')

#     # args = parser.parse_args()

#     # Check if stdin is provided
#     # if not sys.stdin.isatty():
#     #     # Read input strings from stdin
#     #     input_strings = [s.strip() for s in sys.stdin.readlines()]
#     # else:
#     #     input_strings = args.input_strings

#     # print('Command-line arguments:', args.input_strings)
#     # print('Input strings from stdin:', input_strings)
#     path_translated = os.environ.get('PATH_TRANSLATED')
#     html_content =  "<html>\n\t<head>\n\t</head>\n\t<body>\n\t\t<ul>\n\t\t\t<li>" + path_translated + "</li>"
#     query_string = os.environ.get("QUERY_STRING")
#     html_content += "<li>" + query_string + "</li>" + "\n\t\t</ul>\n\t</body>\n</html>"
#     # if args.output:
#     #     print('Output file:', args.output)

#     # if args.verbose:
#     #     print('Verbose mode enabled')
#     #     # Additional verbose information
#     #     print('Performing some extra operations...')
#     sys.stdout.write(html_content)


import sys

def save_binary_to_file(file_path, output_file_path):
    try:
        with open(file_path, 'rb') as input_file:
            # Read binary data from the input file
            binary_data = input_file.read()

            # Write binary data to the output file
            with open(output_file_path, 'wb') as output_file:
                output_file.write(binary_data)

        # print(f'Successfully saved binary data to {output_file_path}')
    except Exception as e:
        print(f'Error saving binary data: {e}')

# # Example usage
# input_file_path = 'input.bin'
# output_file_path = 'output.bin'
# save_binary_to_file(input_file_path, output_file_path)

if __name__ == '__main__':
   save_binary_to_file()