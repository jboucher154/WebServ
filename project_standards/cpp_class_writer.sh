#!/bin/bash 

if [ $# -eq 0 ]; then
	echo "No arguments provided"
	echo "Usage: ./class_writer.sh <class_name>"
	# add other usage notes when defined
	exit 1
fi

CAPS_CLASS_NAME=$(echo "$1" | tr '[:lower:]' '[:upper:]')
CLASS_FORMAT="class	$1 
{
	private:
		/* PRIVATE METHODS AND MEMBERS */

	public:
		$1( void );
		$1( /*add parameters*/ );
		$1( const $1& to_copy );

		~$1( void );

		$1&	operator=( const $1& to_copy );

		/* PUBLIC METHODS */

};"

HEADER_GUARD_WITH_CLASS="#ifndef ${CAPS_CLASS_NAME}_HPP
# define ${CAPS_CLASS_NAME}_HPP

${CLASS_FORMAT}

#endif"

INCLUDES="
#include \"$1.hpp\"

"
DEFAULT_CONSTRUCTOR="$1::$1( void )
{
	/* default constructor */
}
"

PARAMETERIZED_CONSTRUCTOR="$1::$1( /*add parameters*/ )
{
	/* parameterized constructor */
}
"

COPY_CONSTRUCTOR="$1::$1( const $1& to_copy )
{
	/* copy constructor */
}
"

DESTRUCTOR="$1::~$1( void )
{
	/* destructor */
}
"

COPY_ASSIGNMENT="$1&	$1::operator=( const $1& to_copy )
{
	/* copy assignment operator overload */
}
"

SOURCE_FORMAT="${INCLUDES} 

/* CONSTRUCTORS */

${DEFAULT_CONSTRUCTOR}${PARAMETERIZED_CONSTRUCTOR}${COPY_CONSTRUCTOR} 

/* DESTRUCTOR */

${DESTRUCTOR} 

/* OPERATOR OVERLOADS */

${COPY_ASSIGNMENT}

/* CLASS PUBLIC METHODS */

/* CLASS PRIVATE METHODS */"

# create the class files
touch $1.hpp
echo "${HEADER_GUARD_WITH_CLASS}" > $1.hpp
echo "$1.hpp created"
touch $1.cpp
echo "${SOURCE_FORMAT}" > $1.cpp
echo "$1.cpp created"
