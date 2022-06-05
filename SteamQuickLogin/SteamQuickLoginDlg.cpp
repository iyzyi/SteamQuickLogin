
// SteamQuickLoginDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SteamQuickLogin.h"
#include "SteamQuickLoginDlg.h"
#include "afxdialogex.h"

#include <regex>
#include <fstream>
#include <string>
#include <iostream>
#include <tlhelp32.h>
#include <atlstr.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable: 4996)


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSteamQuickLoginDlg 对话框



CSteamQuickLoginDlg::CSteamQuickLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STEAMQUICKLOGIN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSteamQuickLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
}


// 解决回车键 ESC 默认关闭窗口
BOOL CSteamQuickLoginDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)     return   TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)   return   TRUE;
	else
		return   CDialog::PreTranslateMessage(pMsg);

}


BEGIN_MESSAGE_MAP(CSteamQuickLoginDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CSteamQuickLoginDlg::OnNMDblclkList1)
END_MESSAGE_MAP()


// CSteamQuickLoginDlg 消息处理程序

BOOL CSteamQuickLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

#ifdef _DEBUG
	// 开启控制台窗口
	AllocConsole();
	FILE* stream = nullptr;
	freopen_s(&stream, "CONOUT$", "w", stdout);
#endif

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 以下是List Control
	CString head[] = { TEXT("账号"), TEXT("密码"), TEXT("备注") };

	//插入列标题
	m_List.InsertColumn(0, head[0], LVCFMT_LEFT, 80);			// 仅用于创建本行，长度设为0，不在图像界面的列表中显示
	m_List.InsertColumn(1, head[1], LVCFMT_LEFT, 160);
	m_List.InsertColumn(2, head[2], LVCFMT_LEFT, 180);

	//设置风格样式
	//LVS_EX_GRIDLINES 网格
	//LVS_EX_FULLROWSELECT 选中整行
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()
		| LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 以上是List Control

	LoadUserInfo();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSteamQuickLoginDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSteamQuickLoginDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSteamQuickLoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void StringToWstring(std::wstring& szDst, std::string str)
{
	std::string temp = str;
	int len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)temp.c_str(), -1, NULL, 0);
	wchar_t* wszUtf8 = new wchar_t[len + 1];
	memset(wszUtf8, 0, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)temp.c_str(), -1, (LPWSTR)wszUtf8, len);
	szDst = wszUtf8;
	std::wstring r = wszUtf8;
	delete[] wszUtf8;
}


BOOL CSteamQuickLoginDlg::LoadUserInfo() {
	ifstream ifs("SteamData.ini");
	string sFileContent((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
	if (sFileContent.length() == 0) {
		std::cout << "打开SteamData.ini失败\n";
		return FALSE;
	}
	ifs.close();

	std::smatch m;
	auto pos = sFileContent.cbegin();
	auto end = sFileContent.cend();
	std::regex regPattern("\\[User\\]\\s*([^\\s]+?)\\r?\\n\\[Password\\]\\s*([^\\s]+?)\\r?\\n\\[Remark\\]\\s*([^\\s].+?)((\\r?\\n)|$)");
	
	for (; std::regex_search(pos, end, m, regPattern); pos = m.suffix().first)
	{
		DWORD dwInsertIndex = m_List.GetItemCount();
		
		CString* pcsPassword = new CString();

		LV_ITEM   lvitemData = { 0 };
		lvitemData.mask = LVIF_PARAM;
		lvitemData.iItem = dwInsertIndex;
		lvitemData.lParam = (LPARAM)(pcsPassword);		// 这里的指针可以用于保存密码
		m_List.InsertItem(&lvitemData);

		wstring wsUser;
		wstring wsPassword;
		wstring wsRemark;

		StringToWstring(wsUser, m.str(1));
		StringToWstring(wsPassword, m.str(2));
		StringToWstring(wsRemark, m.str(3));

		m_List.SetItemText(dwInsertIndex, 0, wsUser.c_str());
		m_List.SetItemText(dwInsertIndex, 1, L"************");
		m_List.SetItemText(dwInsertIndex, 2, wsRemark.c_str());

		*pcsPassword = wsPassword.c_str();
	}

	return true;
}




#define PROCESS_ID_LIST_NUMBER 128


// 检测有无正在运行的steam进程
DWORD GetActiveSteamProcessIDByName(PWCHAR wszName, PDWORD ProcessIdList)
{
	DWORD dwProcessIdNumbers = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe))
	{
		if (wcscmp(pe.szExeFile, wszName) == 0)
		{
			if ((pe.cntThreads) > 1) {		// 状态为已挂起的steam进程只有一个线程
				ProcessIdList[dwProcessIdNumbers] = pe.th32ProcessID;
				dwProcessIdNumbers++;
			}
		}
	}
	CloseHandle(hSnapshot);
	return dwProcessIdNumbers;
}


BOOL SteamLogin(PWCHAR wszUser, PWCHAR wszPassword) {
	WCHAR wszSteam[] = L"steam.exe";
	WCHAR wszSteamPath[MAX_PATH] = L"E:\\Program\\Steam\\Steam.exe";

	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	WCHAR wszCmdLine[MAX_PATH] = { 0 };

	// 检测有无正在运行的steam进程，若有则调用命令行关闭对应进程
	DWORD ProcessIdList[PROCESS_ID_LIST_NUMBER] = { 0 };
	DWORD dwProcessNum = GetActiveSteamProcessIDByName(wszSteam, ProcessIdList);
	if (dwProcessNum > 0) {
		printf("[INFO] 即将关闭正在运行的其他steam进程\n");

		ZeroMemory(wszCmdLine, MAX_PATH);
		wsprintf(wszCmdLine, L"%s -shutdown", wszSteamPath);

		BOOL bRet = CreateProcess(NULL, wszCmdLine, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
		if (!bRet) {
			printf("[ERROR] steam -shutdown 调用失败\n");
			return FALSE;
		}
		else {
			// 等待成功关闭其他steam进程
			printf("[INFO] 正在等待成功关闭其他steam进程");
			while (GetActiveSteamProcessIDByName(wszSteam, ProcessIdList)) {
				Sleep(200);
				printf(".");
			}
			printf("\n[INFO] 关闭其他steam进程成功\n");
		}
	}

	// 登录命令行
	ZeroMemory(wszCmdLine, MAX_PATH);
	wsprintf(wszCmdLine, L"%s -login %s %s", wszSteamPath, wszUser, wszPassword);
	BOOL bRet = CreateProcess(NULL, wszCmdLine, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
	if (!bRet) {
		printf("[ERROR] steam -login 调用失败\n");
		return FALSE;
	}
	else {
		printf("[INFO] steam -login 调用成功\n");
		return TRUE;
	}
}


// 双击则登录本行账号
void CSteamQuickLoginDlg::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	int iItem = pNMItemActivate->iItem;	// 选择的行号

	WCHAR wszUser[MAX_PATH];
	WCHAR wszPassword[MAX_PATH];
	CString* pcsPassword;

	m_List.GetItemText(iItem, 0, wszUser, MAX_PATH);

	LV_ITEM  lvitemData = { 0 };
	lvitemData.mask = LVIF_PARAM;
	lvitemData.iItem = iItem;
	m_List.GetItem(&lvitemData);
	pcsPassword = (CString*)lvitemData.lParam;
	wcscpy(wszPassword, pcsPassword->GetBuffer());

	SteamLogin(wszUser, wszPassword);

	*pResult = 0;
}