#include "LexerSharedInputState.hpp"
#include "CharBuffer.hpp"

ANTLR_BEGIN_NAMESPACE(antlr)

/** This object contains the data associated with an
 *  input stream of characters.  Multiple lexers
 *  share a single LexerSharedInputState to lex
 *  the same input stream.
 */

LexerInputState::LexerInputState(InputBuffer* inbuf)
: line(1)
, guessing(0)
, input(inbuf)
, inputResponsible(true)
{
}

LexerInputState::LexerInputState(InputBuffer& inbuf)
: line(1)
, guessing(0)
, input(&inbuf)
, inputResponsible(false)
{
}

LexerInputState::LexerInputState(ANTLR_USE_NAMESPACE(std)istream& in)
: line(1)
, guessing(0)
, input(new CharBuffer(in))
, inputResponsible(true)
{
}

LexerInputState::~LexerInputState()
{
	if (inputResponsible)
		delete input;
}

InputBuffer& LexerInputState::getInput()
{
	return *input;
}

ANTLR_END_NAMESPACE

