/*
============================================================================
 Name        : WmlParser.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWmlParser implementation
============================================================================
*/
#include "Define.h"
#include "WmlParser.h"
#include "WapBrowserappui.h"
#include "WapEngine.h"
#include "ImageEngine.h"
//#include "PageBuilder.h"


#include "UtilityTools.h"

//#define TRACE RDebug::Printf
//#define TRACE(x,...) RDebug::Printf(x,...)
//#define TRACE printf

//RPointerArray<HBufC> BufArray;
void TRACE(const char* str)
{
	return;

	//HBufC* textResource = StringLoader::LoadLC( R_HEWB_COMMAND1_TEXT );

	TPtrC8 ptr((const TUint8*)str);
	HBufC* buf = HBufC::NewLC(ptr.Length());
	buf->Des().Copy(ptr);

	RDebug::Print(*buf);
	CleanupStack::PopAndDestroy();

// 	buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(ptr);
// 	BufArray.Append(buf);


// 	RDebug::Print(*buf);
// 	delete buf;
// 	
// 	CAknInformationNote* informationNote = new ( ELeave ) CAknInformationNote;
// 	informationNote->ExecuteLD(*buf);

}

CWmlParser::CWmlParser(CWapEngine& aWapEngine,MPageBuilder& aPageBuilder)
	: iWapEngine(aWapEngine)
	, iPageBuilder(aPageBuilder)
{
	// No implementation required

	UtilityTools::WriteLogsL(_L("CWmlParser::CWmlParser"));
	RDebug::Print(_L("CWmlParser::CWmlParser"));
}

CWmlParser::~CWmlParser()
{
/*
	RDebug::Print(_L("CWmlParser::~CWmlParser"));
	for (int i = 0 ; i < BufArray.Count() ; i++)
	{
		RDebug::Print(*BufArray[i]);
	}
*/


	//BufArray.ResetAndDestroy();
}

CWmlParser* CWmlParser::NewLC(CWapEngine& aWapEngine,MPageBuilder& aPageBuilder)
{
	CWmlParser* self = new (ELeave)CWmlParser(aWapEngine,aPageBuilder);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CWmlParser* CWmlParser::NewL(CWapEngine& aWapEngine,MPageBuilder& aPageBuilder)
{
	CWmlParser* self=CWmlParser::NewLC(aWapEngine,aPageBuilder);
	CleanupStack::Pop(); // self;
	return self;
}


void CWmlParser::ConstructL()
{

}

TBool CWmlParser::ParseFile(const char* aFileName)
{
	//TiXmlDocument doc("C:\\default.xml");
	TiXmlDocument doc(aFileName);
	//doc.LoadFile("C:\\default.xml");
	doc.LoadFile();
	return Parse(doc);
}

TBool CWmlParser::ParseData(const char* p)
{
	UtilityTools::WriteLogsL(_L("CWmlParser::ParseData"));

	TPtrC8 text;
	text.Set((const unsigned char*)p);
	UtilityTools::WriteLogsL(text.Left(120));
	UtilityTools::WriteLogsL(_L("CWmlParser::ParseData.."));

	TiXmlDocument doc;
	doc.Parse(p);
	return Parse(doc);
	UtilityTools::WriteLogsL(_L("CWmlParser::ParseData End"));
}

TBool CWmlParser::Parse(TiXmlDocument& doc)
{
	UtilityTools::WriteLogsL(_L("CWmlParser::Parse1"));
	if(TiXmlElement* root = doc.RootElement())	//wml tag
	{
		UtilityTools::WriteLogsL(_L("CWmlParser::Parse2"));
		if(TiXmlElement* card = root->FirstChildElement("card"))	//card tag
		{
			UtilityTools::WriteLogsL(_L("CWmlParser::Parse3"));
			/*
			<card ontimer="http://xdown.monternet.com/portalapp/wml/content/logic/queryHotLogic.jsp?page=1&amp;mark=4&amp;AUTH=true&amp;S=1&amp;CHANNEL=0002">
							<timer value="10"/>
							<p/>
							</card>
			*/			
			if(const char* ontimer = card->Attribute("ontimer"))
			{
				TPtrC8 link;
				link.Set((const unsigned char*)ontimer);
				CWapBrowserAppUi::Static()->RequestPageL(link);
			}
			else if(const char* onenterforward  = card->Attribute("onenterforward "))
			{
				TPtrC8 link;
				link.Set((const unsigned char*)onenterforward);
				CWapBrowserAppUi::Static()->RequestPageL(link);
			}
			else if(TiXmlElement* onevent = card->FirstChildElement("onevent"))
			{
				if(const char* type  = onevent->Attribute("type"))
				{
					if(strcmp(type,"onenterforward") == 0)
					{
						if(TiXmlElement* go = onevent->FirstChildElement("go"))
						{
							if(const char* href = go->Attribute("href"))
							{
								UtilityTools::WriteLogsL(_L("href:"));
								TPtrC8 link((const TUint8*)href);
// 								TPtrC8 link;
// 								link.Set((const unsigned char*)href);
								UtilityTools::WriteLogsL(link.Left(127));
								CWapBrowserAppUi::Static()->RequestPageL(link);
							}
						}
					}
				}
			}
			else
			{
				const char* id = card->Attribute("id");					//id attribute
				const char* title = card->Attribute("title");			//title attribute
				TRACE(id);
				TRACE(title);
				TiXmlElement* p = card->FirstChildElement("p");
				//do
				while(p)//TiXmlElement* p = card->FirstChildElement("p"))		//p tag
				{
					//const char* align = p->Attribute("align");			//align attribute
					//				TRACE(align);
					ParseP(p);
					iPageBuilder.AddBr();
					p = p->NextSiblingElement("p");
					//delete p;
				}//while(p = p->NextSiblingElement("p"));//TiXmlElement* p = card->FirstChildElement("p")
				//iWapEngine.ImageEngine().StartPicSession();
				return ETrue;
			}
			//delete card;
			UtilityTools::WriteLogsL(_L("CWmlParser::Parse8"));
		}
		//delete root;
		UtilityTools::WriteLogsL(_L("CWmlParser::Parse9"));
		//return ETrue;
	}
	UtilityTools::WriteLogsL(_L("CWmlParser::Parse10"));
	return EFalse;
}

void CWmlParser::ParseP(TiXmlElement* p)
{
	TiXmlNode* childNode = p->FirstChild();
	while (childNode)
	{
		ParseChild(childNode);
		childNode = childNode->NextSibling();
	}	 
}

void CWmlParser::ParseChild(TiXmlNode* childNode)
{
	if(childNode->Type() == TiXmlNode::TEXT)
	{
		const char* text = childNode->Value();
		TRACE(text);
		iPageBuilder.AddText(text);
		//TRACE("%d",10);
	}
	else if(childNode->Type() == TiXmlNode::ELEMENT)
	{
		TiXmlElement* child = childNode->ToElement();
		const char* value = child->Value();
		if(strcmp(value,"a") == 0)		//链接
		{
			const char* href = child->Attribute("href");
			TiXmlNode* node = child->FirstChild();
			while(node)
			{
				switch(node->Type())
				{
				case TiXmlNode::TEXT:
					{
						const char* text = node->Value();
						TRACE(text);
						iPageBuilder.AddText(text,href);
					}
					break;

				case TiXmlNode::ELEMENT:
					if(TiXmlElement* ele = node->ToElement())
					{
						if(strcmp(ele->Value(),"img") == 0)		//图片
						{
							const char* src = ele->Attribute("src");
							const char* alt = ele->Attribute("alt");
							iPageBuilder.AddPicture(src,alt,href);
						}
					}
					break;

				default:
					break;
				}
				node = node->NextSibling();
			}
		}
		else if(strcmp(value,"br") == 0)		//换行
		{
			TRACE("br");
			iPageBuilder.AddBr();
		}
		else if(strcmp(value,"img") == 0)		//图片
		{
			const char* src = child->Attribute("src");
			const char* alt = child->Attribute("alt");
			iPageBuilder.AddPicture(src,alt);
		}
		else if(strcmp(value,"input") == 0)
		{
			//ParseInput(child);
		}
		else if(strcmp(value,"anchor") == 0)
		{
			ParseAnchor(child);
		}
		//archor在这里被忽略
	}
}

void CWmlParser::ParseInput(TiXmlElement* element)
{
	ASSERT(strcmp(element->Value(),"input") == 0);

	const char* name		= element->Attribute("name");
	const char* type		= element->Attribute("type");
	const char* tabindex	= element->Attribute("tabindex");
	const char* value		= element->Attribute("value");
	const char* size		= element->Attribute("size");

	while (element = element->NextSiblingElement())
	{
		const char* elementName = element->Value();
		if(strcmp(elementName,"anchor") == 0)
		{
			ParseAnchor(element);
		}
		else
		{
			break;
		}
	}
}

void CWmlParser::ParseAnchor(TiXmlElement* element)
{
	ASSERT(strcmp(element->Value(),"anchor") == 0);

	TiXmlNode* node = element->FirstChild();
	const char* text = NULL;
	const char* link = NULL;
	const char* method = NULL;
	char* body = NULL;
	while (node)
	{
		switch(node->Type())
		{
		case TiXmlNode::TEXT:
			text = node->Value();
			break;

		case TiXmlNode::ELEMENT:
			if(TiXmlElement* goElement = element->FirstChildElement("go"))
			{
				link	= goElement->Attribute("href");
				method	= goElement->Attribute("method");


				if(TiXmlElement*postfieldElement = goElement->FirstChildElement("postfield"))
				{
					const char* name	= postfieldElement->Attribute("name");
					const char* value	= postfieldElement->Attribute("value");

					int namelen		= strlen(name);
					int valuelen	= strlen(value);
					int bodylen		= namelen + valuelen + 2;

					body = new char[bodylen];
					memset(body,0,bodylen);
					memcpy(body,name,namelen);
					memset(body + namelen,'=',1);
					memcpy(body + namelen + 1,value,valuelen);


					//HBufC8* 

					//bid=2&keyword=aaa
/*
					if(postfieldElement = postfieldElement->NextSiblingElement())
					{
						const char* name	= postfieldElement->Attribute("name");
						const char* value	= postfieldElement->Attribute("value");
					}
		*/

				}
			}
			break;

		default:
			break;
		}
		node = node->NextSibling();
	}
	if(method && strcmp(method,"post") == 0)
	{
		iPageBuilder.AddPostText(text,link,body);
		delete body;
	}
	else
	{
		iPageBuilder.AddText(text,link);
	}

/*
	if(TiXmlElement* goElement = element->FirstChildElement("go"))
	{
		const char* href	= goElement->Attribute("href");
		const char* method	= goElement->Attribute("method");

		if(TiXmlElement*postfieldElement = goElement->FirstChildElement("postfield"))
		{
			const char* name	= postfieldElement->Attribute("name");
			const char* value	= postfieldElement->Attribute("value");

			if(postfieldElement = postfieldElement->NextSiblingElement())
			{
				const char* name	= postfieldElement->Attribute("name");
				const char* value	= postfieldElement->Attribute("value");
			}
		}

		TiXmlNode* node = goElement->NextSibling();
		if(node && node->Type() == TiXmlNode::TEXT)
		{
			const char* text = node->Value();
			TRACE(text);
			iPageBuilder.AddText(text,href);
		}
	}
*/

}
