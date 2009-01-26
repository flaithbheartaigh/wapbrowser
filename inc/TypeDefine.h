/*
============================================================================
 Name        : TypeDefine.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : 预定义窗口类型枚举、控件类型枚举
============================================================================
*/

#ifndef TYPEDEFINE_H
#define TYPEDEFINE_H


enum TWindowType
{
	EMainWindow,
	EStartWindow,
	EEndWindow,
	EHelpWindow,
	EMusicPlayerWindow,
	EDownloadManagerWindow,
	EDownloadedWindow,
	EMainSearchView,
	ESearchResultView,
	ESearchLocalResultView,
	ESearchOutResultView,
	ERecommendWindow,
	EUserAdviseWindow,
	EResultWindow,//分类下载
	EnewDownloadView,//下载的结果界面
	EFriendLinkView,//友链
	ENewestdownView,//最新下载
	ESutraDownloadView,//精品下载.
	ESutraSearchView,//精品搜索界面

	EMainHelpWindow,//帮助主界面
	EDisplayHelpWindow,//显示具体的帮助信息

	ESelectBoxView,

	ESystemManageWindow,//系统管理界面//*start*//
	EDownloadHistorySettingWindow,//下载历史记录设置
	EDownloadDirSettingWindow,//下载保存目录设置
	EInfoDetailCacheSettingWindow,//信息详情缓存设置
	EContentListCacheSettingWindow,//内容列表缓存设置
	ECacheCleanWindow,//清除缓存设置
	EDownloadVoiceSettingWindow,//下载提示音设置
	EEnterModeWindow,//登陆方式
	EJoinAccountSettingWindow,//联网帐号设置
	EMultiThreadsDownloadSettingWindow,//多线程下载设置
	ESystemMessageWindow,//系统消息
	ETraficStatisticWindow,//联网流量统计

	EOpenFileWindow_Pic,
	EOpenFileWindow_Book,
	EOpenFileWindow_Music,
	EOpenFileWindow_Video
};

enum TNormalControlType
{
	EScrollBar,
	EPopUpMenu,
	ETitleBar,
	EDialog,
	EMultiText,
	EListBox,
	ESlideText,
	ENaviPane,
	ESelectBox,
	ESelectBoxGroup,
	EInputDialog,
	EInfoDialog,
	EFileBrowser,
	ECopyToDialog,
	EControlText,
	EGradeDialog,
	EContentInfoDialog,

	ESelectAppDialog,
	ELinkListBox,
	ESearchListBox
};

enum TExtentControlType
{
	EDownloadingList = 1000,
	EDownloadedList,
	EDownloadHistoryList,
	EFileAttribute
};
#endif // TYPEDEFINE_H

// End of File
