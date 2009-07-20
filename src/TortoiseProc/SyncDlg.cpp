// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2009 - TortoiseGit

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

// SyncDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TortoiseProc.h"
#include "SyncDlg.h"


// CSyncDlg dialog

IMPLEMENT_DYNAMIC(CSyncDlg, CResizableStandAloneDialog)

CSyncDlg::CSyncDlg(CWnd* pParent /*=NULL*/)
	: CResizableStandAloneDialog(CSyncDlg::IDD, pParent)
	, m_bAutoLoadPuttyKey(FALSE)
{
	m_pTooltip=&this->m_tooltips;
	m_bInited=false;
}

CSyncDlg::~CSyncDlg()
{
}

void CSyncDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_PUTTY_KEY, m_bAutoLoadPuttyKey);
	DDX_Control(pDX, IDC_COMBOBOXEX_URL, m_ctrlURL);
	DDX_Control(pDX, IDC_BUTTON_TABCTRL, m_ctrlDumyButton);
	DDX_Control(pDX, IDC_BUTTON_PULL, m_ctrlPull);
	DDX_Control(pDX, IDC_BUTTON_PUSH, m_ctrlPush);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_ctrlStatus);
	DDX_Control(pDX, IDC_PROGRESS_SYNC, m_ctrlProgress);
	DDX_Control(pDX, IDC_ANIMATE_SYNC, m_ctrlAnimate);

	BRANCH_COMBOX_DDX;
}


BEGIN_MESSAGE_MAP(CSyncDlg, CResizableStandAloneDialog)
	ON_BN_CLICKED(IDC_BUTTON_PULL, &CSyncDlg::OnBnClickedButtonPull)
	ON_BN_CLICKED(IDC_BUTTON_PUSH, &CSyncDlg::OnBnClickedButtonPush)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CSyncDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_EMAIL, &CSyncDlg::OnBnClickedButtonEmail)
	ON_BN_CLICKED(IDC_BUTTON_MANAGE, &CSyncDlg::OnBnClickedButtonManage)
	BRANCH_COMBOX_EVENT
	ON_NOTIFY(CBEN_ENDEDIT, IDC_COMBOBOXEX_URL, &CSyncDlg::OnCbenEndeditComboboxexUrl)
	ON_CBN_EDITCHANGE(IDC_COMBOBOXEX_URL, &CSyncDlg::OnCbnEditchangeComboboxexUrl)
END_MESSAGE_MAP()


// CSyncDlg message handlers

void CSyncDlg::OnBnClickedButtonPull()
{
	// TODO: Add your control notification handler code here
	this->m_regPullButton =this->m_ctrlPull.GetCurrentEntry();
}

void CSyncDlg::OnBnClickedButtonPush()
{
	// TODO: Add your control notification handler code here
	this->m_regPushButton=this->m_ctrlPush.GetCurrentEntry();

}

void CSyncDlg::OnBnClickedButtonApply()
{
	// TODO: Add your control notification handler code here
}

void CSyncDlg::OnBnClickedButtonEmail()
{
	// TODO: Add your control notification handler code here
}

BOOL CSyncDlg::OnInitDialog()
{
	CResizableStandAloneDialog::OnInitDialog();

	//Create Tabctrl
	CWnd *pwnd=this->GetDlgItem(IDC_BUTTON_TABCTRL);
	CRect rectDummy;
	pwnd->GetWindowRect(&rectDummy);
	this->ScreenToClient(rectDummy);

	if (!m_ctrlTabCtrl.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, IDC_SYNC_TAB))
	{
		TRACE0("Failed to create output tab window\n");
		return FALSE;      // fail to create
	}
	m_ctrlTabCtrl.SetResizeMode(CMFCTabCtrl::RESIZE_NO);

	DWORD dwStyle =LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_OWNERDATA | WS_BORDER | WS_TABSTOP;

	if( !m_OutLogList.Create(dwStyle,rectDummy,&m_ctrlTabCtrl,IDC_OUT_LOGLIST))
	{
		TRACE0("Failed to create output commits window\n");
		return FALSE;      // fail to create

	}
	m_ctrlTabCtrl.InsertTab(&m_OutLogList,_T("Out Commits"),0);

	m_OutLogList.InsertGitColumn();

	dwStyle = LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP |LVS_SINGLESEL |WS_CHILD | WS_VISIBLE;
	
	if( !m_OutChangeFileList.Create(dwStyle,rectDummy,&m_ctrlTabCtrl,IDC_OUT_CHANGELIST))
	{
		TRACE0("Failed to create output change files window\n");
		return FALSE;      // fail to create
	}
	m_ctrlTabCtrl.InsertTab(&m_OutChangeFileList,_T("Out ChangeList"),1);

	m_OutChangeFileList.Init(SVNSLC_COLEXT | SVNSLC_COLSTATUS |SVNSLC_COLADD|SVNSLC_COLDEL , _T("RebaseDlg"),(SVNSLC_POPALL ^ SVNSLC_POPCOMMIT),false);

	this->m_tooltips.Create(this);

	AddAnchor(IDC_SYNC_TAB,TOP_LEFT,BOTTOM_RIGHT);

	AddAnchor(IDC_GROUP_INFO,TOP_LEFT,TOP_RIGHT);
	AddAnchor(IDC_COMBOBOXEX_URL,TOP_LEFT,TOP_RIGHT);
	AddAnchor(IDC_BUTTON_MANAGE,TOP_RIGHT);
	AddAnchor(IDC_BUTTON_PULL,BOTTOM_LEFT);
	AddAnchor(IDC_BUTTON_PUSH,BOTTOM_LEFT);
	AddAnchor(IDC_BUTTON_APPLY,BOTTOM_LEFT);
	AddAnchor(IDC_BUTTON_EMAIL,BOTTOM_LEFT);
	AddAnchor(IDC_PROGRESS_SYNC,BOTTOM_LEFT,BOTTOM_RIGHT);
	AddAnchor(IDOK,BOTTOM_RIGHT);
	AddAnchor(IDHELP,BOTTOM_RIGHT);
	AddAnchor(IDC_STATIC_STATUS,BOTTOM_LEFT);
	AddAnchor(IDC_ANIMATE_SYNC,TOP_RIGHT);
	
	BRANCH_COMBOX_ADD_ANCHOR();

	CString WorkingDir=g_Git.m_CurrentDir;
	WorkingDir.Replace(_T(':'),_T('_'));
	m_RegKeyRemoteBranch = CString(_T("Software\\TortoiseGit\\History\\SyncBranch\\"))+WorkingDir;


	this->AddOthersToAnchor();
	// TODO:  Add extra initialization here

	this->m_ctrlPush.AddEntry(CString(_T("Push")));
	this->m_ctrlPush.AddEntry(CString(_T("Push tags")));
	this->m_ctrlPush.AddEntry(CString(_T("Push All")));

	this->m_ctrlPull.AddEntry(CString(_T("&Pull")));
	this->m_ctrlPull.AddEntry(CString(_T("&Fetch")));
	this->m_ctrlPull.AddEntry(CString(_T("Fetch&&Rebase")));

	
	WorkingDir.Replace(_T(':'),_T('_'));

	CString regkey ;
	regkey.Format(_T("Software\\TortoiseGit\\TortoiseProc\\Sync\\%s"),WorkingDir);

	this->m_regPullButton = CRegDWORD(regkey+_T("\\Pull"),0);
	this->m_regPushButton = CRegDWORD(regkey+_T("\\Push"),0);

	this->m_ctrlPull.SetCurrentEntry(this->m_regPullButton);
	this->m_ctrlPush.SetCurrentEntry(this->m_regPushButton);

	CString str;
	this->GetWindowText(str);
	str += _T(" - ") + g_Git.m_CurrentDir;
	this->SetWindowText(str);

	EnableSaveRestore(_T("SyncDlg"));

	this->LoadBranchInfo();

	this->m_bInited=true;
	FetchOutList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSyncDlg::OnBnClickedButtonManage()
{
	// TODO: Add your control notification handler code here
	CAppUtils::LaunchRemoteSetting();
}

BOOL CSyncDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	m_tooltips.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}
void CSyncDlg::FetchOutList()
{
	if(!m_bInited)
		return;
	m_OutChangeFileList.Clear();
	this->m_OutLogList.Clear();

	CString remote;
	this->m_ctrlURL.GetWindowText(remote);
	CString remotebranch;
	this->m_ctrlRemoteBranch.GetWindowText(remotebranch);
	remotebranch=remote+_T("/")+remotebranch;

	if(IsURL())
	{
		CString str;
		str=_T("Don't know what will push befause you enter URL");
		m_OutLogList.ShowText(str);
		this->m_ctrlTabCtrl.ShowTab(m_OutChangeFileList.GetDlgCtrlID()-1,FALSE);
		m_OutLocalBranch.Empty();
		m_OutRemoteBranch.Empty();
		return ;
	
	}else if(g_Git.GetHash(remotebranch).GetLength()<40)
	{
		CString str;
		str.Format(_T("Don't know what will push befause unkown \"%s\""),remotebranch);
		m_OutLogList.ShowText(str);
		this->m_ctrlTabCtrl.ShowTab(m_OutChangeFileList.GetDlgCtrlID()-1,FALSE);
		m_OutLocalBranch.Empty();
		m_OutRemoteBranch.Empty();
		return ;
	}
	else
	{
		CString localbranch;
		localbranch=this->m_ctrlLocalBranch.GetString();

		if(localbranch != m_OutLocalBranch || m_OutRemoteBranch != remotebranch)
		{
			m_OutLogList.ClearText();
			m_OutLogList.FillGitLog(NULL,CGit::	LOG_INFO_STAT| CGit::LOG_INFO_FILESTATE | CGit::LOG_INFO_SHOW_MERGEDFILE,
				&remotebranch,&localbranch);
			
			CString str;
			if(m_OutLogList.GetItemCount() == 0)
			{			
				str.Format(_T("No commits ahead \"%s\""),remotebranch);
				m_OutLogList.ShowText(str);
				this->m_ctrlStatus.SetWindowText(str);
				this->m_ctrlTabCtrl.ShowTab(m_OutChangeFileList.GetDlgCtrlID()-1,FALSE);
			}
			else
			{
				str.Format(_T("%d commits ahead \"%s\""),m_OutLogList.GetItemCount(),remotebranch);
				this->m_ctrlStatus.SetWindowText(str);
				g_Git.GetCommitDiffList(localbranch,remotebranch,m_arOutChangeList);
				m_OutChangeFileList.Show(0,this->m_arOutChangeList);
				m_OutChangeFileList.SetEmptyString(CString(_T("No changed file")));
				this->m_ctrlTabCtrl.ShowTab(m_OutChangeFileList.GetDlgCtrlID()-1,TRUE);
			}
		}
		this->m_OutLocalBranch=localbranch;
		this->m_OutRemoteBranch=remotebranch;
	}

}

bool CSyncDlg::IsURL()
{
	CString str;
	this->m_ctrlURL.GetWindowText(str);
	if(str.Find(_T('\\'))>=0 || str.Find(_T('/'))>=0)
		return true;
	else
		return false;
}
void CSyncDlg::OnCbenEndeditComboboxexUrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CSyncDlg::OnCbnEditchangeComboboxexUrl()
{
	this->FetchOutList();
	// TODO: Add your control notification handler code here
}