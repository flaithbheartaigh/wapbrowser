

#if !defined(AFX_DBENGINE_H__FA5B66FF_28EC_434D_818B_227CD790CDD3__INCLUDED_)
#define AFX_DBENGINE_H__FA5B66FF_28EC_434D_818B_227CD790CDD3__INCLUDED_


#include <e32std.h>
#include <badesca.h>    // CDesCArrayFlat (cannot be forward declarated)
#include <d32dbms.h>    // RDbStoreDatabase
#include <f32file.h>    // RFs

#include "CoCoPreDefine.h"


_LIT(KSelect,		"SELECT ");
_LIT(KFrom,			" FROM ");
_LIT(KWhere,		" WHERE ");
_LIT(KEqual,		" ='");
_LIT(KOrderBy,		"' ORDER BY ");
_LIT(KDot,			", ");
_LIT(KDeleteFrom,	"DELETE FROM ");
_LIT(KSQLQuote,		"'");
_LIT(KSQLUpdate,	"UPDATE ");
_LIT(KSQLSet,		" SET ");
_LIT(KSQLBlank,		" ");
_LIT(KAnd,			" And ");

// Constants
_LIT(KBookMarksTable, "BookMarks");                
_LIT(KBookNameCol, "BookName");           
_LIT(KBookMarkLineCol, "Line");             

_LIT(KBooksIndexName,"BooksIndex");        

//������ʷ��¼
_LIT(KDownLoadHistoryTable, "DownLoadHistory");                
_LIT(KDownLoadFileNameCol, "FileName");           
_LIT(KDownLoadURLCol, "URL");             
_LIT(KDownLoadFileSizeCol, "FileSize");             
_LIT(KDownLoadTimeCol, "DownLoadTime");             

//ϵͳ������
_LIT(KSystemParmTable, "SystemParm");                
_LIT(KParmNameCol, "ParmName");           
_LIT(KParmValueCol, "ParmValue");             

_LIT(KSystemParmIndexName,"ParmIndex");        

//ҳ�滺���¼
_LIT(KPageCacheTable,		"PageCache");
_LIT(KFirstPageCacheTable,	"FirstPageCache");

_LIT(KPageCacheFileName,	"FileName");
_LIT(KPageCacheFirstType,	"FirstTypeId");
_LIT(KPageCacheSecondType,	"SecondTypeId");
_LIT(KPageCacheThirdType,	"ThirdTypeId");
_LIT(KPageCachePageIndex,	"PageIndex");
_LIT(KPageCacheTimeStamp,	"TimeStamp");


const int KTitleMaxLength = 60;            
const int KDescriptionMaxLength = 128;     

const int KBookItemMaxLength = 256;

_LIT(KSeparator,"|");

class CFileStore;

class TPageRecord;

class CDBEngine : public CBase
    {
public: // Creation and destruction

    static CDBEngine* NewL();

    ~CDBEngine();


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

	TInt CheckIsDownLoadHistoryByKeyL(const TDesC& aKeyName,TInt aType=0);
	//SystemParm Table
	TInt SetSystemParmWithSQL(const TDesC& aParmName,
									const TDesC& aParmValue);

	TInt GetSystemParmWithSQL(const TDesC& aParmName,
									TDes& aParmValue);

	//////////////////////////////////////////////////////////////////////////
	//��¼ҳ�����ļ�
	//////////////////////////////////////////////////////////////////////////
	//û�м�¼����Ӽ�¼,���򷵻�ʧ��
	TBool AddPageCacheRecord(const TPageRecord& aRecord);
	//���¼�¼
	TBool UpdatePageCacheRecord(const TPageRecord& aRecord);
	//RemovePageCacheRecord
	//ɾ����TPageRecord�е�����ƥ��ļ�¼
	TBool RemovePageCacheRecord(const TPageRecord& aRecord);
	//ɾ��ĳ��Ŀ�µ����м�¼
	TBool RemovePageCacheRecordGroup(const TPageRecord& aRecord);
	//ɾ����ɵ�һ����¼,ͬʱ���ؼ�¼
	TBool RemoveOldestPageCacheRecord(TPageRecord& aRecord);
	//ɾ�����м�¼
	TBool RemoveAllPageCacheRecord();
	//GetPageCacheRecord
	//aCondition:��Ż�ȡ��¼������
	//aResult:��ȡ���ļ�¼
	TBool GetPageCacheRecord(const TPageRecord& aCondition,TPageRecord& aResult);
	//���ؼ�¼����
	TInt PageCacheRecordCount();
// 	TPageRecord GetOldestPageCacheRecord() const;

	//////////////////////////////////////////////////////////////////////////
	//����ҳ����,����������Ŀ�ĵ�һҳ
	//////////////////////////////////////////////////////////////////////////
	//AddFirstPageCacheRecord
	//û�м�¼����Ӽ�¼,�м�¼�����
	TBool AddFirstPageCacheRecord(const TPageRecord& aRecord);
	TBool RemoveFirstPageCacheRecord(const TPageRecord& aCondition);
	TBool GetFirstPageCacheRecord(const TPageRecord& aCondition,TPageRecord& aResult);

	//���¼�¼
	TBool UpdateFirstPageCacheRecord(const TPageRecord& aRecord);


private: // Construction
    void ConstructL();

    CDBEngine();

private: // Helpers

    void CreateBookMarksTableL();     /* Create the Books table                  */
    void CreateBookMarksIndexL();     /* Create the BooksIndex for Books table   */
    void DropBookMarksTable();       /* Drop Books table (DDL example)          */

	void CreateDownLoadHistoryTableL();     
    void DropDownLoadHistoryTable();

	void CreateSystemParmTableL();     /* Create the Books table                  */
    void CreateSystemParmIndexL();     /* Create the BooksIndex for Books table   */
    void DropSystemParmTable();       /* Drop Books table (DDL example)          */

	//����ҳ����
	void CreatePageCacheTableL();
	//void DropPageCacheTable();

	//����ҳ����,����������Ŀ�ĵ�һҳ
	void CreateFirstPageCacheTableL();
	
	//////////////////////////////////////////////////////////////////////////
	//test
	//////////////////////////////////////////////////////////////////////////
public:
	void	CreateTestTableL();
	TInt	AddTestItem(const TDesC& aParmName,const TDesC& aParmValue);

private: // Member data

    RFs              iFsSession;  /* For use of iFileStore                   */
    RDbStoreDatabase iDb;/* For database operations                 */
    CFileStore*      iFileStore;  /* For creating and opening database files */
    TBool            iOpen;       /* Flag indicating iBookDb open status*/
    };

#endif // __BOOKDBENGINE_H__


