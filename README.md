# WebServ: JAS Server

A simple HTTP server written in C++

This project introduced us to the wonderful world of HTTP, RFC, server configurations, CGI, and TCP socket mangement. 
This simple server provides GET, HEAD, POST, and DELETE methods and provides common gateway interface (CGI) for scripted/ interpreted languages such as python and bash. It was developed to specifically work with a Chrome client and run on Mac OS 10.14.6.

## Collaborators

Team JAS!

[Jenny Boucher](https://github.com/jboucher154)

[Azär Sarikhani](https://github.com/azarSarikhani)

[Sakari Salmi](https://github.com/sakarisalmi)

## Project Coding Standards

Coding standards set for the project can be found [here](project_standards/standards.md)

The project began with a restiction to use only C++98 standard. The restriction was lifted towards the end of the project, and although the JAS Team did not complete any major refactoring, a few modern methods and objects can be found throughout the project.

## Technology

- C++
- Chrome (current)
- Mac OS 10.14.6
- Bash
- Python

## Features

### General

- Serves fully static websites
- Follows HTTP 1.1, though not fully compliant
- Server settings can be confirgured in header file [Webserv.hpp](include/WebServ.hpp)

The program can be switched between using poll() or select() in I/O multiplexing of the file descriptors (server and client sockets, pipe file descriptors). Highly recommended to use poll(). This is the default, but it can be changed by modifying the WebServ.hpp `POLL_TRUE_SELECT_FALSE` macro to false.

### Logger

The JAS Server provides a Logger that prints log messages in ERROR, INFO, and DEBUG categories.

Settings for the Logger can be configured in [Logger.hpp](include/Logger.hpp). 

The logging of DEBUG messages can be configured from [Webserv.hpp](include/WebServ.hpp) with the `GET_DEBUG_LOG` macro. 
- There is an additional `GET_SELECT_POLL_LOOP_FD_INFO` macro can be turned true for additional debug messages.

The `LOG_LOCATION` macro settings in Logger.hpp include:
- logging to files only
- loging to console only
- logging to both

### Configuration File

Multiple servers can be configured in a `.conf` file and examples can be found in the directory [config](config) of this project.

- Each block must be enclosed in curley braces `{}`.
- Each  line in a `main` or `location` block must end with a `;`.
- Comments can be made in the config file using a leading `#`.

#### Basic Requirements

The config file requires a `server` block to contain at least a `main` block and a `location` block for the root (`/`). 

The `main` block must have the following directives designated:
- listen: the port the server will listen on
- server_name: name of the server (Note: If ports are shared, the server_name must be unique.)
- host: the IP of the server
- root: the path to the root of the served content (will be used for locations without given roots)
- client_max_body_size: largest size of body in request that the server will accept. Limited to size of double.

The `main` block may have error pages designated with this format
- error_page_404 followed/by/path/to/error_page.html;

If nothing is designated for a `location` block, the following defaults will be set:
- index: will be set to index.html for the default file
- allow_methods: default will be no methods allowed
- autoindex: default will be `off`, in which case no directory listing will be shown and the client will be directed to the default file set in the index directive
- root: will use the root from the `main` block if none given


The following are optional `location` directives that will not have a default set:
- return: set to the name of another location for a redirection to be sent to client
- alias: set to name of location for server to refence for resources, other directive set here will override directive in the other location.
- save_dir: filepath for a directory to save files from POST requests

At least one directive needs to be set for a `location` to be valid.

Special location `/cgi-bin` can be designated to configure CGI script. 
- This location must have this name (/cgi-bin). 
- There are unique directives to be set for this location:
	- script: list of scripts existing in the /cgi-bin location
	- cgi_path: path to the interpreter/ executor for the scripts in same order as the extensions (below) e.g., /usr/local/bin/python3 /bin/bash
	- cgi_ext: extensions for CGI scripts in the same order as the paths e.g., .py .sh
	- The extension `.cgi` may be included, but must come last on the list. If included it will allow any executable without extension to be run.

### HTTP Status Codes

HTTP Status Codes are designated by the RFC and used to communicate specific messages of Information (1XX), Success (2XX), Redirection (3XX), Client Errors (4XX), and Server Error (5XX).

### HTTP Request/ Response

General notes on request and response handling by the JAS Server:
- The server honours reactive negotiation request/ response pattern
- Chunked requests will be unchunked. Once all the chunks are received, the request will be processed and a response made
- Currently Responses are not sent as chunked.
- If not all parts of a request have not been recieved in the REQUEST_TIMEOUT_SEC (default set to 10), a timeout response will be sent to client and the connection closed.

### HTTP Methods

The JAS Server provides GET, HEAD, POST, and DELETE methods. Any other method will return a `501 NOT_IMPLEMENTED` response.

#### GET

GET method is safe and idempotent. If GET method is allowed by the location for which the request URI is requesting the resource from, the resource will be returned in the body of the response. This of course depends on the resource being found and accessable.

Due to project limitations, the size of media that is able to be returned may be limited depending on the system that is running it, especially if the resource is larger than 5MB. In the case of an incomplete response sent, an Error will be added to the server log output.

#### HEAD

HEAD method is safe and idempotent. The response will indicate if the method is allowed and all resources are available.

#### POST

POST method is not safe and not idempotent. The JAS server will only handle POST requests that are submitted with a form, either multipart/form-data or a default form type.

In the case of a POST request for a file, the JAS Server is capable of creating the resource without a CGI script. The resource will be created in the location given in the `action` option of the html form. Otherwise, after verification of the validitiy of the request, the POST handling will be passed to the CGI Handler.

#### DELETE

DELETE method is not safe, but is idempotent. The JAS server will verify the request and delete the resource, similar to a command line use of `rm` if the method is allowed, etc. It is possible to pass the handling of the DELETE method to a CGI script if the user desires more fine-grained control of the verification and handling of the request.

### CGI

The Common Gateway Interface provides users of the JAS Server the ability to create srcipts to modify and create content to be returned to the client.

#### Limitations

Due to system limitations for the project, data sent to and responses returned from CGI scripts are limited to 60KB.
In cases where a larger file is sent by POST to a cgi script the server will make a temporary file. The path to this fill can be parsed as a key=value pair from the QUERY_STRING in the the environment passed to the script. The key will be `temp_filepath`. The temporary file will be deleted once the response is sent to the client.

#### Input to CGI

Input to CGI scripts is handled in two ways.

First, the METAVARIABLES are prepared and passed as the environment to the scripts when they are run. Any data from the form, other than file contents, will be stored in the environment QUERY_STRING and be url encoded.

Second, if there is a file it will either be saved a temporary file (as described above), or sent using a pipe and written to the STDOUT. The script must read the data from the STDIN.

#### Script Output Expectations

Script output will be read by the server from the STDIN so the script must write output to the STDOUT.

The output should be a properly formated HTTP response, including response line and any headers required. If there is no error on the server side, the script is 100% responsible for the response and what is returned from it will be sent to the client directly.

### Cookies

Currently there is no support for cookies.

## How To Use

The project can be compiled with `make` and ran `./webserv config_file_here`.

If no config file is given the default designated in the `Webserv.hpp` will be used.

- The config file can be custom and need not be one of the provided examples as long as if follows the guidelines for what is required.

## Robusticity

Siege was used to stress test the server. We achieved 99.8% availability on our local environment.

### To run this test

If you don't have siege installed first: `brew install siege`

Run the server with the siege configuration: `./webserv ./config/good/siege.conf`

Then, in a separate terminal run: `siege -t(amount of minutes) http://localhost:8080`

If you run this with a different configuration change the hostname and port to the appropriate configuration

