#ifndef MIMETYPES_HPP
# define MIMETYPES_HPP

# include <map>
# include <string>

/*! \brief MimeTypes is a utility class for handling MIME types within the web server.
 *
 *	@class MimeTypes
 *	This @b static @b class provides functionality to map file extensions to their corresponding
 *	MIME types. It is designed to be used within the web server to accurately identify
 *	the content type of files based on their extensions.
 */
class	MimeTypes {

	private:
		MimeTypes( void );
		MimeTypes( const MimeTypes& to_copy );
		~MimeTypes( void );
		MimeTypes&	operator=( const MimeTypes& to_copy );

		/* PRIVATE METHODS AND MEMBERS */
		static void	intializeMimeTypes( void );

		/* MAP of MIME TYPES */
		static	std::map<std::string, std::string> mime_types_; /*!< static map of extensions and their corresponding mime types */

	public:
		/* PUBLIC METHODS */
		static std::string&	getMimeTypeByExtension( std::string extension );
		//make reverse lookup?
};

#endif
