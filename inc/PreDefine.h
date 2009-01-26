/*
============================================================================
 Name        : PreDefine.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : PreDefine.h - header file
============================================================================
*/

#ifndef PREDEFINE_H
#define PREDEFINE_H

#include <e32base.h> 
#include <gdi.h>
#include <w32std.h>
#include <coedef.h>
#include <coemain.h>
#include <badesca.h>
#include <coecntrl.h>
#include <utf.h>
#include <aknutils.h>
#ifdef __SERIES60_3X__
	#include <akniconutils.h>
#endif
#include <string.h>

#include <fontids.hrh>


//#include "tinyxml.h"

#include "CoCoPreDefine.h"
#include "UtilityTools.h"
//#include "const.h"

//#define __WRITE_LOG__

/*
//#undef ASSERT
//#ifndef __WINS__
#ifndef ASSERT
#define ASSERT(a)	if(a == 0)	\
					{	\
					TPtrC8 ptr8((const TUint8*)__FILE__,strlen(__FILE__));	\
					TFileName buf;			\
					buf.Copy(ptr8);			\
					TParse parse;			\
					parse.Set(buf,NULL,NULL);		\
					WriteLogsL(ptr8);		\
					WriteLogsL(parse.Name());		\
					WriteLogsL(_L("%d"),__LINE__);		\
					User::Panic(buf,__LINE__);		\
					}	
					//User::Panic(parse.Name(),__LINE__);	
#else
#endif	
*/

#undef ASSERT

#define ASSERT(a)	if(a == 0)	\
					{	\
					TPtrC8 ptr8((const TUint8*)__FILE__,strlen(__FILE__));	\
					TFileName buf;			\
					buf.Copy(ptr8);			\
					TParse parse;			\
					parse.Set(buf,NULL,NULL);		\
					UtilityTools::WriteLogsL(ptr8);		\
					UtilityTools::WriteLogsL(parse.Name());		\
					UtilityTools::WriteLogsL(_L("%d"),__LINE__);		\
					User::Panic(buf,__LINE__);		\
					}	
/*

//以下断言发生时将显示出断言失败的文件名与行号
#ifdef __WINS__
#define ASSERT(a)	if(a == 0)	\
					{	\
					TPtrC8 ptr8((const TUint8*)__FILE__,strlen(__FILE__));	\
					TFileName buf;			\
					buf.Copy(ptr8);			\
					TParse parse;			\
					parse.Set(buf,NULL,NULL);		\
					User::Panic(buf,__LINE__);		\
					}	
#else
//有的手机无法显示对话框,将错误记录在日志里
#define ASSERT(a)	if(a == 0)	\
					{	\
					TPtrC8 ptr8((const TUint8*)__FILE__,strlen(__FILE__));	\
					TFileName buf;			\
					buf.Copy(ptr8);			\
					TParse parse;			\
					parse.Set(buf,NULL,NULL);		\
					UtilityTools::WriteLogsL(ptr8);		\
					UtilityTools::WriteLogsL(parse.Name());		\
					UtilityTools::WriteLogsL(_L("%d"),__LINE__);		\
					User::Panic(buf,__LINE__);		\
					}	
#endif
*/

#define KKeyCodeUp			EKeyUpArrow
#define KKeyCodeDown		EKeyDownArrow
#define KKeyCodeLeft		EKeyLeftArrow
#define KKeyCodeRight		EKeyRightArrow

#define KLeftSoftKey		EKeyDevice0
#define KRightSoftKey		EKeyDevice1
#define KOKKey				EKeyDevice3

#define KKeyCodeHome		'1'
#define KKeyCodeEnd			'4'
#define KKeyCodePageUp		'2'
#define KKeyCodePageDown	'5'

#define KRgb1		TRgb(153,190,47)
#define KRgb2		TRgb(255,245,0)


#define KNaviPaneTextPoint	TPoint(4,38)
#define KNaviPaneSize		TSize(169, 20)
#define KNaviPaneRect		TRect(KNaviPaneTextPoint,KNaviPaneSize)

#define KLeftButtonPos		TPoint(3,191)
#define KRightButtonPos		TPoint(173,191)

#define KDiglogPos			TPoint(19,74)
#define KDialogIconPos		KDiglogPos + TPoint(2, 23)

_LIT8(KContentType,		"text/plain");


_LIT(KBodyReceived,"Body received");
_LIT(KAnotherEvent, "Another Event!");
_LIT(KBlank,		" ");

_LIT(KResourcePath,		"\\system\\Apps\\soarsky\\");

#define LINE_HEIGHT 19

#define KThreadNumPerPage		15
#define KPostNumPerPage			10
#define KThreadMaxPage			10
#define KPostMaxPage			10

class CThread;
class CPost;

typedef RPointerArray<CThread>	RThreadArray;
typedef RPointerArray<CPost> RPostArray;

_LIT8(KDivision,	"-------------------------------------------------------------");
_LIT8(KCarriageReturn,		"\r\n");

_LIT(KTimeOut,"TimeOut");
_LIT(KTmpXmlFile,	"\\TmpXml.xml");

#define TmpXmlFile	"C:\\TmpXml.xml"
//const TInt KCurrentFontID = ELatinBold12;

//const CFont* KFont = AknLayoutUtils::FontFromId(ELatinBold12);
//#define KFont AknLayoutUtils::FontFromId(ELatinBold12)
#define KFont AknLayoutUtils::FontFromId(EFontCustom)

//EFontCustom for 352*416
const TInt KStandardTimerInterval = 100000;
/*
#ifdef __WINS__
#else
const TInt KStandardTimerInterval = 50000;
#endif
*/

const TInt KCmdEventNull = -1;


class TPageRecord
{
public:
	TFileName	iFileName;		//文件全路径
	//TInt		iModuleType;	//此记录属于哪个模块
	TInt		iFirstTypeId;	//一级栏目ID
	TInt		iSecondTypeId;	//二级栏目ID
	TInt		iThirdTypeId;	//三级栏目ID
	TInt		iPageIndex;		//此记录是模块中的哪一页
	TInt		iTimeStamp;		//时间戳,只有第一页有效
};

#endif // PREDEFINE_H
