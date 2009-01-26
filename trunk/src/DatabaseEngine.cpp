/*
============================================================================
 Name        : DatabaseEngine.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CDatabaseEngine implementation
============================================================================
*/
#include <coemain.h>
#include <badesca.h>    // CDesCArrayFlat
#include <s32file.h>    // CFileStore & CPermanentFileStore
#include <bautils.h>    // file helpers
#include <eikenv.h>
#include "DBEngine.h"

// Implementation specific constants
const int KCustomSqlMaxLength = 256;
const int KArrayGranularity = 5;     // for CDesCArrayFlat

// ---------------------------------------------------------------------------
// CDatabaseEngine::NewL()
//
// Create instance of the Book database engine.
// ---------------------------------------------------------------------------
CDatabaseEngine* CDatabaseEngine::NewL() 
{
	CDatabaseEngine* self = new (ELeave)CDatabaseEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::~CDatabaseEngine()
//
// Destructor of the Book database engine. Release resources.
// ---------------------------------------------------------------------------
CDatabaseEngine::~CDatabaseEngine()
{
	Close();  // Just in case, if the user does not close this explicitely
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::ConstructL()
//
// Second phase construction. Leaves, if RFs session cannot be created.
// ---------------------------------------------------------------------------
void CDatabaseEngine::ConstructL()
{
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::CDatabaseEngine()
//
// Constructor
// ---------------------------------------------------------------------------
CDatabaseEngine::CDatabaseEngine() : iFsSession(CCoeEnv::Static()->FsSession())
								   : iOpen(EFalse)
{
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::OpenDbL()
//
// Open existing Book database for exclusive access.
// ---------------------------------------------------------------------------
TInt CDatabaseEngine::OpenDb(const TFileName& aExistingFile)
{
	Close();

	if(!BaflUtils::FileExists(iFsSession, aExistingFile))
	{
		return KErrNotFound;
	}

	TRAPD(error, 
		iFileStore = CPermanentFileStore::OpenL(iFsSession, aExistingFile, EFileRead|EFileWrite);
		iFileStore->SetTypeL(iFileStore->Layout());/* Set file store type*/
		iDb.OpenL(iFileStore,iFileStore->Root())
		);
	if(error!=KErrNone)
	{
		return error;
	}

	iOpen = ETrue;
	return KErrNone;
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::CreateDbL()
//
// Create a new database. The database will be in exclusive access mode.
// ---------------------------------------------------------------------------
TInt CDatabaseEngine::CreateDb(const TFileName& aBookFile)
{
	Close();

	// Create empty database file.
	TRAPD(error,
		iFileStore = CPermanentFileStore::ReplaceL(iFsSession, aBookFile, EFileRead|EFileWrite);
		iFileStore->SetTypeL(iFileStore->Layout());// Set file store type
		TStreamId id = iDb.CreateL(iFileStore);// Create stream object
		iFileStore->SetRootL(id);// Keep database id as root of store
		iFileStore->CommitL();// Complete creation by commiting

		// Create Book tables and indexes
/*
		CreateBookMarksTableL();
		CreateBookMarksIndexL();

		CreateDownLoadHistoryTableL();

		CreateSystemParmTableL();
		CreateSystemParmIndexL();
*/
	);

	if(error!=KErrNone)
	{
		return error;
	}
	iOpen = ETrue;
	return KErrNone;
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::RemoveDb()
//
// First remove the Books table. Then remove the database file.
// ---------------------------------------------------------------------------
TInt CDatabaseEngine::RemoveDb(const TFileName& aExistingFile)
{
	Close();

	if(!BaflUtils::FileExists(iFsSession, aExistingFile))
	{
		return KErrNotFound;
	}

	// It is enough to delete the database file directly. Because this example
	// demonstrates DDL statements, it first opens and drops the Books table.

	TInt error = OpenDb(aExistingFile);
	if(error!=KErrNone)
	{
		return error;
	}

//	DropBookMarksTable();
	Close();

	iFsSession.Delete(aExistingFile);
	return KErrNone;
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::Close()
//
// Close the database.
// ---------------------------------------------------------------------------
TInt CDatabaseEngine::Close()
{
	iDb.Close();
	if(iFileStore)
	{
		delete iFileStore;
		iFileStore = NULL;
	}
	iOpen = EFalse;
	return KErrNone;
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::IsOpen()
//
// Return open status of the database.
// ---------------------------------------------------------------------------
TBool CDatabaseEngine::IsOpen() const
{
	return iOpen;
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::CreateBookMarksTableL()
//
// Creates Books table. Leaves, if the table cannot be created.
// ---------------------------------------------------------------------------
void CDatabaseEngine::CreateBookMarksTableL()
{
	// Specify columns for Books table
	TDbCol booknameCol(KBookNameCol, EDbColText);   // Using default length
	TDbCol lineCol(KBookMarkLineCol, EDbColText,10);
	booknameCol.iAttributes = TDbCol::ENotNull;
	lineCol.iAttributes = TDbCol::ENotNull;

	// Add the columns to column set
	CDbColSet* bookmarkColSet = CDbColSet::NewLC();
	bookmarkColSet->AddL(booknameCol);
	bookmarkColSet->AddL(lineCol);

	// Create the Books table
	User::LeaveIfError(iDb.CreateTable(KBookMarksTable,
		*bookmarkColSet));
	CleanupStack::PopAndDestroy(bookmarkColSet);
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::CreateBookMarksIndexL()
//
// Creates an index for Books table. Leaves, if the index cannot be created.
// ---------------------------------------------------------------------------
void CDatabaseEngine::CreateBookMarksIndexL()
{
	// Create index consisting of two columns
	TDbKeyCol booknameCol(KBookNameCol);

	CDbKey* index = CDbKey::NewLC();   // create index key set
	index->AddL(booknameCol);
	User::LeaveIfError(iDb.CreateIndex(
		KBooksIndexName, KBookMarksTable, *index));
	CleanupStack::PopAndDestroy(index);
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::DropBookMarksTable()
//
// Drop the Books table incrementally. Uses RDbIncremental and DDL statement.
// ---------------------------------------------------------------------------
void CDatabaseEngine::DropBookMarksTable()
{

	_LIT(KDropTable, "DROP TABLE ");

	// Sql: DROP TABLE Books
	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KDropTable);
	sqlStr.Append(KBookMarksTable);

	RDbIncremental incOp;
	TInt incStep = 0xFFFF;
	// Initialise Execution
	TInt incStat = incOp.Execute(iDb, sqlStr, incStep);
	while (incStep>0 && incStat==KErrNone)
	{
		incStat = incOp.Next(incStep); // Do the work
	}
	incOp.Close();
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::AddBookMarkWithSqlL()
//
// Add a book to database using RDbView and SQL
// ---------------------------------------------------------------------------
TInt CDatabaseEngine::AddBookMarkWithSql(const TDesC& aBookName,
										 const TInt aLine)
{

	if(aBookName.Length()==0)
	{
		return KErrGeneral;
	}

	_LIT(KSelect, "SELECT ");
	_LIT(KFrom, " FROM ");
	_LIT(KOrderBy, " ORDER BY ");
	_LIT(KDot, ", ");

	TBuf<10> nLine;
	nLine.Zero ();
	nLine.AppendNum (aLine);

	// Sql: SELECT Author, Title, Description FROM Books ORDER BY Title, Author
	TBuf<KCustomSqlMaxLength> sqlStr;

	sqlStr.Append(KSelect);
	sqlStr.Append(KBookNameCol);
	sqlStr.Append(KDot);
	sqlStr.Append(KBookMarkLineCol);
	sqlStr.Append(KFrom);
	sqlStr.Append(KBookMarksTable);
	sqlStr.Append(KOrderBy);
	sqlStr.Append(KBookMarkLineCol);
	sqlStr.Append(KDot);
	sqlStr.Append(KBookNameCol);

	RDbView view;    // Create a view on the database
	TInt error;
	error = view.Prepare(iDb, TDbQuery(sqlStr, EDbCompareFolded));
	if(error!=KErrNone)
	{
		return error;
	}
	error = view.EvaluateAll();
	if(error!=KErrNone)
	{
		return error;
	}

	TRAP(error,
		view.InsertL();  // Insert a row. Column order matches sql select statement
	view.SetColL(1, aBookName);
	view.SetColL(2, nLine);
	);
	if(error!=KErrNone)
	{
		return error;
	}

	TRAP(error, view.PutL()); // Complete insertion
	if(error!=KErrNone)
	{
		return error;
	} 
	view.Close();
	return KErrNone;
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::AddBookMarkWithCppApi()
//
// Add a book to database using RDbTable API
// ---------------------------------------------------------------------------
TInt CDatabaseEngine::AddBookMarkWithCppApi(const TDesC& aBookName,
											const TInt aLine)
{

	if(aBookName.Length()==0 )
	{
		return KErrGeneral;
	}

	TBuf<10> nLine;
	nLine.Zero ();
	nLine.AppendNum (aLine);

	// Create an updateable database table object
	RDbTable table;
	TInt err = table.Open(iDb, KBookMarksTable, table.EUpdatable);

	if(err!=KErrNone)
	{
		return err;
	}

	CDbColSet* booksColSet = table.ColSetL();
	CleanupStack::PushL(booksColSet);

	table.Reset();

	TRAPD(error,
		table.InsertL();
	table.SetColL(booksColSet->ColNo(KBookNameCol), aBookName); // col = 1
	table.SetColL(booksColSet->ColNo(KBookMarkLineCol), nLine);   // col = 2
	);

	if(error!=KErrNone)
	{
		return error;
	}   

	TRAP(err, table.PutL());    // Complete changes (the insertion)
	if(err!=KErrNone)
	{
		return err;
	}   

	CleanupStack::PopAndDestroy(booksColSet);
	table.Close();

	return KErrNone;

}

// ---------------------------------------------------------------------------
// CDatabaseEngine::GetBookMarksByKeyL()
//
// Get array of books from database according to column name and a search
// pattern. Format of each array item is:
//      <Author>|<Title>|<Description>
// ---------------------------------------------------------------------------
TInt CDatabaseEngine::GetBookMarksByKeyL(const TDesC& aColumnName,
										 const TDesC& aSearchString)
{
	TPtrC line;
	TInt RetLine;
	TBuf<KBookItemMaxLength> rowText;

	_LIT(KSelect, "SELECT ");
	_LIT(KFrom, " FROM ");
	_LIT(KWhere, " WHERE ");
	_LIT(KEqual, " = '");
	_LIT(KOrderBy, "' ORDER BY ");
	_LIT(KDot, ", ");
	// Sql: SELECT Author, Title, Description FROM Books
	//      WHERE "aColumnName LIKE aSearchString"
	//      ORDER BY Title, Author
	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KSelect);
	sqlStr.Append(KBookMarkLineCol);
	sqlStr.Append(KFrom);
	sqlStr.Append(KBookMarksTable);
	sqlStr.Append(KWhere);
	sqlStr.Append(aColumnName);
	sqlStr.Append(KEqual);
	sqlStr.Append(aSearchString);
	sqlStr.Append(_L("'"));
	//sqlStr.Append(KOrderBy);
	//sqlStr.Append(KBookNameCol);

	CDesCArrayFlat* resultArray =
		new (ELeave)CDesC16ArrayFlat(KArrayGranularity);
	CleanupStack::PushL(resultArray);

	// Create a view on the database
	RDbView view;
	User::LeaveIfError(
		view.Prepare(iDb, TDbQuery(sqlStr), view.EReadOnly));

	User::LeaveIfError(view.EvaluateAll());

	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL(colSet);

	// Append each result row to array
	if(view.FirstL()) 
	{
		view.GetL();

		line.Set(view.ColDes(colSet->ColNo(KBookMarkLineCol)));

		rowText.Zero ();
		rowText.Append (line);

		TLex temp(rowText);
		temp.Val( RetLine);
	}
	else
		RetLine=-1;

	CleanupStack::PopAndDestroy(colSet);
	view.Close();
	CleanupStack::Pop(resultArray);

	return RetLine;
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::RemoveBookMarks()
//
// Delete a book using title pattern and RDbUpdate (DML)
// ---------------------------------------------------------------------------
TInt CDatabaseEngine::RemoveBookMarks(const TDesC& aBookName, TInt& aResultCount)
{
	RDbUpdate updOp;

	_LIT(KDeleteFrom, "DELETE FROM ");
	_LIT(KWhere, " WHERE ");
	_LIT(KEqual, " = '");
	_LIT(KDot, "'");

	// Sql: DELETE FROM Books WHERE Title LIKE 'aTitle'
	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KDeleteFrom);
	sqlStr.Append(KBookMarksTable);
	sqlStr.Append(KWhere);
	sqlStr.Append(KBookNameCol);
	sqlStr.Append(KEqual);
	sqlStr.Append(aBookName);
	sqlStr.Append(KDot);

	// Initialize execution and perform the first step.
	// Note: Execute() returns 0 (=KErrNone), but it does not affect database
	//       until Next() is called.
	TInt incStat = updOp.Execute(iDb, sqlStr, EDbCompareFolded);
	incStat = updOp.Next(); // This will leave, if Execute() failed.

	while( incStat == 1 ) // Just in case, if the operation has more steps
	{
		incStat = updOp.Next();
	}
	aResultCount = updOp.RowCount();
	updOp.Close();
	return incStat; // KErrNone or system wide error code
}

// ---------------------------------------------------------------------------
// CDatabaseEngine::RemoveAllBookMarks()
//
// Delete books using asynchronous API. (RDbUpdate and DML)
// This implementation is still synchronous, because it uses
// User::WaitForRequest. Normally asynchronous functionality should be hidden
// into active object and client callback interfaces.
// ---------------------------------------------------------------------------
TInt CDatabaseEngine::RemoveAllBookMarks(TInt& aResultCount)
{
	_LIT(KDeleteFrom, "DELETE FROM ");

	// Sql: DELETE FROM Books
	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KDeleteFrom);
	sqlStr.Append(KBookMarksTable);

	RDbUpdate updOp;
	TRequestStatus incStat(1);
	TInt updStat = updOp.Execute(iDb, sqlStr, EDbCompareFolded);
	while (updStat==KErrNone && incStat ==1)
	{
		updOp.Next(incStat);           // Start async operation. It returns
		// immediately.
		User::WaitForRequest(incStat); // For simplicity wait completion here.
	}

	aResultCount = updOp.RowCount();
	updOp.Close();

	if(updStat!=KErrNone)
		return updStat;       // System wide error code
	else
		return incStat.Int(); // KErrNone or system wide error code
}


// ---------------------------------------------------------------------------
// CDatabaseEngine::UpdateBookMarkLine()
//
// Update book title using SQL UPDATE.
// ---------------------------------------------------------------------------
//
TInt CDatabaseEngine::UpdateBookMarkLine(const TDesC& aBookName,
										 const TInt aNewLine)
{
	_LIT(KSQLUpdateStart, "UPDATE BookMarks SET Line = '");
	_LIT(KSQLUpdateMiddle, "' WHERE BookName = '");
	_LIT(KSQLUpdateEnd, "'");

	TBuf<10> nLine;
	nLine.Zero ();
	nLine.AppendNum (aNewLine);

	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KSQLUpdateStart);
	sqlStr.Append(nLine);
	sqlStr.Append(KSQLUpdateMiddle);
	sqlStr.Append(aBookName);
	sqlStr.Append(KSQLUpdateEnd);

	TInt nRet;

	nRet=iDb.Execute(sqlStr);

	if(nRet)
		return KErrNone;
	else
		return KErrGeneral;
}

//=====================================================================================
//=========================DownLoadHistory Table=======================================
void CDatabaseEngine::CreateDownLoadHistoryTableL()
{
	// Specify columns for Books table
	TDbCol filenameCol(KDownLoadFileNameCol, EDbColText);   // Using default length
	TDbCol urlCol(KDownLoadURLCol, EDbColText,255);  
	TDbCol filesizeCol(KDownLoadFileSizeCol, EDbColText,20);
	TDbCol downloadtimeCol(KDownLoadTimeCol, EDbColText);

	filenameCol.iAttributes = TDbCol::ENotNull;
	urlCol.iAttributes = TDbCol::ENotNull;
	filesizeCol.iAttributes = TDbCol::ENotNull;
	downloadtimeCol.iAttributes = TDbCol::ENotNull;

	// Add the columns to column set
	CDbColSet* downloadhistoryColSet = CDbColSet::NewLC();
	downloadhistoryColSet ->AddL(filenameCol);
	downloadhistoryColSet ->AddL(urlCol);
	downloadhistoryColSet ->AddL(filesizeCol);
	downloadhistoryColSet ->AddL(downloadtimeCol);

	// Create the Books table
	User::LeaveIfError(iDb.CreateTable(KDownLoadHistoryTable,
		*downloadhistoryColSet ));
	CleanupStack::PopAndDestroy(downloadhistoryColSet);
}

void CDatabaseEngine::DropDownLoadHistoryTable()
{
	_LIT(KDropTable, "DROP TABLE ");

	// Sql: DROP TABLE Books
	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KDropTable);
	sqlStr.Append(KDownLoadHistoryTable);

	RDbIncremental incOp;
	TInt incStep = 0xFFFF;
	// Initialise Execution
	TInt incStat = incOp.Execute(iDb, sqlStr, incStep);
	while (incStep>0 && incStat==KErrNone)
	{
		incStat = incOp.Next(incStep); // Do the work
	}
	incOp.Close();
}

TInt CDatabaseEngine::AddDownLoadHistoryWithSql(const TDesC& aFileName,
												const TDesC& aURL,
												const TInt aFileSize)
{
	//::WriteLogsL(_L("AddDownLoadHistoryWithSql-1")); 
	if(aFileName.Length()==0 || aURL.Length()==0 )
	{
		return KErrGeneral;
	}

	//::WriteLogsL(_L("AddDownLoadHistoryWithSql-2")); 
	_LIT(KSelect, "SELECT ");
	_LIT(KFrom, " FROM ");
	_LIT(KOrderBy, " ORDER BY ");
	_LIT(KDot, ", ");

	TBuf<20> nFileSize;
	nFileSize.Zero ();
	nFileSize.AppendNum (aFileSize);

	TBuf<64> nTime;
	TTime	oTime;
	oTime.HomeTime();
	oTime.FormatL(nTime, _L("%D%M%Y%2/%3 %H:%T:%S "));

	//::WriteLogsL(_L("AddDownLoadHistoryWithSql-3")); 

	// Sql: SELECT Author, Title, Description FROM Books ORDER BY Title, Author
	TBuf<KCustomSqlMaxLength> sqlStr;

	sqlStr.Append(KSelect);
	sqlStr.Append(KDownLoadFileNameCol);
	sqlStr.Append(KDot);
	sqlStr.Append(KDownLoadURLCol);
	sqlStr.Append(KDot);
	sqlStr.Append(KDownLoadFileSizeCol);
	sqlStr.Append(KDot);
	sqlStr.Append(KDownLoadTimeCol);
	sqlStr.Append(KFrom);
	sqlStr.Append(KDownLoadHistoryTable);

	//::WriteLogsL(sqlStr); 
	//::WriteLogsL(_L("AddDownLoadHistoryWithSql-4")); 
	RDbView view;    // Create a view on the database
	TInt error;
	error = view.Prepare(iDb, TDbQuery(sqlStr, EDbCompareFolded));
	if(error!=KErrNone)
	{
		return error;
	}
	//::WriteLogsL(_L("AddDownLoadHistoryWithSql-5")); 
	error = view.EvaluateAll();
	if(error!=KErrNone)
	{
		return error;
	}

	//::WriteLogsL(_L("AddDownLoadHistoryWithSql-6")); 
	TRAP(error,
		view.InsertL();  // Insert a row. Column order matches sql select statement
	view.SetColL(1, aFileName);
	view.SetColL(2, aURL);
	view.SetColL(3, nFileSize);
	view.SetColL(4, nTime);
	);
	//::WriteLogsL(_L("AddDownLoadHistoryWithSql-7")); 
	if(error!=KErrNone)
	{
		return error;
	}

	//::WriteLogsL(_L("AddDownLoadHistoryWithSql-8")); 
	TRAP(error, view.PutL()); // Complete insertion
	//::WriteLogsL(_L("AddDownLoadHistoryWithSql-9")); 
	if(error!=KErrNone)
	{
		return error;
	} 
	view.Close();
	//::WriteLogsL(_L("AddDownLoadHistoryWithSql-10")); 
	return KErrNone;	
}

TInt CDatabaseEngine::RemoveAllDownLoadHistory(TInt& aResultCount)
{
	_LIT(KDeleteFrom, "DELETE FROM ");

	// Sql: DELETE FROM Books
	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KDeleteFrom);
	sqlStr.Append(KDownLoadHistoryTable);

	RDbUpdate updOp;
	TRequestStatus incStat(1);
	TInt updStat = updOp.Execute(iDb, sqlStr, EDbCompareFolded);
	while (updStat==KErrNone && incStat ==1)
	{
		updOp.Next(incStat);           // Start async operation. It returns
		// immediately.
		User::WaitForRequest(incStat); // For simplicity wait completion here.
	}

	aResultCount = updOp.RowCount();
	updOp.Close();

	if(updStat!=KErrNone)
		return updStat;       // System wide error code
	else
		return incStat.Int(); // KErrNone or system wide error code	
}

TInt CDatabaseEngine::RemoveDownLoadHistory(const TDesC& aFileName,const TDesC& aURL)
{
	_LIT(KDeleteFrom, "DELETE FROM ");
	_LIT(KWhere, " WHERE ");
	_LIT(KEqual, " = '");
	_LIT(KAnd, " AND ");

	// Sql: DELETE FROM Books
	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KDeleteFrom);
	sqlStr.Append(KDownLoadHistoryTable);
	sqlStr.Append(KWhere);
	sqlStr.Append(KDownLoadFileNameCol);
	sqlStr.Append(KEqual);
	sqlStr.Append(aFileName);
	sqlStr.Append(_L("'"));
	//sqlStr.Append(KAnd);
	//sqlStr.Append(KDownLoadURLCol);
	//sqlStr.Append(KEqual);
	//sqlStr.Append(aURL);
	//sqlStr.Append(_L("'"));

	RDbUpdate updOp;
	TRequestStatus incStat(1);
	TInt updStat = updOp.Execute(iDb, sqlStr, EDbCompareFolded);

	while (updStat==KErrNone && incStat ==1)
	{
		updOp.Next(incStat);           // Start async operation. It returns
		// immediately.
		User::WaitForRequest(incStat); // For simplicity wait completion here.
	}
	TInt aResultCount = updOp.RowCount();

	updOp.Close();

	if(updStat!=KErrNone)
	{
		return updStat;       // System wide error code
	}
	else
	{
		return incStat.Int(); // KErrNone or system wide error code	
	}
}

void CDatabaseEngine::GetAllDownLoadHistory(CArrayFixFlat<TINFOCONTENTTEXT>* aArray)
{
	TPtrC nFileName;
	TPtrC nURL;	
	TPtrC nFileSize;

	TBuf<KBookItemMaxLength> rowText;

	_LIT(KSelect, "SELECT ");
	_LIT(KFrom, " FROM ");
	_LIT(KWhere, " WHERE ");
	_LIT(KEqual, " = '");
	_LIT(KOrderBy, " ORDER BY ");
	_LIT(KDot, ", ");

	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KSelect);
	sqlStr.Append(KDownLoadFileNameCol);
	sqlStr.Append(KDot);
	sqlStr.Append(KDownLoadURLCol);
	sqlStr.Append(KDot);
	sqlStr.Append(KDownLoadFileSizeCol);
	sqlStr.Append(KDot);
	sqlStr.Append(KDownLoadTimeCol);
	sqlStr.Append(KFrom);
	sqlStr.Append(KDownLoadHistoryTable);
	sqlStr.Append(KOrderBy);
	sqlStr.Append(KDownLoadTimeCol);

	/*CArrayFixFlat<TINFOCONTENTTEXT>* resultArray =
	new (ELeave)CArrayFixFlat<TINFOCONTENTTEXT>(5);
	CleanupStack::PushL(resultArray);*/

	// Create a view on the database
	RDbView view;
	User::LeaveIfError(
		view.Prepare(iDb, TDbQuery(sqlStr), view.EReadOnly));

	User::LeaveIfError(view.EvaluateAll());

	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL(colSet);

	// Append each result row to array
	for (view.FirstL(); view.AtRow(); view.NextL())
	{
		view.GetL();

		nFileName.Set(view.ColDes(colSet->ColNo(KDownLoadFileNameCol)));
		nFileSize.Set(view.ColDes(colSet->ColNo(KDownLoadFileSizeCol)));
		nURL.Set(view.ColDes(colSet->ColNo(KDownLoadURLCol)));

		rowText.Zero ();
		rowText.Append (nFileName);
		rowText.Append (_L("*#"));
		rowText.Append (nFileSize);
		rowText.Append (_L("*#"));
		rowText.Append (nURL);

		aArray->AppendL (rowText);
	}
	CleanupStack::PopAndDestroy(colSet);
	view.Close();

	/*CleanupStack::PopAndDestroy(resultArray);

	return resultArray;*/
}

TInt CDatabaseEngine::CheckIsDownLoadHistoryByKeyL(const TDesC& aURLName)
{
	TInt RetNum;

	_LIT(KSelect, "SELECT ");
	_LIT(KFrom, " FROM ");
	_LIT(KWhere, " WHERE ");
	_LIT(KEqual, " = '");
	_LIT(KOrderBy, "' ORDER BY ");
	_LIT(KDot, ", ");
	// Sql: SELECT Author, Title, Description FROM Books
	//      WHERE "aColumnName LIKE aSearchString"
	//      ORDER BY Title, Author
	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KSelect);
	sqlStr.Append(KDownLoadFileNameCol);
	sqlStr.Append(KFrom);
	sqlStr.Append(KDownLoadHistoryTable);
	sqlStr.Append(KWhere);
	sqlStr.Append(KDownLoadURLCol);
	sqlStr.Append(KEqual);
	sqlStr.Append(aURLName);
	sqlStr.Append(_L("'"));

	// Create a view on the database
	RDbView view;
	User::LeaveIfError(
		view.Prepare(iDb, TDbQuery(sqlStr), view.EReadOnly));

	User::LeaveIfError(view.EvaluateAll());

	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL(colSet);
	//::WriteLogsL(_L("CheckIsDownLoadHistoryByKeyL-1")); 
	// Append each result row to array
	if(view.FirstL()) 
	{
		//::WriteLogsL(_L("CheckIsDownLoadHistoryByKeyL-2")); 
		RetNum=1;
	}
	else
	{
		//::WriteLogsL(_L("CheckIsDownLoadHistoryByKeyL-3")); 
		RetNum=0;
	}

	CleanupStack::PopAndDestroy(colSet);
	view.Close();

	return RetNum;
}
//========================================================================================================
//==================================SystemParm Table======================================================
void CDatabaseEngine::CreateSystemParmTableL()
{
	TDbCol parmnameCol(KParmNameCol, EDbColText);   // Using default length
	TDbCol parmvalueCol(KParmValueCol, EDbColText,255);  

	parmnameCol.iAttributes = TDbCol::ENotNull;
	parmvalueCol.iAttributes = TDbCol::ENotNull;

	CDbColSet* systemparmColSet = CDbColSet::NewLC();
	systemparmColSet->AddL(parmnameCol);
	systemparmColSet->AddL(parmvalueCol);

	User::LeaveIfError(iDb.CreateTable(KSystemParmTable,
		*systemparmColSet));
	CleanupStack::PopAndDestroy(systemparmColSet);
}

void CDatabaseEngine::CreateSystemParmIndexL()
{
	TDbKeyCol parmnameCol(KParmNameCol);

	CDbKey* index = CDbKey::NewLC();   // create index key set
	index->AddL(parmnameCol);
	User::LeaveIfError(iDb.CreateIndex(
		KSystemParmIndexName, KSystemParmTable, *index));
	CleanupStack::PopAndDestroy(index);
}

void CDatabaseEngine::DropSystemParmTable()
{
	_LIT(KDropTable, "DROP TABLE ");

	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KDropTable);
	sqlStr.Append(KSystemParmTable);

	RDbIncremental incOp;
	TInt incStep = 0xFFFF;
	// Initialise Execution
	TInt incStat = incOp.Execute(iDb, sqlStr, incStep);
	while (incStep>0 && incStat==KErrNone)
	{
		incStat = incOp.Next(incStep); // Do the work
	}
	incOp.Close();	
}

TInt CDatabaseEngine::SetSystemParmWithSQL(const TDesC& aParmName,
										   const TDesC& aParmValue)
{
	if(aParmName.Length()==0 || aParmValue.Length ()==0)
	{
		return KErrGeneral;
	}

	//::WriteLogsL(_L("SetSystemParmWithSQL-1")); 
	_LIT(KSelect, "SELECT ");
	_LIT(KFrom, " FROM ");
	_LIT(KDot, ", ");
	_LIT(KWhere, " WHERE ");
	_LIT(KEqual, " = '");

	TBuf<KCustomSqlMaxLength> sqlStr;

	sqlStr.Append(KSelect);
	sqlStr.Append(KParmNameCol);
	sqlStr.Append(KDot);
	sqlStr.Append(KParmValueCol);
	sqlStr.Append(KFrom);
	sqlStr.Append(KSystemParmTable);
	sqlStr.Append(KWhere);
	sqlStr.Append(KParmNameCol);
	sqlStr.Append(KEqual);
	sqlStr.Append(aParmName);
	sqlStr.Append (_L("'"));

	//::WriteLogsL(sqlStr); 
	//::WriteLogsL(_L("SetSystemParmWithSQL-2")); 
	RDbView view;    // Create a view on the database
	TInt error;
	error = view.Prepare(iDb, TDbQuery(sqlStr, EDbCompareFolded));
	if(error!=KErrNone)
	{
		return error;
	}
	error = view.EvaluateAll();
	if(error!=KErrNone)
	{
		return error;
	}

	//::WriteLogsL(_L("SetSystemParmWithSQL-3")); 
	TInt nCount=0;
	nCount=view.CountL();
	//::WriteLogsL(_L("SetSystemParmWithSQL-4")); 
	if(nCount==0)
	{
		//::WriteLogsL(_L("SetSystemParmWithSQL-5-1")); 
		TRAP(error,
			view.InsertL();  // Insert a row. Column order matches sql select statement
		view.SetColL(1, aParmName);
		view.SetColL(2, aParmValue);
		);
		if(error!=KErrNone)
		{
			return error;
		}

		TRAP(error, view.PutL()); // Complete insertion
		if(error!=KErrNone)
		{
			return error;
		} 
		view.Close();
		//::WriteLogsL(_L("SetSystemParmWithSQL-5-2")); 
		return KErrNone;
	}
	else
	{
		view.Close();

		//::WriteLogsL(_L("SetSystemParmWithSQL-6-1")); 
		_LIT(KSQLUpdateStart, "UPDATE SystemParm SET ParmValue = '");
		_LIT(KSQLUpdateMiddle, "' WHERE ParmName = '");
		_LIT(KSQLUpdateEnd, "'");

		TBuf<KCustomSqlMaxLength> sqlStr;
		sqlStr.Append(KSQLUpdateStart);
		sqlStr.Append(aParmValue);
		sqlStr.Append(KSQLUpdateMiddle);
		sqlStr.Append(aParmName);
		sqlStr.Append(KSQLUpdateEnd);
		//::WriteLogsL(sqlStr); 
		//::WriteLogsL(_L("SetSystemParmWithSQL-6-2")); 
		TInt nRet;

		nRet=iDb.Execute(sqlStr);
		//::WriteLogsL(_L("SetSystemParmWithSQL-6-3")); 
		if(nRet)
		{
			//::WriteLogsL(_L("SetSystemParmWithSQL-6-4")); 
			return KErrNone;
		}
		else
		{
			//::WriteLogsL(_L("SetSystemParmWithSQL-6-5")); 
			return KErrGeneral;
		}
	}	
}

TInt CDatabaseEngine::GetSystemParmWithSQL(const TDesC& aParmName,
										   TDes& aParmValue)
{
	TPtrC nParmValue;
	TInt nRet;

	_LIT(KSelect, "SELECT ");
	_LIT(KFrom, " FROM ");
	_LIT(KWhere, " WHERE ");
	_LIT(KEqual, " = '");

	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KSelect);
	sqlStr.Append(KParmValueCol);
	sqlStr.Append(KFrom);
	sqlStr.Append(KSystemParmTable);
	sqlStr.Append(KWhere);
	sqlStr.Append(KParmNameCol);
	sqlStr.Append(KEqual);
	sqlStr.Append(aParmName);
	sqlStr.Append(_L("'"));

	RDbView view;
	User::LeaveIfError(
		view.Prepare(iDb, TDbQuery(sqlStr), view.EReadOnly));

	User::LeaveIfError(view.EvaluateAll());

	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL(colSet);

	if(view.FirstL()) 
	{
		view.GetL();

		nParmValue.Set(view.ColDes(colSet->ColNo(KParmValueCol)));

		aParmValue.Zero ();
		aParmValue.Append (nParmValue);

		nRet=0;
	}
	else
	{
		aParmValue.Zero ();

		nRet= 1;
	}

	CleanupStack::PopAndDestroy(colSet);
	view.Close();

	return nRet;	
}