#ifndef RESPONSECODES_HPP
# define RESPONSECODES_HPP

# include <map>
# include <string>

class	ResponseCodes {

	private:
		ResponseCodes( void );
		ResponseCodes( const ResponseCodes& to_copy );
		~ResponseCodes( void );
		ResponseCodes&	operator=( const ResponseCodes& to_copy );

		/* PRIVATE METHODS AND MEMBERS */
		static std::map<int, std::string>	codes_;

		static void	initialize_codes( void );

	public:
		/* PUBLIC METHODS */
		static std::string	getCodeElementBody( int code );
		static std::string	getCodeStatusLine( int code );
		static std::string	getCombinedStatusLineAndBody( int code );
};

#endif
