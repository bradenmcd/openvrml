#ifndef INC_TokenStreamRecognitionException_hpp__
#define INC_TokenStreamRecognitionException_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/RIGHTS.html
 *
 * $Id: TokenStreamRecognitionException.hpp,v 1.1.1.1 2003-04-06 22:26:27 braden Exp $
 */

#include <antlr/config.hpp>
#include <antlr/TokenStreamException.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

class TokenStreamRecognitionException : public TokenStreamException {
public:
	TokenStreamRecognitionException(RecognitionException& re)
	: TokenStreamException(re.getMessage())
	, recog(re)
	{
	}
	~TokenStreamRecognitionException() throw()
	{
	}
	ANTLR_USE_NAMESPACE(std)string toString() const
	{
		return recog.getFileLineColumnString()+getMessage();
	}
private:
	RecognitionException recog;
};

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_TokenStreamRecognitionException_hpp__
