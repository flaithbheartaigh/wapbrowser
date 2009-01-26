/*
============================================================================
 Name        : WmlParser.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWmlParser declaration
============================================================================
*/

#ifndef WMLPARSER_H
#define WMLPARSER_H
// INCLUDES
#include <e32std.h>
#include <e32base.h>
// CLASS DECLARATION

class MPageBuilder;

class CWmlParser : public CBase
{
public: // Constructors and destructor
	~CWmlParser();
	static CWmlParser* NewL(MPageBuilder& aPageBuilder);
	static CWmlParser* NewLC(MPageBuilder& aPageBuilder);

private:
	CWmlParser();
	void ConstructL(MPageBuilder& aPageBuilder);

private:

};

#endif // WMLPARSER_H
