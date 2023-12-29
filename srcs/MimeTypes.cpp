
#include "MimeTypes.hpp"

std::map<std::string, std::string>	MimeTypes::mime_types_;

/* CONSTRUCTORS */

// MimeTypes::MimeTypes( void ) {

// 	/* default constructor */
// }

// MimeTypes::MimeTypes( /*add parameters*/ ) {

// 	/* parameterized constructor */
// }

// MimeTypes::MimeTypes( const MimeTypes& to_copy ) {

// 	/* copy constructor */
// } 

// /* DESTRUCTOR */

// MimeTypes::~MimeTypes( void ) {

// 	/* destructor */
// } 

/* OPERATOR OVERLOADS */

// MimeTypes&	MimeTypes::operator=( const MimeTypes& rhs ) {

// 	/* copy assignment operator overload */
// }

/* CLASS PUBLIC METHODS */

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
* 	REFERENCE: https://www.iana.org/assignments/media-types/media-types.xhtml
*
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

	//application
	MimeTypes::mime_types_["unknown"] = "application/octet-stream";//unkown binary file
	MimeTypes::mime_types_["pdf"] = "application/pdf";
	MimeTypes::mime_types_["msword"] = "application/msword";//?
	MimeTypes::mime_types_["zip"] = "application/zip";

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