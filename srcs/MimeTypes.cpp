
#include "MimeTypes.hpp"

/************ CLASS STATIC ATTRIBUTE INTIALIZATION **************/
std::map<std::string, std::string>	MimeTypes::mime_types_;

/* CONSTRUCTORS */

/*! \brief default constructor, private for static class
*       
*	Default constructor, private for static class.
*
*/
MimeTypes::MimeTypes( void ) {}

/*! \brief copy constructor, private for static class
*       
*	Copy constructor, private for static class.
*
*/
MimeTypes::MimeTypes( const MimeTypes& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

/*! \brief default destructor, private for static class
*       
*	Default destructor, private for static class.
*
*/
MimeTypes::~MimeTypes( void ) {} 

/* OPERATOR OVERLOADS */

/*! \brief copy assignment operator, private for static class
*       
*	Copy assignment operator, private for static class.
*
*/
MimeTypes&	MimeTypes::operator=( const MimeTypes& rhs ) {

	(void)rhs;
	return *this;
}

/* CLASS PUBLIC METHODS */

/*! \brief returns reference to the mime type belonging to the param extension
*       
*	Returns reference to the mime type belonging to the param extension. If mime
*	type is not stored, an empty string is returned.
*
*/
std::string&	MimeTypes::getMimeTypeByExtension( std::string extension ) {

	if (MimeTypes::mime_types_.empty())
		MimeTypes::intializeMimeTypes();
	return MimeTypes::mime_types_[extension];
}

/* CLASS PRIVATE METHODS */


/*! \brief	used to intialize Mime type values
*
*
*	Sets values in the mime_types_ map to use for recognizing and setting mime types.
*	All values are used based on the IANA standards linked below.
*
*	read more about mime types in <a href="https://www.iana.org/assignments/media-types/media-types.xhtml">iana.org media-types page</a>
*/
void	MimeTypes::intializeMimeTypes( void ) {

	//text
	MimeTypes::mime_types_["html"] = "text/html";
	MimeTypes::mime_types_["css"] = "text/css";
	MimeTypes::mime_types_["txt"] = "text/plain";
	MimeTypes::mime_types_["js"] = "text/javascript";
	MimeTypes::mime_types_["md"] = "text/markdown";
	MimeTypes::mime_types_["csv"] = "text/csv";
	MimeTypes::mime_types_["rtf"] = "text/rtf";
	MimeTypes::mime_types_["xml"] = "text/xml";
	// text/x-python-script

	//application
	MimeTypes::mime_types_["unknown"] = "application/octet-stream";//unkown binary file
	MimeTypes::mime_types_["pdf"] = "application/pdf";
	MimeTypes::mime_types_["msword"] = "application/msword";//?
	MimeTypes::mime_types_["zip"] = "application/zip";
	MimeTypes::mime_types_["x-www-form-urlencoded"] ="application/x-www-form-urlencoded";

	//audio
	// MimeTypes::mime_types_["mp4"] = {"mp4", "audio/mp4";

	//font
	MimeTypes::mime_types_["ttf"] = "font/ttf";

	//image
	MimeTypes::mime_types_["bmp"] = "image/bmp";
	MimeTypes::mime_types_["gif"] = "image/gif";
	MimeTypes::mime_types_["jpg"] = "image/jpeg";
	MimeTypes::mime_types_["jpeg"] = "image/jpeg";
	MimeTypes::mime_types_["png"] = "image/png";
	MimeTypes::mime_types_["tiff"] = "image/tiff";
	MimeTypes::mime_types_["ico"] = "image/x-icon";

	//message


	//model
	//multipart

	//video
	// MimeTypes::mime_types_["jpeg"] = {"jpeg", "video/jpeg", true};
	MimeTypes::mime_types_["mp4"] = "video/mp4";
	MimeTypes::mime_types_["raw"] = "video/raw";
	MimeTypes::mime_types_["MPV"] = "video/MPV";

}
