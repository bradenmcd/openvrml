#ifndef INC_CommonASTWithHiddenTokens_hpp__
#define INC_CommonASTWithHiddenTokens_hpp__

/** A CommonAST whose initialization copies hidden token
 *  information from the Token used to create a node.
 */

#include "config.hpp"
#include "CommonAST.hpp"

ANTLR_BEGIN_NAMESPACE(antlr)

class CommonASTWithHiddenTokens : public CommonAST {
protected:
	RefToken hiddenBefore,hiddenAfter; // references to hidden tokens
public:
	RefToken getHiddenAfter() const
		{ return hiddenAfter; }
	RefToken getHiddenBefore() const
		{ return hiddenBefore; }

	void initialize(RefToken t);

	static RefAST factory();
};

typedef ASTRefCount<CommonASTWithHiddenTokens> RefCommonASTWithHiddenTokens;

ANTLR_END_NAMESPACE

#endif //INC_CommonASTWithHiddenTokens_hpp__
