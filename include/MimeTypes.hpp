#ifndef MIMETYPES_HPP
# define MIMETYPES_HPP

# include <map>
# include <string>

class	MimeTypes {

	private:
		MimeTypes( void );
		MimeTypes( const MimeTypes& to_copy );
		~MimeTypes( void );
		MimeTypes&	operator=( const MimeTypes& to_copy );

		/* PRIVATE METHODS AND MEMBERS */
		static void	intializeMimeTypes( void );

		/* MAP of MIME TYPES */
		static	std::map<std::string, std::string> mime_types_;

	public:
		/* PUBLIC METHODS */
		static std::string&	getMimeTypeByExtension( std::string extension );
		//make reverse lookup?
};

#endif
