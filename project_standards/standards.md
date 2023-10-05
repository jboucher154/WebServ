
# Coding Standards

## Contents

- [General](#general)
- [Update Log](#update-log)
- [Denomination](#denomination)
- [Formatting](#formatting)
- [Functions](#functions)
- [Typedefs, enum, union](#typedefs-enum-union)
- [Classes](#classes)
- [Headers](#headers)
- [Templates](#templates)
- [Macros and Pre-processors](#macros-and-pre-processors)
- [Comments](#comments)
- [Files](#files)
- [Makefile](#makefile)

## General

- The project must comply with the C++ 98 standard. Then, it should still compile if you add the flag -std=c++98.

- Try to always develop using the most C++ features you can (for example, choose <cstring> over <string.h>). You are allowed to use C functions, but always prefer their C++ versions if possible.

- The project code must complie with c++ and the flags -Wall -Wextra -Werror

- Any external library and Boost libraries are forbidden.

- This document may be updated and a note will be made about the date of an update in the [Update Log](#update-log).

## Update Log

| __Date__        | __Changes__   |
| -------------  |:-------------:|
|                |  |
|                |       |
|                |     |
|                |     |
|                |    |
|                |       |
|                |       |
|                |     |
|                |       |
<br><br>

## Denomination

Naming convention by type.

- Class names should be UpperCamelCase (aka PascalCase)

- Struct names should be preceeded by a s_ followed by UpperCamelCase name

- A typedef’s name must start by t_

- A union’s name must start by u

- An enum’s name must start by e_

- A global’s name must start by g_

- Function names should be camelCase

- Variables should be snake_case

- Private class attributes and member functions should be followed_by_an_underscore_

- Non-class files and directories names can only contain lowercases, digits and ’_’ (snake_case)

- Class files (`.cpp`, `.hpp`, `.tpp`) should have the same UpperCamelCase name as the class. 
	- If the class definition is split into separate files the name should lead with the class name followed by an underscore and an approriate description of the contentes. Example: ClassName_dates.cpp

## Formatting

- Indentations : Each nested level of code will be indented once from the level above.

- Function Length : As much as possible keep length of functions to a minimum. Follow DRY (dont repeat yourself) and SRP (single responsibility principle) whenever possible.

- Column Width : Keep column width within reason. If you want to break up a line for readablity indent once on the new line.

- Space Between Functions : At least one newline should separate a function, preferably one.

- Space Within Functions : A newline should follow under the function name (see Braces example). Otherwise spaces should only exist to improve readability of the code.

- Empty lines : must be empty

- Order for parameter designations: Const should come first followed by type, followed by reference or pointer symbol. 
	- Example: const return_type& func( const type& name );

- Braces: Curly braces should open on the same line as the function declaration or control structure. 

Example: 

```	
void	function( void ) { 

		/*function body here*/
		if (condtion) {
			/*code here*/
		}
		std::cout << "long line of printing stuff here " << "more ..."
			<< "even more ..." << std::endl;
	}
```

## Functions

- A function that doesn’t take arguments must be explicitly prototyped with the
word "void" as the argument.

- Return of a function has to be between parenthesis.

- Parameters in functions’ prototypes must be named.

- Each function must be separated from the next by an empty line.

- Each function must have a single tabulation between its return type and its name.

- Function parameters should have a leading and trailing space inside of the parenthesis `()`.

- Each function should have a single space following the parameters, between the `)`  and `{`.

```	
void	function( int num, std::string& input ) { 

		/*function body here*/
		if (condtion) {
			/*code here*/
		}
	}
```

## Typedefs, enum, union

- Do not declare enum or union in a `.cpp` file

- Add a tabulation when declaring a struct, enum or union.

- When declaring a variable of type struct, enum or union, add a single space in the
type

- You must indent all structures’ names on the same column for their scope.

## Classes

** Examples of the class formatting will be maintained in the Example.hpp and Example.cpp **

** The cpp_class_writer.sh script will be maintained to produce the intial desired format **

- Class declarations should be in a `.hpp` file and defintions should be in a separate `.cpp` file named after the class

- There should only be one class declaration per `.hpp` file unless the classes are nested.

- Specified private member functions and attributes should be defined before public methods.

- Static attributes should be intialized at the top of the class `.cpp` or `.tpp` file unless they are const, in which case they may be intialized in the `.hpp`

- For Constructors, if an intialization list is used, it should be on the line following the defintion.

## Headers

- Header files must be protected from double inclusions. If the file is ft_foo.h, its bystander macro is FT_FOO_H.

- Use forward declarations when possible

## Templates

- Templates should be defined in a `.tpp` file. 

- If the template is a Class the class definition should be in an `.hpp` file and the function defintions should be written in a `.tpp` and included to the header.

- Template use should always be explicit. Example: template<type>(params)

## Macros and Pre-processors

- Macros used project wide in separate header. Example: Color codes

- Macro names must be all uppercase.

- You must indent characters following #if, #ifdef or #ifndef.

## Comments

- Comment should document the use of the code so that someone reading the comment without the code can understand the purpose and function.

- Comments should be written following an agreed on Doxygen format. Example:

```	
	/*! \brief Brief description.
	*         Brief description continued.
	*
	*  Detailed description starts here.
	*/
```


## Files

- Do not include a `.cpp` file


## Makefile

- The Makefile must at least contain the rules:
	$(NAME), all, clean, fclean and re

- The Makefile which will compile your source files. It must not relink

