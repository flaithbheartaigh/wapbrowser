/*
============================================================================
 Name        : CoCoPreDefine.h
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 预定义的头文件
============================================================================
*/

#ifndef COCOPREDEFINE_H
#define COCOPREDEFINE_H

#define KWhiteRgb TRgb(255,255,255)
#define KBlackRgb TRgb(0,0,0)
//删除对象指针
#define DELETE(p) {if(p){delete p; p=NULL;}}
#define CLEAR(p) {p=NULL;}

//coco的mbm文件名称
_LIT(KICONFILENAME, "soarskyres.mbm");
//运行安装程序
_LIT(KRUNPROGRAMENAME,  "z:\\System\\apps\\appinst\\appinst.app");
//j2me程序安装路径
_LIT(KCMIDLETSAPPSPATH, "c:\\system\\midlets\\");
_LIT(KEMIDLETSAPPSPATH, "e:\\system\\midlets\\");
//symbian程序安装路径
_LIT(KCAPPSPATH, "c:\\system\\apps\\");
_LIT(KEAPPSPATH, "e:\\system\\apps\\");

//存放安装程序的文件记录
_LIT(KSETUPPROGRAMENAME,  "programs.txt");
//存放安装主题的文件记录
_LIT(KSETUPTOPICNAME,  "topics.txt");

//xml文件
_LIT(KTEMPXML,"\\readxml.xml");
//同步栏目文件
_LIT(KSYNXML,"\\system\\Apps\\coco\\res\\synxml.xml");

//程序安装目录
_LIT(KAPPPATH,	"\\system\\Apps\\coco\\");
//资源的目录
_LIT(KRESOURCEPATH,	"\\Resource\\Apps\\");
//视频屏幕尺寸
#define KWinHeight 36
#define KVideoRect TRect(0,KWinHeight,176,160+KWinHeight)
//mbm路径
#ifdef __SERIES60_3X__
	_LIT(KMBMPATH,"\\resource\\apps\\coco.mbm");
#else
	_LIT(KMBMPATH,"\\system\\apps\\coco\\coco.mbm");
#endif

//配色方案
#define KNaviPaneSelectedItemColor TRgb(9,216,215)
#define KNaviPaneBackgroundColor TRgb(46,61,90)

#define KPopMenuSelectedItemColor TRgb(13,146,150)
#define KPopMenuBackgroundColor TRgb(27,37,54)
#define KPopMenuRectColor TRgb(158,158,158)

#define KMainIconBackgroundColor TRgb(23,60,105)

#define KScrollBackgroundColor TRgb(68,80,103)
#define KScrollSpaceColor TRgb(16,188,255)

#define KListSelectedColor TRgb(64,61,90)

#define KTextColor TRgb(255,255,255)
#define KListTextColor TRgb(137,137,137)
#define KSelectedTextColor TRgb(0,231,231)
_LIT(KWAPHOMEPAGE,"http://wap.cocobox.cn");
//电子书编码格式
enum TBookEncoding
{
	EUNICODE,
	EUNICODEBIGENDIAN,
	EANSI,
	EUTF8,
	ENOTSUPPORTED
};
//联网接入点状态
enum TConState
{
	ECMNET,
	ECMWAP
};
//控制栏显示文字索引
enum TControlText
{
	ECurrentTime,
	EWeather,
	EDownLoading,
	ELoadType
};
//http联网逻辑处理
enum THttpHandleType
{
	EHttpGrade,
	EHttpSystemInfo,
	EHttpAutoUpdate,
	EHttpContentInfo,
	EHttpContentInfoImage,
	EHttpInSearch,
	ELocalSearch,
	EHttpLogin,
	EHttpExit,
	EHttpFirstContentUpdate,
	EHttpSecondContentUpdate,
	EHttpWeather,
	EHttpOutSearch,
	EHttpOutSearchError,
	EHttpOutSearchKeyWord,
	EHttpTempBuf

};
//文件类别文件夹名称
_LIT(KIMAGES,"images\\");
//_LIT(KIMAGES,"pictures\\");
_LIT(KMUSIC,"music\\");
_LIT(KAPPS,"apps\\");
_LIT(KVIDEO,"video\\");
_LIT(KBOOKS,"books\\");
_LIT(KTOPS,"topic\\");
_LIT(KOTHER,"other\\");

_LIT(KIMAGES1,"images");
//_LIT(KIMAGES1,"pictures");
_LIT(KMUSIC1,"music");
_LIT(KAPPS1,"apps");
_LIT(KVIDEO1,"video");
_LIT(KBOOKS1,"books");
_LIT(KTOPS1,"topic");
_LIT(KOTHER1,"other");

_LIT(KSYSINFO,"sysinfo");

//程序安装
_LIT(KINSTALL,"\\system\\inistall\\");

//http联网发送头
_LIT8(KUserAgent2, "Nokia6681/1.0 (3.08) Profile/MIDP-2.0 Configuration/CLDC-1.0");
_LIT8(KAccept2, "*/*");
_LIT8(KUserAgent, "SimpleClient 1.0");
_LIT8(KAccept, "*/*");
_LIT(KHttpPrefix, "http://");
_LIT8(KHttpPrefix8, "http://");
_LIT(KHttpsPrefix, "https://");
_LIT8(KHttpsPrefix8, "https://");
_LIT(KGoogle, "http://www.google.cn");
_LIT8(KProxy, "10.0.0.172:80");

//图片后缀名
_LIT(KBMP, "bmp");
_LIT(KJPG, "jpg");
_LIT(KGIF, "gif");
_LIT(KPNG, "png");
_LIT(KWBMP, "wbmp");
_LIT(KTIFF, "tiff");
_LIT(KEXIF, "exif");
//音乐后缀名
_LIT(KMP3, "mp3");
_LIT(KWMA, "wma");
_LIT(KMIDI, "midi");
_LIT(KMID, "mid");
_LIT(KWAV, "wav");
_LIT(KAAC, "aac");
_LIT(KOGG, "ogg");
_LIT(KMMF, "mmf");
_LIT(KIMY, "imy");
_LIT(KAMR, "amr");
//程序后缀名
_LIT(KSIS, "sis");
_LIT(KSISX, "sisx");
_LIT(KJAR, "jar");
_LIT(KJAD, "jad");
//视频后缀名
_LIT(K3GP, "3gp");
_LIT(K3GPP, "3gpp");
_LIT(KRM, "rm");
_LIT(KAVI, "avi");
_LIT(KMP4, "mp4");
_LIT(KMOV, "mov");
_LIT(KASF, "asf");
_LIT(KRMVB, "rmvb");
_LIT(KWMV, "wmv");
//电子书后缀名
_LIT(KTXT, "txt");
_LIT(KUMD, "umd");
_LIT(KPDB, "pdb");
_LIT(KPDF, "pdf");
//主题后缀名
_LIT(KTHM, "thm");
_LIT(KMTF, "mtf");
_LIT(KNTH, "nth");
_LIT(KSDT, "sdt");
_LIT(KHME, "hme");

//键盘键值预定义
#define	ScanCode_F1		(0xA4)	//left function key
#define	ScanCode_F2		(0xA5)	//right function key
#define	ScanCode_Enter	(0xA7)
#define	ScanCode_Cancel	(0x01)

#define	ScanCode_Dial	(0xC4)
#define	ScanCode_Hangup	(0xC5)

#define	ScanCode_Left	(0x0E)
#define	ScanCode_Right	(0x0F)
#define	ScanCode_Up		(0x10)
#define	ScanCode_Down	(0x11)

#define	ScanCode_Char0	(0x30)
#define	ScanCode_Char1	(0x31)
#define	ScanCode_Char2	(0x32)
#define	ScanCode_Char3	(0x33)
#define	ScanCode_Char4	(0x34)
#define	ScanCode_Char5	(0x35)
#define	ScanCode_Char6	(0x36)
#define	ScanCode_Char7	(0x37)
#define	ScanCode_Char8	(0x38)
#define	ScanCode_Char9	(0x39)

////#define	ScanCode_CharA	(0x85)	//*+
#define	ScanCode_CharA	(0x2A)
#define	ScanCode_CharB	(0x7F)	//#..

#define KKeyForegroundEvent 900 //前台
#define KKeyBackgroundEvent 901 //后台
#define KKeyPhoneIdleEvent 902 //电话呼入
#define KKeyPhoneIncomingEvent 903 //电话挂机

typedef struct tagPoint POINT;
struct tagPoint
{
	TInt x,y;
};

typedef struct tagRect RECT;
struct tagRect
{
	TInt nLeft,nTop,nWidth,nHeight;
};

#define SCREEN_WIDTH (176)
#define SCREEN_HEIGHT (208)

//文件属性
typedef struct tagFileAttribute FILEATTRIBUTE;
struct tagFileAttribute
{
	TFileName	m_szName;
	TFileName	m_szPath;
	TBuf<16>	m_szDate;
	TBuf<16>	m_szTime;
	TInt	m_nSize;
};

typedef TBuf<32> TTEXTRES;
typedef TBuf<100> TCONTENTTEXT;
typedef TBuf<200> TURLTEXT;

typedef TBuf<500> TINFOCONTENTTEXT;
typedef TBuf8<500> TINFOCONTENTTEXT8;

//联网返回信息状态
enum THttpResponseEvent
{
	EHttpNoError=0,
	EHttpConnectionError,
	EHttpErrorStatus,
	EHttpTimeOut,
	EHttpContentError
};
//文字信息索引
enum TTurkeyTextRes
{
	ECoCoVersionNumber,

	ETurkeyTextRes_Images,
	ETurkeyTextRes_Music,
	ETurkeyTextRes_Game,
	ETurkeyTextRes_Tool,
	ETurkeyTextRes_Video,

	ETurkeyTextRes_Book,
	ETurkeyTextRes_Topic,
	ETurkeyTextRes_Other,
	ETurkeyTextRes_Browse,
	ETurkeyTextRes_Downloading,
	ETurkeyTextRes_Exit,

	ETurkeyTextRes_Option,
	ETurkeyTextRes_Back,
	ETurkeyTextRes_Open,
	ETurkeyTextRes_Play,
	ETurkeyTextRes_Pause,
	ETurkeyTextRes_Stop,
	ETurkeyTextRes_Setup,

	ETurkeyTextRes_Run,
	ETurkeyTextRes_Setting,
	ETurkeyTextRes_Attribute,
	ETurkeyTextRes_Delete,
	ETurkeyTextRes_ReName,

	ETurkeyTextRes_SaveAs,
	ETurkeyTextRes_File,
	ETurkeyTextRes_Location,
	ETurkeyTextRes_Name,
	ETurkeyTextRes_Capacity,

	ETurkeyTextRes_Size,
	ETurkeyTextRes_Date,
	ETurkeyTextRes_Time,
	ETurkeyTextRes_Save,
    ETurkeyTextRes_View,

	ETurkeyTextRes_SetWallpaper,
	ETurkeyTextRes_Conneting,
	ETurkeyTextRes_Exiting,
	ETurkeyTextRes_LoadingTitle,
	ETurkeyTextRes_FileAttribute,

	ETurkeyTextRes_FileName,
	ETurkeyTextRes_FileType,
	ETurkeyTextRes_FileDate,
	ETurkeyTextRes_FileTime,
	ETurkeyTextRes_FileSize,
	
	ETurkeyTextRes_FileKB,
	ETurkeyTextRes_MainMenuHelp,
	ETurkeyTextRes_MainMenuAutoUpdate,

	ETurkeyTextRes_MainDown,
	ETurkeyTextRes_MainSearch,	
	ETurkeyTextRes_MainCode,
	ETurkeyTextRes_MainInfomation,
	ETurkeyTextRes_MainReward,
	ETurkeyTextRes_MainCommend,
	ETurkeyTextRes_MainFileMan,
	ETurkeyTextRes_MainProMan,
	ETurkeyTextRes_MainSystemSet,


	ETurkeyTextRes_Ok,

	ETurkeyTextRes_MenuDownloading,
	ETurkeyTextRes_MenuWaiting,
	ETurkeyTextRes_MainTopMan,
	ETurkeyTextRes_QDelFile,

	ETurkeyTextRes_DelFileFi,
	ETurkeyTextRes_SetWaFi,
	ETurkeyTextRes_SetWaLo,
	ETurkeyTextRes_SaveTo,
	ETurkeyTextRes_SaveSucess,
	ETurkeyTextRes_SaveFail,
	ETurkeyTextRes_QExit,
	ETurkeyTextRes_SaveData,
	ETurkeyTextRes_Cancel,

	//卞涛增加内容
	ETurkeyTextRes_Camera,
	ETurkeyTextRes_Send,

	ETurkeyTextRes_CameraScan,
	ETurkeyTextRes_SystemDoing,
	ETurkeyTextRes_CameraScanError,
	ETurkeyTextRes_CodeError,
	ETurkeyTextRes_ConnectionError,
	ETurkeyTextRes_CodeOK,
	ETurkeyTextRes_CameraScaning,

	ETurkeyTextRes_DownloadURL,

	ETurkeyTextRes_Top,
	ETurkeyTextRes_Hot,
	ETurkeyTextRes_New,
	ETurkeyTextRes_Type,
	ETurkeyTextRes_All,

	ETurkeyTextRes_StartPage,
	ETurkeyTextRes_UpPage,
	ETurkeyTextRes_DownPage,
	ETurkeyTextRes_EndPage,

	ETurkeyTextRes_Input,
	ETurkeyTextRes_Input1,
	ETurkeyTextRes_Input2,
	ETurkeyTextRes_Input3,
	ETurkeyTextRes_Input4,
	ETurkeyTextRes_Input5,

	ETurkeyTextRes_InputNull,
	ETurkeyTextRes_NumError,
	ETurkeyTextRes_ServerConnetting,
	ETurkeyTextRes_Left,
	ETurkeyTextRes_Right,
	//20070306张晖增加
	ETurkeyTextRes_MainMenuWapSite,
	ETurkeyTextRes_Upgrate,
	//20070313卞涛增加
	ETurkeyTextRes_Hidden,
	//卞涛增加推荐
	ETurkeyTextRes_Commend,
	ETurkeyTextRes_CommendSend,
	ETurkeyTextRes_CommendNull,
	ETurkeyTextRes_CommendMobi,

	//卞涛增加信息公告
	ETurkeyTextRes_ADInfo,
	ETurkeyTextRes_NextStep,

	//20070326张晖增加
	ETurkeyTextRes_DownLoadingList,
	ETurkeyTextRes_DownLoadedList,

	ETurkeyTextRes_DownLoadStatus_DownLoading,
	ETurkeyTextRes_DownLoadStatus_Waiting,
	ETurkeyTextRes_DownLoadStatus_Stoped,

	ETurkeyTextRes_DownLoadAddToList,
	ETurkeyTextRes_DownLoadAddFail,

	ETurkeyTextRes_DownLoadFinishNotice,

	ETurkeyTextRes_DownLoadList,

	//2007-03-30张晖增加下载列表菜单
	ETurkeyTextRes_DownLoadMenu_StopTask,
	ETurkeyTextRes_DownLoadMenu_BeginTask,
	ETurkeyTextRes_DownLoadMenu_DelTask,
	ETurkeyTextRes_DownLoadMenu_StopAllTask,
	ETurkeyTextRes_DownLoadMenu_BeginAllTask,
	ETurkeyTextRes_DownLoadMenu_DelAllTask,

	ETurkeyTextRes_DownLoadMenu_OpenFile,
	ETurkeyTextRes_DownLoadMenu_DelFromList,
	ETurkeyTextRes_DownLoadMenu_DelFromFile,
	ETurkeyTextRes_DownLoadMenu_DelAllFromList,
	ETurkeyTextRes_DownLoadMenu_DelAllFromFile,

	//2007-04-08张晖为音乐播放菜单增加
	ETurkeyTextRes_MusicMenu_SortPlay,
	ETurkeyTextRes_MusicMenu_RandomPlay,
	ETurkeyTextRes_MusicMenu_UpMusic,
	ETurkeyTextRes_MusicMenu_NextMusic,
	ETurkeyTextRes_MusicMenu_Pause,
	ETurkeyTextRes_MusicMenu_Play,
	ETurkeyTextRes_MusicMenu_Stop,
	ETurkeyTextRes_MusicMenu_NoVol,
	ETurkeyTextRes_MusicMenu_HaveVol,
	ETurkeyTextRes_MusicMenu_DownVol,
	ETurkeyTextRes_MusicMenu_UpVol,
	ETurkeyTextRes_MusicMenu_SetVoice,
	ETurkeyTextRes_MusicMenu_Close,

	ETurkeyTextRes_MusicTipInfo,

	//2007-04-10张晖为图片浏览菜单增加
	ETurkeyTextRes_PicMenu_UpPic,
	ETurkeyTextRes_PicMenu_NextPic,
	ETurkeyTextRes_PicMenu_SetWallPaper,
	ETurkeyTextRes_PicMenu_ScreenShow,
	ETurkeyTextRes_PicMenu_SetBig,
	ETurkeyTextRes_PicMenu_SetSmall,
	ETurkeyTextRes_PicMenu_ReName,

	ETurkeyTextRes_LeftArrow,
	ETurkeyTextRes_RightArrow,

	ETurkeyTextRes_NotSupported,
	ETurkeyTextRes_NoMemory,

	ETurkeyTextRes_InputCode,
	ETurkeyTextRes_InputKeyWord,

	ETurkeyTextRes_WapHomeText,
	ETurkeyTextRes_CommendFriend,

	ETurkeyTextRes_BookMark,
	ETurkeyTextRes_FriendLink,

	ETurkeyTextRes_ListNull,

	ETurkeyTextRes_FilePeople,
	ETurkeyTextRes_FileGrade,

	ETurkeyTextRes_SystemUpdating,

	ETurkeyTextRes_BookMarkAdd,
	ETurkeyTextRes_BookMarkRename,

	ETurkeyTextRes_IFBMDelete,
	ETurkeyTextRes_BMDeleteOK,

	ETurkeyTextRes_NoBMOperation,

	ETurkeyTextRes_BMAddCode,
	ETurkeyTextRes_BMAddInput,

	ETurkeyTextRes_BMAdd1,
	ETurkeyTextRes_BMAdd2,
	ETurkeyTextRes_BMAdd3,
	ETurkeyTextRes_BMAdd4,
	ETurkeyTextRes_BMAdd5,
	ETurkeyTextRes_BMAdd6,
	ETurkeyTextRes_BMAdd7,

	ETurkeyTextRes_BMAddName,
	ETurkeyTextRes_BMAddUrl,

	ETurkeyTextRes_NameNull,
	ETurkeyTextRes_UrlNull,

	ETurkeyTextRes_ResHistory,
	ETurkeyTextRes_ResTranslation,
	ETurkeyTextRes_ResJoke,
	ETurkeyTextRes_ResWeather,
	ETurkeyTextRes_ResMobileAddr,

	ETurkeyTextRes_WeatherSCity,
	ETurkeyTextRes_WeatherDCity,

	ETurkeyTextRes_ResTranslationInfo,
	ETurkeyTextRes_ResMobileAddrInfo,
	ETurkeyTextRes_ResLeftInput,

	ETurkeyTextRes_BookMarkBak,
	ETurkeyTextRes_BookMarkBakIsNot,

	//2007-04-25张晖为系统设置增加
	ETurkeyTextRes_Setting_Main,
	ETurkeyTextRes_Setting_PersonInfo,
	ETurkeyTextRes_Setting_Traffic,
	ETurkeyTextRes_Setting_SystemMsg,
	ETurkeyTextRes_Setting_Net,
	ETurkeyTextRes_Setting_DownLoadDir,
	ETurkeyTextRes_Setting_Login,
	ETurkeyTextRes_Setting_Update,
	ETurkeyTextRes_Setting_Help,

	ETurkeyTextRes_Setting_Login_OffLine,
	ETurkeyTextRes_Setting_Login_OnLine,

	ETurkeyTextRes_Setting_Change,
	ETurkeyTextRes_Setting_DownLoadDir_Current,
	ETurkeyTextRes_Setting_DownLoadDir_C,
	ETurkeyTextRes_Setting_DownLoadDir_E,
	ETurkeyTextRes_Setting_DownLoadDir_Select,
	ETurkeyTextRes_Setting_DownLoadDir_IsOK,

	ETurkeyTextRes_Setting_Net_IsOK,
	ETurkeyTextRes_Setting_Net_ChangeSucess,
	ETurkeyTextRes_Setting_Net_ChangeFail,

	//程序管理
	ETurkeyTextRes_Program_Running,
	ETurkeyTextRes_Program_Phone,
	ETurkeyTextRes_Program_Card,

	//程序管理-正在运行程序管理
	ETurkeyTextRes_Program_TaskMem,
	ETurkeyTextRes_Program_TaskInfo1,
	ETurkeyTextRes_Program_TaskInfo2,
	
	ETurkeyTextRes_Program_Menu_Refresh,
	ETurkeyTextRes_Program_Menu_SwitchTo,
	ETurkeyTextRes_Program_Menu_Close,
	ETurkeyTextRes_Program_Menu_CloseAll,

	ETurkeyTextRes_Program_Size,

	ETurkeyTextRes_Program_Menu_Info,
	ETurkeyTextRes_Program_Menu_Open,
	ETurkeyTextRes_Program_Menu_Unstall,

	ETurkeyTextRes_Program_AppInfo1,
	ETurkeyTextRes_Program_AppInfo2,

	ETurkeyTextRes_Program_IsUnstall,
	ETurkeyTextRes_Program_UnstallSucess,
	ETurkeyTextRes_Program_UnstallFail,
	ETurkeyTextRes_Program_DontUnstall,

	//程序管理-属性
	ETurkeyTextRes_Program_FName,
	ETurkeyTextRes_Program_FUID,
	ETurkeyTextRes_Program_FSize,
	ETurkeyTextRes_Program_FType,
	ETurkeyTextRes_Program_FPath,
	ETurkeyTextRes_Program_Symbian,
	ETurkeyTextRes_Program_Java,

	//离线在线
	ETurkeyTextRes_Online_IsOK,
	ETurkeyTextRes_OnlineStatus_Online,
	ETurkeyTextRes_OnlineStatus_Offline,
	ETurkeyTextRes_Setting_Login_SaveSuccess,

	//个人信息
	ETurkeyTextRes_PersonInfo_ID,
	ETurkeyTextRes_PersonInfo_NickName,
	ETurkeyTextRes_PersonInfo_Age,
	ETurkeyTextRes_PersonInfo_Sex,
	ETurkeyTextRes_PersonInfo_Area,
	ETurkeyTextRes_PersonInfo_Birthday,
	ETurkeyTextRes_PersonInfo_Level,
	ETurkeyTextRes_PersonInfo_Score,
	ETurkeyTextRes_PersonInfo_Help,
	ETurkeyTextRes_PersonInfo_Male,
	ETurkeyTextRes_PersonInfo_Female,
	ETurkeyTextRes_PersonInfo_NoID,

	ETurkeyTextRes_PersonInfo_InputNickName,
	ETurkeyTextRes_PersonInfo_InputNickNameHelp,
	ETurkeyTextRes_PersonInfo_InputAge,
	ETurkeyTextRes_PersonInfo_InputAgeHelp,
	ETurkeyTextRes_PersonInfo_InputBirthday,
	ETurkeyTextRes_PersonInfo_InputBirthdayHelp,
	ETurkeyTextRes_PersonInfo_InputArea,
	ETurkeyTextRes_PersonInfo_InputAreaHelp,

	ETurkeyTextRes_PersonInfo_SendToServerInfo,
	ETurkeyTextRes_PersonInfo_SaveSuccess,
	ETurkeyTextRes_PersonInfo_SaveFail,

	ETurkeyTextRes_System_Wait,

	ETurkeyTextRes_Res_ListNull,
	ETurkeyTextRes_Search_SDownLoad,
	ETurkeyTextRes_Search_GoPage,
	ETurkeyTextRes_Search_SHome,
	ETurkeyTextRes_Search_TurnPage,

	ETurkeyTextRes_Search_GoTone,
	ETurkeyTextRes_Search_10086,

	ETurkeyTextRes_Search_FreeUse,
	ETurkeyTextRes_Search_SorryNoFound,
	ETurkeyTextRes_Search_LeftTurnPage,
	ETurkeyTextRes_Search_NetErrorReturn,

	//20070530张晖增加,流量统计
	ETurkeyTextRes_Traffic_MonthRev,
	ETurkeyTextRes_Traffic_MonthSend,
	ETurkeyTextRes_Traffic_CurrentRev,
	ETurkeyTextRes_Traffic_CurrentSend,

	ETurkeyTextRes_DownLoadAddSuccess,

	ETurkeyTextRes_GoToPage,
	ETurkeyTextRes_GoToPage_Input,
	ETurkeyTextRes_GoToPage_InputHelp,
	ETurkeyTextRes_GoToPage_Err_OutRange,
	ETurkeyTextRes_GoToPage_Err_NotNum,

	ETurkeyTextRes_DownLoadAgainInfo,
	ETurkeyTextRes_URLInDownLoadList,

	ETurkeyTextRes_Traffic_AttentionInfo,

	ETurkeyTextRes_DownLoadFail_NoDiskSpace,

	ETurkeyTextRes_DownLoadStatus_ErrURLInfo,
	ETurkeyTextRes_DownLoadStatus_ErrURL,

	//	Added by xueyw on May 28 2007
	ETurkeyTextRes_FullScreen,
	ETurkeyTextRes_OpenFile,
	ETurkeyTextRes_OpenAsDefault,
	ETurkeyTextRes_OpenDirect,
	ETurkeyTextRes_OpenWith,
	ETurkeyTextRes_SelectApp,
	ETurkeyTextRes_SelectOtherApp,
	ETurkeyTextRes_IgnoreAppGroup1,
	ETurkeyTextRes_IgnoreAppGroup2,
	ETurkeyTextRes_IgnoreAppGroup3,
	ETurkeyTextRes_IgnoreAppGroup4,
	ETurkeyTextRes_IgnoreAppGroup5,
	ETurkeyTextRes_IgnoreAppGroup6,
	ETurkeyTextRes_IgnoreAppGroup7,
	ETurkeyTextRes_IgnoreAppGroup8,
	ETurkeyTextRes_IgnoreAppGroup9,
	ETurkeyTextRes_IgnoreAppGroup10,
	ETurkeyTextRes_IgnoreAppGroup11,
	ETurkeyTextRes_IgnoreAppGroup12,
	ETurkeyTextRes_IgnoreAppGroup13,
	ETurkeyTextRes_RecommenedAppGroup,

	//20070605张晖增加
	ETurkeyTextRes_FileTypeNotSupport,
	ETurkeyTextRes_DownLoad_FileTypeNotSupportInfo,
	ETurkeyTextRes_DownLoad_OpenFile,

	ETurkeyTextRes_Book_FirstPage,
	ETurkeyTextRes_Book_UpPage,
	ETurkeyTextRes_Book_NextPage,
	ETurkeyTextRes_Book_LastPage,

	//20070607张晖增加正在上传功能
	ETurkeyTextRes_UpLoadList,
	ETurkeyTextRes_UpLoadStatus_UpLoading,
	
	ETurkeyTextRes_UpLoadMenu_Del,
	ETurkeyTextRes_UpLoadMenu_DelAll,
	ETurkeyTextRes_UpLoadStatus_UpLoadSuccess,
	ETurkeyTextRes_UpLoadStatus_UpLoadFail,

	ETurkeyTextRes_Res_UpDate,

	//20070622张晖增加隐私声明（Symbian Sign需要）
	ETurkeyTextRes_Setting_SignSet,
	ETurkeyTextRes_Setting_PersonInfoNotice,
	ETurkeyTextRes_Setting_PersonInfoNotice_ShowInfo,
	ETurkeyTextRes_Setting_PersonInfoNotice_NotShowInfo,
	ETurkeyTextRes_Setting_FeeInfoNotice,
	ETurkeyTextRes_Setting_FeeInfoNotice_ShowInfo,
	ETurkeyTextRes_Setting_FeeInfoNotice_NotShowInfo,
	ETurkeyTextRes_Setting_SignSet_IsChange,
	ETurkeyTextRes_Setting_SignSet_ChangeSuccess,
	ETurkeyTextRes_Setting_SignSet_ChangeFail,

	ETurkeyTextRes_FeeP_Title1,
	ETurkeyTextRes_FeeP_Title2,
	ETurkeyTextRes_FeeP_Op1,
	ETurkeyTextRes_FeeP_Op2,

	ETurkeyTextRes_Camer_Err_Use,

	//20070627张晖增加
	ETurkeyTextRes_ContentDetail,
	ETurkeyTextRes_ContentDetail_NoDetail,

	ETurkeyTextRes_DialogInfo_Connecting,

	ETurkeyTextRes_BookMark_SetMark,
	ETurkeyTextRes_BookMark_ReturnMark,
	ETurkeyTextRes_BookMark_IsSet,
	ETurkeyTextRes_BookMark_SetSuccess,
	ETurkeyTextRes_BookMark_SetFail,

	ETurkeyTextRes_SaveAs_FileExist,
	ETurkeyTextRes_SaveAs_NoSpace,
	ETurkeyTextRes_SaveAs_NoSourceFile,

	ETurkeyTextRes_NewSearch_KeyWord,
	ETurkeyTextRes_NewSearch_In,
	ETurkeyTextRes_NewSearch_Out,
	ETurkeyTextRes_NewSearch_Local,

	ETurkeyTextRes_NewSearch_Type1,
	ETurkeyTextRes_NewSearch_Type2,
	ETurkeyTextRes_NewSearch_Type3,
	ETurkeyTextRes_NewSearch_Type4,
	ETurkeyTextRes_NewSearch_Type5,
	ETurkeyTextRes_NewSearch_Type6,
	ETurkeyTextRes_NewSearch_Type7,

	ETurkeyTextRes_NewSearch_IsCopy,
	ETurkeyTextRes_NewSearch_IsCut,

	ETurkeyTextRes_NewSearch_NoFound,

	ETurkeyTextRes_NewSearch_OperationOk,
	ETurkeyTextRes_NewSearch_OperationError,
	ETurkeyTextRes_NewSearch_SameFile,

	ETurkeyTextRes_NewSearch_SaveFile,

	ETurkeyTextRes_NewSearch_FileCopy,
	ETurkeyTextRes_NewSearch_FileCut,

	ETurkeyTextRes_NewSearch_FileSearching,

	ETurkeyTextRes_Main_UserCallBack,

	//20070704张晖增加
	ETurkeyTextRes_FileNotExist,
	ETurkeyTextRes_DownLoadAgain,

	ETurkeyTextRes_DownLoad_History,

	//20070712张晖增加下载线程数设置
	ETurkeyTextRes_DownLoadThreadSet,
	ETurkeyTextRes_DownLoadThreadSet_Info,
	ETurkeyTextRes_DownLoadThreadSet_Success,
	ETurkeyTextRes_DownLoadThreadSet_Fail,

	ETurkeyTextRes_DownLoadHistorySet,
	ETurkeyTextRes_DownLoadHistorySet_Info,
	ETurkeyTextRes_DownLoadHistorySet_Success,
	ETurkeyTextRes_DownLoadHistorySet_Fail,

	ETurkeyTextRes_DownLoadHistorySet1,

	ETurkeyTextRes_CheckUpdate,

	ETurkeyTextRes_Commend_Help1,
	ETurkeyTextRes_Commend_Help2,
	ETurkeyTextRes_Commend_Help3,

	ETurkeyTextRes_Commend_InputMobile,
	ETurkeyTextRes_Commend_OpenPhoneBook,
	ETurkeyTextRes_Commend_Commending,
	ETurkeyTextRes_Commend_CommendFive,

	ETurkeyTextRes_Res_HandleInput,
	ETurkeyTextRes_Res_HandleTitle,

	ETurkeyTextRes_Res_JokeTitle1,
	ETurkeyTextRes_Res_JokeTitle2,
	ETurkeyTextRes_Res_JokeTitle3,
	ETurkeyTextRes_Res_JokeTitle4,

	ETurkeyTextRes_Res_Video_FullScreenPlay,
	ETurkeyTextRes_Res_Video_NoVoice,
	ETurkeyTextRes_Res_Video_Voice,
	ETurkeyTextRes_Res_Video_Continue,
	ETurkeyTextRes_Res_Video_FullScreenContinue,

	//为文件管理目录功能增加
	ETurkeyTextRes_CreateFolder,
	ETurkeyTextRes_InputFolderName,
	ETurkeyTextRes_CreateFolderFailed,
	ETurkeyTextRes_CreateFolderNotSupported,
	ETurkeyTextRes_CopyFile,
	ETurkeyTextRes_FileCopied,
	ETurkeyTextRes_CopyFolderNotSupported,
	ETurkeyTextRes_PasteFile,
	ETurkeyTextRes_FilePasted,

	ETurkeyTextRes_BarCode_PhotographedCode,
	ETurkeyTextRes_BarCode_InputCode,

	ETurkeyTextRes_Info_SystemIn,
	ETurkeyTextRes_ContrlLate_Info,

	ETurkeyTextRes_FileMainTipInfo,
	
	ETurkeyTextRes_DownLoadUpdateFileSuccess,

	ETurkeyTextRes_Copy_NotSelect,
	ETurkeyTextRes_Copy_SameName,

	ETurkeyTextRes_DelFileFailed,
	ETurkeyTextRes_IsDelDir,
	ETurkeyTextRes_DirDelSuccess,
	ETurkeyTextRes_DirDelFailed,
	ETurkeyTextRes_DirReNameFailed,

	ETurkeyTextRes_Late_UpdateRes,
	ETurkeyTextRes_Late_UpdateErr,

	ETurkeyTextRes_Video_Paused,
	ETurkeyTextRes_Video_PlayError,

	ETurkeyTextRes_FilePaste_SourceNotExist,
	ETurkeyTextRes_FilePaste_OtherError,

	ETurkeyTextRes_File_OpenImageError,
	ETurkeyTextRes_Net_OutTime,

	ETurkeyTextRes_File_ViewFileAttrFailed,

	ETurkeyTextRes_HXML_UPage,
	ETurkeyTextRes_HXML_DPage,
	
	//主界面九宫格
	ECoCoTextRes_MainView_One,
	ECoCoTextRes_MainView_Two,
	ECoCoTextRes_MainView_Three,
	ECoCoTextRes_MainView_Four,
	ECoCoTextRes_MainView_Five,
	ECoCoTextRes_MainView_Six,
	ECoCoTextRes_MainView_Severn,
	ECoCoTextRes_MainView_Eight,
	ECoCoTextRes_MainView_Nine,
	ECoCoTextRes_MainView_Ten,

	//主界面菜单
	//打开-帮助-自动升级-意见反馈-WAP主页-系统消息-退出
	ECoCoTextRes_MainView_MenuOpen,
	ECoCoTextRes_MainView_MenuHelp,
	ECoCoTextRes_MainView_MenuAutoUpdate,
	ECoCoTextRes_MainView_MenuAdvice,
	ECoCoTextRes_MainView_MenuWapPage,
	ECoCoTextRes_MainView_MenuSysmtemInfo,
	ECoCoTextRes_MainView_MenuExit,


	
	ECoCoTextRes_FileBrowser_Sort,
	ECoCoTextRes_FileBrowser_SortByDate,
	ECoCoTextRes_FileBrowser_SortByName,



	ECoCoTextRes_DefaultAppName,

	ECoCoTextRes_CoCo_DownLoad,
	ECoCoTextRes_Web_Url,

	ECoCoTextRes_File_Name,
	ECoCoTextRes_File_Type,
	ECoCoTextRes_File_Size,
	ECoCoTextRes_File_Remark,

	ECoCoTextRes_List_PageNum,

	ECoCoTextRes_PopUpMenu_GoBackMainView,

	ECoCoTextRes_ControlText_DownLoad,

	ECoCoTextRes_OpenDownload,
	ECoCoTextRes_OpenFileManager,
	ECoCoTextRes_OpenMusicManager,

	ECoCoTextRes_GetSystemInfo,

	ECoCoTextRes_GetImage,
	ECoCoTestRes_LookWithContent,

	ECoCoTestRes_UserAdviseTip,
	ECoCoTestRes_UserAdviseInfo,

	ECoCoTestRes_UnKownError,
	ECoCoTestRes_UnKownContentError,
	ECoCoTestRes_ReadingListInfo,

	ECoCoTestRes_SelectFile,
	ECoCoTestRes_SelectFolder,

	ECoCoTestRes_IsSaveFolder,

	ECoCoTestRes_ErrorCode,
	ECoCoTestRes_NetError_IsRetry,

	//2007-10-30张晖为视频播放增加
	ECoCoTextRes_PlayVideo_Paused,
	ECoCoTextRes_PlayVideo_Stopped
	};
	enum ESystemManageRes
	{
		//为2.10版本：系统管理而新增加（王啸华）
		ESystemManageRes_SoftwareSetting,//软件设置
		ESystemManageRes_NetworkSetting,//网络设置
		ESystemManageRes_SystemInfo,//系统信息



		ESystemManageRes_DownloadHistory_Setting,
		ESystemManageRes_DownloadHistory_Setting0,
		ESystemManageRes_DownloadHistory_Setting1,
		ESystemManageRes_DownloadHistory_SettingSuccess,
		ESystemManageRes_DownloadHistory_SettingFail,

		ESystemManageRes_DownloadDir_Setting,
		ESystemManageRes_Current_Dir,
		ESystemManageRes_DownloadDir_C,
		ESystemManageRes_DownloadDir_E,
		ESystemManageRes_DownLoadDir_Select,
		ESystemManageRes_DownLoadDir_Makesure,
		ESystemManageRes_DownloadDir_SettingSuccess,
		ESystemManageRes_DownloadDir_SettingFail,

		ESystemManageRes_InfoDetailCacheSetting,
		ESystemManageRes_InfoDetailCacheSetting0,
		ESystemManageRes_InfoDetailCacheSetting1,
		ESystemManageRes_InfoDetailCacheSettingSuccess,
		ESystemManageRes_InfoDetailCacheSettingfail,

		ESystemManageRes_ContentListCacheSetting,
		ESystemManageRes_ContentListCacheSetting0,
		ESystemManageRes_ContentListCacheSetting1,
		ESystemManageRes_ContentListCacheSettingSuccess,
		ESystemManageRes_ContentListCacheSettingFail,

		ESystemManageRes_CacheClean,
		ESystemManageRes_CacheClean0,
		ESystemManageRes_CacheCleanKind1,
		ESystemManageRes_CacheCleanKind2,
		ESystemManageRes_CacheClean1,
		ESystemManageRes_CacheClean_Makesure,
		ESystemManageRes_CacheCleanSuccess,
		ESystemManageRes_CacheCleanFail,

		ESystemManageRes_Download_VoiceSetting,
		ESystemManageRes_Download_VoiceSetting0,
		ESystemManageRes_VoiceSetting_Yes,
		ESystemManageRes_VoiceSetting_No,
		ESystemManageRes_Download_VoiceSetting1,
		ESystemManageRes_Download_VoiceSettingSuccess,
		ESystemManageRes_Download_VoiceSettingFail,

		ESystemManageRes_EnterMode,
		ESystemManageRes_EnterMode0,
		ESystemManageRes_EnterMode_Kind1,
		ESystemManageRes_EnterMode_Kind2,
		ESystemManageRes_EnterMode1,
		ESystemManageRes_EnterMode_Success,
		ESystemManageRes_EnterMode_Fail,

		ESystemManageRes_JoinAccountSetting,
		ESystemManageRes_JoinAccountSetting0,
		ESystemManageRes_JoinAccountSetting1,
		ESystemManageRes_JoinAccountSettingSuccess,
		ESystemManageRes_JoinAccountSettingFail,

		ESystemManageRes_MultiThreadsDownloadSetting,
		ESystemManageRes_MultiThreadsDownloadSetting0,
		ESystemManageRes_MultiThreadsDownloadSetting1,
		ESystemManageRes_MultiThreadsDownloadSettingSuccess,
		ESystemManageRes_MultiThreadsDownloadSettingFail,

		ESystemManageRes_SystemMessage,
		ESystemManageRes_Ok,
		ESystemManageRes_Delete,
		ESystemManageRes_DeleteAll,
		ESystemManageRes_Delete_Inquire,
		ESystemManageRes_DeleteAll_Inquire,

		ESystemManageRes_Connecttion_Traffic,//联网流量
		ESystemManageRes_Traffic_MonthRev,
		ESystemManageRes_Traffic_MonthSend,
		ESystemManageRes_Traffic_CurrentRev,
		ESystemManageRes_Traffic_CurrentSend,
		ESystemManageRes_Wait
	};
#endif // COCOPREDEFINE_H

// End of File
