import argparse
import sys

def main():
    parser = argparse.ArgumentParser(description='A simple Python script with arguments.')
    parser.add_argument('input_strings', nargs='*', help='Array of input strings')
    parser.add_argument('--output', '-o', help='Path to the output file (optional)')
    parser.add_argument('--verbose', '-v', action='store_true', help='Enable verbose mode')

    args = parser.parse_args()

    # Check if stdin is provided
    if not sys.stdin.isatty():
        # Read input strings from stdin
        input_strings = [s.strip() for s in sys.stdin.readlines()]
    else:
        input_strings = args.input_strings

    print('Command-line arguments:', args.input_strings)
    print('Input strings from stdin:', input_strings)

    if args.output:
        print('Output file:', args.output)

    if args.verbose:
        print('Verbose mode enabled')
        # Additional verbose information
        print('Performing some extra operations...')

if __name__ == '__main__':
    main()



