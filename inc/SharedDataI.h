#ifndef __SHAREDDATAI_H
#define __SHAREDDATAI_H
class CSharedDataI : public CBase 
{
public:
 IMPORT_C virtual  ~CSharedDataI(void);
 IMPORT_C void  AddCallBackL(class TCallBack const &, class TDesC16 const &);
 IMPORT_C void  AddNotify(class TDesC16 const &);
 IMPORT_C int  AddToValue(class TDesC16 const &, int &);
 IMPORT_C void  CancelSignal(class TDesC16 const &);
 IMPORT_C int  Get(class TDesC16 const &, int &);
 IMPORT_C int  Get(class TDesC16 const &, double &);
 IMPORT_C int  Get(class TDesC16 const &, class TDes16 &);
 IMPORT_C static class CSharedDataI *  NewL(class TUid const &, int);
 IMPORT_C int  Set(class TDesC16 const &, class TDesC16 const &);
 IMPORT_C int  Set(class TDesC16 const &, int &);
 IMPORT_C int  Set(class TDesC16 const &, double &);
 IMPORT_C int  Signal(class TDesC16 const &);
};
#endif 
//__SHAREDDATAI_H