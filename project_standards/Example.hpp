#ifndef EXAMPLE_HPP
# define EXAMPLE_HPP

class	Example {

	private:
		/* PRIVATE METHODS AND MEMBERS */

	public:
		Example( void );
		Example( /*add parameters*/ );
		Example( const Example& to_copy );

		~Example( void );

		Example&	operator=( const Example& to_copy );

		/* PUBLIC METHODS */

};

#endif
