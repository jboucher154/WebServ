# WebServ

A simple HTTP server written in C++

This project introduced us to the wonderful world of HTTP, RFC, server configurations, CGI, and TCP socket mangement. 
This simple server provides GET, HEAD, POST, and DELETE methods and provides common gateway interface (CGI) for scripted/  interpred languages such as python and bash. It was developed to specifically work with a chrome client and run on Mac OS 10.14.6.

## Collaborators

Team JAS!

[Jenny Boucher](https://github.com/jboucher154)
[Azär Sarikhani](https://github.com/azarSarikhani)
[Sakari Salmi](https://github.com/sakarisalmi)

## Project Coding Standards

Coding standards set for the project can be found [here](project_standards/standards.md)

The project began with guidelines to use C++98 standard. The limit was lifted towoards the end of the project so no major refactoring was done, yet a few methods and objects from standards beyond 98 can be found.

## Technology

- C++
- Chrome (current)
- Mac OS 10.14.6
- Bash
- Python

## Features

### General

- serve fully static websites
- Follows HTTP 1.1, though is not fully compliant

### Configuration File

Multiple servers can be configured in on `.conf` file and examples can be found in the directory [config](config) of this project.

- Each block must be enclosed in curley braces `{}`.
- Each  line in a `main` or `location` block must end with a `;`.
- Comments can be made in the config file using is leading `#`.

#### Basic Requirements

The config file requires a `server` block to contain at least a `main` block and a `location` block for the root (`/`). 

The `main` block must have the following directives designated:
- listen: value is the port the server will listen on
- server_name: name of the server (Note: If ports are shared the server_name must be unique.)
- host: the IP of the server
- root: the path to the root of the served content (will be used for locations without given roots)
- client_max_body_size: largest size of body in request that the server will accept. Limited to size of double.

The `main` block may have error pages designated with this format
- error_page_404 followed/by/path/to/error_page.html;

If nothing is is designated for a `location` block the following defaults will be set:
- index: will be set to index.html for the default file
- 

At least one directive needs to be set for a `location` to be valid.

### HTTP Status Codes

### HTTP Methods

#### GET

#### POST

#### DELETE

### CGI

### Cookies

Currently there is no support for cookies.

## Robusticity

- Stress tests

## How To Use