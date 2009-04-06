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

class CWapEngine;
class MPageBuilder;

// class TiXmlDocument;
// class TiXmlNode;

class CWmlParser : public CBase
{
public: // Constructors and destructor
	~CWmlParser();
	static CWmlParser* NewL(CWapEngine& aWapEngine,MPageBuilder& aPageBuilder);
	static CWmlParser* NewLC(CWapEngine& aWapEngine,MPageBuilder& aPageBuilder);

private:
	CWmlParser(CWapEngine& aWapEngine,MPageBuilder& aPageBuilder);
	void ConstructL();

public:
	TBool ParseFile(const char* aFileName);
	TBool ParseData(const char* p);
	TBool Parse(TiXmlDocument& doc);

	void ParseP(TiXmlElement* p);
	void ParseChild(TiXmlNode* child);
	void ParseInput(TiXmlElement* element);
	void ParseAnchor(TiXmlElement* element);

	//MPageBuilder& iPageBuilder;

private:
	CWapEngine&		iWapEngine;
	MPageBuilder&	iPageBuilder;
	const char*		iFileName;
};

#endif // WMLPARSER_H
