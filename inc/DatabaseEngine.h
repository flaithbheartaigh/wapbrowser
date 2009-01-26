/*
============================================================================
 Name        : DatabaseEngine.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CDatabaseEngine declaration
============================================================================
*/

#ifndef DATABASEENGINE_H
#define DATABASEENGINE_H

#include <e32std.h>
#include <badesca.h>    // CDesCArrayFlat (cannot be forward declarated)
#include <d32dbms.h>    // RDbStoreDatabase
#include <f32file.h>    // RFs

//#include "CoCoPreDefine.h"

// Forward declarations
class CFileStore;

// Constants
_LIT(KBookMarksTable, "BookMarks");                
_LIT(KBookNameCol, "BookName");           
_LIT(KBookMarkLineCol, "Line");             

_LIT(KBooksIndexName,"BooksIndex");        

//下载历史记录
_LIT(KDownLoadHistoryTable, "DownLoadHistory");                
_LIT(KDownLoadFileNameCol, "FileName");           
_LIT(KDownLoadURLCol, "URL");             
_LIT(KDownLoadFileSizeCol, "FileSize");             
_LIT(KDownLoadTimeCol, "DownLoadTime");             

//系统参数表
_LIT(KSystemParmTable, "SystemParm");                
_LIT(KParmNameCol, "ParmName");           
_LIT(KParmValueCol, "ParmValue");             

_LIT(KSystemParmIndexName,"ParmIndex");        

const int KTitleMaxLength = 60;            
const int KDescriptionMaxLength = 128;     

const int KBookItemMaxLength = 256;

_LIT(KSeparator,"|");

class CDatabaseEngine : public CBase
{
public: // Creation and destruction

	static CDatabaseEngine* NewL();

	~CDatabaseEngine();


public: // Public API for database operations

	TInt OpenDb(const TFileName& aExistingBookFile);

	TInt CreateDb(const TFileName& aNewBookFile);

	TInt RemoveDb(const TFileName& aExistingBookFile);

	TInt Close();

	TBool IsOpen() const;

	//BookMark Table
	TInt AddBookMarkWithSql(const TDesC& aBookName,
		const TInt aLine);

	TInt AddBookMarkWithCppApi(const TDesC& aBookName,
		const TInt aLine);


	TInt GetBookMarksByKeyL(const TDesC& aColumnName,
		const TDesC& aSearchString);

	TInt RemoveBookMarks(const TDesC& aTitle, TInt& aResultCount);

	TInt RemoveAllBookMarks(TInt& aResultCount);

	TInt UpdateBookMarkLine(const TDesC& aBookName,const TInt aNewLine);

	//DownLoadHistory Table
	TInt AddDownLoadHistoryWithSql(const TDesC& aFileName,
		const TDesC& aURL,
		const TInt aFileSize);

	TInt RemoveAllDownLoadHistory(TInt& aResultCount);

	TInt RemoveDownLoadHistory(const TDesC& aFileName,const TDesC& aURL);

	void GetAllDownLoadHistory(CArrayFixFlat<TINFOCONTENTTEXT>* aArray);

	TInt CheckIsDownLoadHistoryByKeyL(const TDesC& aURLName);
	//SystemParm Table
	TInt SetSystemParmWithSQL(const TDesC& aParmName,
		const TDesC& aParmValue);

	TInt GetSystemParmWithSQL(const TDesC& aParmName,
		TDes& aParmValue);


private: // Construction
	void ConstructL();

	CDatabaseEngine();

private: // Helpers

	void CreateBookMarksTableL();     /* Create the Books table                  */
	void CreateBookMarksIndexL();     /* Create the BooksIndex for Books table   */
	void DropBookMarksTable();       /* Drop Books table (DDL example)          */

	void CreateDownLoadHistoryTableL();     
	void DropDownLoadHistoryTable();

	void CreateSystemParmTableL();     /* Create the Books table                  */
	void CreateSystemParmIndexL();     /* Create the BooksIndex for Books table   */
	void DropSystemParmTable();       /* Drop Books table (DDL example)          */

private: // Member data
	RFs&				iFsSession;
	RDbStoreDatabase iDb;/* For database operations                 */
	CFileStore*      iFileStore;  /* For creating and opening database files */
	TBool            iOpen;       /* Flag indicating iBookDb open status*/
};


#endif // DATABASEENGINE_H
