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

#include "tinyxml.h"

class MPageBuilder;

// class TiXmlDocument;
// class TiXmlNode;

class CWmlParser : public CBase
{
public: // Constructors and destructor
	~CWmlParser();
	static CWmlParser* NewL(MPageBuilder& aPageBuilder);
	static CWmlParser* NewLC(MPageBuilder& aPageBuilder);

private:
	CWmlParser(MPageBuilder& aPageBuilder);
	void ConstructL();

public:
	void ParseFile(const char* aFileName);
	void ParseData(const char* p);
	void Parse(TiXmlDocument& doc);

	
	void ParseInput(TiXmlElement* element);
	void ParseAnchor(TiXmlElement* element);

private:
	MPageBuilder& iPageBuilder;
	const char* iFileName;
};

#endif // WMLPARSER_H
