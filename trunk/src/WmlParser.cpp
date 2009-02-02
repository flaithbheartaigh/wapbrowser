/*
============================================================================
 Name        : WmlParser.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWmlParser implementation
============================================================================
*/
#include <e32debug.h>
#include <aknnotewrappers.h>
#include <utf.h>

#include "WmlParser.h"
#include "tinyxml.h"
//#include "PageBuilder.h"
#include "Define.h"

//#include "UtilityTools.h"

//#define TRACE RDebug::Printf
//#define TRACE(x,...) RDebug::Printf(x,...)
//#define TRACE printf

RPointerArray<HBufC> BufArray;
void TRACE(const char* str)
{
	return;

	//HBufC* textResource = StringLoader::LoadLC( R_HEWB_COMMAND1_TEXT );

	TPtrC8 ptr((const TUint8*)str);
	HBufC* buf = HBufC::NewLC(ptr.Length());
	buf->Des().Copy(ptr);

	RDebug::Print(*buf);
	CleanupStack::PopAndDestroy();

	buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(ptr);
	BufArray.Append(buf);
// 	RDebug::Print(*buf);
// 	delete buf;
// 	
// 	CAknInformationNote* informationNote = new ( ELeave ) CAknInformationNote;
// 	informationNote->ExecuteLD(*buf);

}


CWmlParser::CWmlParser()
{
	// No implementation required

	RDebug::Print(_L("CWmlParser::CWmlParser"));
}


CWmlParser::~CWmlParser()
{
	RDebug::Print(_L("CWmlParser::~CWmlParser"));
	for (int i = 0 ; i < BufArray.Count() ; i++)
	{
		RDebug::Print(*BufArray[i]);
	}


	//BufArray.ResetAndDestroy();
}

CWmlParser* CWmlParser::NewLC(MPageBuilder& aPageBuilder)
{
	CWmlParser* self = new (ELeave)CWmlParser();
	CleanupStack::PushL(self);
	self->ConstructL(aPageBuilder);
	return self;
}

CWmlParser* CWmlParser::NewL(MPageBuilder& aPageBuilder)
{
	CWmlParser* self=CWmlParser::NewLC(aPageBuilder);
	CleanupStack::Pop(); // self;
	return self;
}


void CWmlParser::ConstructL(MPageBuilder& aPageBuilder)
{
	TiXmlDocument doc("C:\\default.xml");
	//doc.LoadFile("C:\\default.xml");
	doc.LoadFile();

	//doc.Clear();
	//return;

	if(TiXmlElement* root = doc.RootElement())	//wml tag
	{
		//delete root;
		if(TiXmlElement* card = root->FirstChildElement("card"))	//card tag
		{
			const char* id = card->Attribute("id");					//id attribute
			const char* title = card->Attribute("title");			//title attribute
			TRACE(id);
			TRACE(title);
			if(TiXmlElement* p = card->FirstChildElement("p"))		//p tag
			{
				const char* align = p->Attribute("align");			//align attribute
				TRACE(align);
				
				TiXmlNode* childNode = p->FirstChild();
				do 
				//while (TiXmlElement* child = p->NextSiblingElement());
				{
					if(childNode->Type() == TiXmlNode::TEXT)
					{
						const char* text = childNode->Value();
						TRACE(text);
						aPageBuilder.AddText(text);
						//TRACE("%d",10);
					}
					else if(childNode->Type() == TiXmlNode::ELEMENT)
					{
						TiXmlElement* child = childNode->ToElement();
						const char* value = child->Value();
						if(strcmp(value,"a") == 0)		//链接
						{
							const char* href = child->Attribute("href");
							if(TiXmlNode* node = child->FirstChild())
							{
								switch(node->Type())
								{
								case TiXmlNode::TEXT:
									{
										const char* text = node->Value();
										TRACE(text);
										aPageBuilder.AddText(text,href);
									}
									break;

								case TiXmlNode::ELEMENT:
									if(TiXmlElement* ele = node->ToElement())
									{
										if(strcmp(ele->Value(),"img") == 0)		//图片
										{
											const char* src = ele->Attribute("src");
											const char* alt = ele->Attribute("alt");
											aPageBuilder.AddPicture(src,alt,href);
										}
									}
									break;

								default:
									break;
								}
							}
						}
						else if(strcmp(value,"br") == 0)		//换行
						{
							TRACE("br");
							aPageBuilder.AddBr();
						}
						else if(strcmp(value,"img") == 0)		//图片
						{
							const char* src = child->Attribute("src");
							const char* alt = child->Attribute("alt");
							aPageBuilder.AddPicture(src,alt);
						}
					}

					//if(
				}while (childNode = childNode->NextSibling());
				
				//delete p;
			}
			//delete card;
		}
		//delete root;
	}
}
