
// Test_GDW_VPDCommDllDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Test_GDW_VPDCommDll.h"
#include "Test_GDW_VPDCommDllDlg.h"
#include "afxdialogex.h"
#include "../GDW_VPDCommDll/GDW_VPDCommDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef DEBUG
#pragma comment(lib, "../debug/GDW_VPDCommDll.lib")
#else
#pragma comment(lib, "../release/GDW_VPDCommDll.lib")
#endif

#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <string>

#define DATA_MESSAGE_NO 0x401
#define RE_CONNECT_MSG 0x402
#define DIS_CONNECT_MSG 0x403

#define BIN_IMG_SIZE 5*1024
#define PLATE_IMG_SIZE 5*1024
#define BIG_IMG_SIZE 100*1024

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTest_GDW_VPDCommDllDlg 对话框



CTest_GDW_VPDCommDllDlg::CTest_GDW_VPDCommDllDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTest_GDW_VPDCommDllDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTest_GDW_VPDCommDllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTest_GDW_VPDCommDllDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_Connet, &CTest_GDW_VPDCommDllDlg::OnBnClickedButtonConnet)
    ON_BN_CLICKED(IDC_BUTTON_SETIP, &CTest_GDW_VPDCommDllDlg::OnBnClickedButtonSetip)
    ON_BN_CLICKED(IDC_BUTTON_SetTime, &CTest_GDW_VPDCommDllDlg::OnBnClickedButtonSettime)
    ON_BN_CLICKED(IDC_BUTTON_Capture, &CTest_GDW_VPDCommDllDlg::OnBnClickedButtonCapture)
    ON_BN_CLICKED(IDC_BUTTON_GetInfo, &CTest_GDW_VPDCommDllDlg::OnBnClickedButtonGetinfo)
    ON_BN_CLICKED(IDC_BUTTON_getInfo2AD, &CTest_GDW_VPDCommDllDlg::OnBnClickedButtongetinfo2ad)
    ON_BN_CLICKED(IDC_BUTTON_DisCon, &CTest_GDW_VPDCommDllDlg::OnBnClickedButtonDiscon)
    ON_BN_CLICKED(IDC_RADIO1, &CTest_GDW_VPDCommDllDlg::OnBnClickedRadio1)
    ON_BN_CLICKED(IDC_RADIO2, &CTest_GDW_VPDCommDllDlg::OnBnClickedRadio2)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTest_GDW_VPDCommDllDlg 消息处理程序

BOOL CTest_GDW_VPDCommDllDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
    static time_t starttime = time(NULL);
    static DWORD starttick = GetTickCount();
    DWORD dwNowTick = GetTickCount() - starttick;
    time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
    //struct tm *pTM = localtime(&nowtime);
    struct tm pTM;
    localtime_s(&pTM, &nowtime);
    DWORD dwMS = dwNowTick % 1000;

    char chTimeInfo[128] = { 0 };
    sprintf_s(chTimeInfo, "%4d%02d%02d%02d%02d%02d", pTM.tm_year + 1900, pTM.tm_mon + 1, pTM.tm_mday, pTM.tm_hour, pTM.tm_min, pTM.tm_sec);
    GetDlgItem(IDC_EDIT1)->SetWindowText(chTimeInfo);

    CButton* radio = (CButton*)GetDlgItem(IDC_RADIO2);
    radio->SetCheck(1);
    m_bMsgEnable = true;

    m_uchBin = new unsigned char[BIN_IMG_SIZE];
    m_uchPlateImg = new unsigned char[PLATE_IMG_SIZE];
    m_uchCarImg = new unsigned char[BIG_IMG_SIZE];
    m_uchFarImg = new unsigned char[BIG_IMG_SIZE];

    memset(m_uchBin, 0, BIN_IMG_SIZE);
    memset(m_uchPlateImg, 0, PLATE_IMG_SIZE);
    memset(m_uchCarImg, 0, BIG_IMG_SIZE);
    memset(m_uchFarImg, 0, BIG_IMG_SIZE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTest_GDW_VPDCommDllDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTest_GDW_VPDCommDllDlg::OnPaint()
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
HCURSOR CTest_GDW_VPDCommDllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTest_GDW_VPDCommDllDlg::OnBnClickedButtonConnet()
{
    // TODO:  在此添加控件通知处理程序代码
    int iRet = GDW_Connect(this->m_hWnd, DATA_MESSAGE_NO, RE_CONNECT_MSG, DIS_CONNECT_MSG);
    //int iRet = GDW_Connect(this->GetSafeHwnd(), DATA_MESSAGE_NO, RE_CONNECT_MSG, DIS_CONNECT_MSG);
    char chLog[260] = { 0 };
    sprintf_s(chLog, "GDW_Connet 返回指为 %d", iRet);
    MessageBox(chLog);
}


void CTest_GDW_VPDCommDllDlg::OnBnClickedButtonSetip()
{
    // TODO:  在此添加控件通知处理程序代码
    // TODO: 在此添加控件通知处理程序代码
    char FileName[MAX_PATH];
    GetModuleFileNameA(NULL, FileName, MAX_PATH - 1);

    PathRemoveFileSpecA(FileName);
    char iniFileName[MAX_PATH] = { 0 };
    char iniDeviceInfoName[MAX_PATH] = { 0 };
    strcat_s(iniFileName, FileName);
    strcat_s(iniFileName, "\\XLW_VPDCommDll.ini");

    //写可靠性配置文件
    CString cstrIP;
    GetDlgItem(IDC_IPADDRESS1)->GetWindowText(cstrIP);

    char chTemp[256] = { 0 };
    sprintf_s(chTemp, "%s", cstrIP.GetBuffer());
    cstrIP.ReleaseBuffer();

    WritePrivateProfileStringA("Device", "IP", chTemp, iniFileName);
}


void CTest_GDW_VPDCommDllDlg::OnBnClickedButtonSettime()
{
    // TODO:  在此添加控件通知处理程序代码
    CString cstrTimeInfo;
    GetDlgItem(IDC_EDIT1)->GetWindowText(cstrTimeInfo);

    int iRet = GDW_AdjustTime(cstrTimeInfo.GetBuffer());
    cstrTimeInfo.ReleaseBuffer();

    char chLog[260] = { 0 };
    sprintf_s(chLog, "GDW_AdjustTime: %s  返回指为 %d", cstrTimeInfo.GetBuffer(), iRet);
    cstrTimeInfo.ReleaseBuffer();
    MessageBox(chLog);
}


void CTest_GDW_VPDCommDllDlg::OnBnClickedButtonCapture()
{
    // TODO:  在此添加控件通知处理程序代码
    int iRet = GDW_Capture();
    char chLog[260] = { 0 };
    sprintf_s(chLog, "GDW_Capture 返回指为 %d", iRet);
    MessageBox(chLog);
}


void CTest_GDW_VPDCommDllDlg::OnBnClickedButtonGetinfo()
{
    // TODO:  在此添加控件通知处理程序代码
    char chPlateNO[128] = { 0 };
    char chPlateTime[128] = { 0 };
    UINT dwPlateSize = 0, dwCarImgSize = 0, dwFarImgSize = 0;
    Dlg_WriteLog("call OnBnClickedButtonGetinfo");
    unsigned char* uchBin = new unsigned char[BIN_IMG_SIZE];
    unsigned char* uchPlateImg = new unsigned char[PLATE_IMG_SIZE];
    unsigned char* uchCarImg = new unsigned char[BIG_IMG_SIZE];
    unsigned char* uchFarImg = new unsigned char[BIG_IMG_SIZE];

    memset(uchBin, 0, BIN_IMG_SIZE);
    memset(uchPlateImg, 0, PLATE_IMG_SIZE);
    memset(uchCarImg, 0, BIG_IMG_SIZE);
    memset(uchFarImg, 0, BIG_IMG_SIZE);
    Dlg_WriteLog("call OnBnClickedButtonGetinfo ask for memery.");

    int iRet = GDW_GetVehicleInfo(chPlateNO, chPlateTime, uchBin,
        uchPlateImg, dwPlateSize,
        uchCarImg, dwCarImgSize,
        uchFarImg, dwFarImgSize);
    if (iRet == 0 || iRet == 1)
    {
        GetDlgItem(IDC_STATIC_PLATENO)->SetWindowText(chPlateNO);
        GetDlgItem(IDC_STATIC_PTIME)->SetWindowText(chPlateTime);
        char chFileName[MAX_PATH] = { 0 };
        //sprintf_s(chFileName, ".\\Result\\%ld-%s-%s", GetTickCount(), chPlateNO, chPlateTime);
        sprintf_s(chFileName, ".\\Result\\%ld-%s", GetTickCount(), chPlateTime);
        if (!strstr(chPlateNO, "无") && !strstr(chPlateNO, "*"))
        {
            std::string strPath = std::string(chFileName).append("-bin.bin");
            SaveImgToDisk(strPath.c_str(), uchBin, 280);
            GetDlgItem(IDC_STATIC_BINPATH)->SetWindowText(strPath.c_str());
        }
        if (1)
        {
            std::string strPath = std::string(chFileName).append(".txt");
            SaveImgToDisk(strPath.c_str(), (BYTE*)chPlateNO, strlen(chPlateNO));
        }
        if (dwPlateSize > 0)
        {
            std::string strPath = std::string(chFileName).append("-plate.jpg");
            SaveImgToDisk(strPath.c_str(), uchPlateImg, dwPlateSize);
            GetDlgItem(IDC_STATIC_PlatePath)->SetWindowText(strPath.c_str());
        }
        if (dwCarImgSize > 0)
        {
            std::string strPath = std::string(chFileName).append("-near.jpg");
            SaveImgToDisk(strPath.c_str(), uchCarImg, dwCarImgSize);
            GetDlgItem(IDC_STATIC_CARPATH)->SetWindowText(strPath.c_str());
        }
        if (dwFarImgSize > 0)
        {
            std::string strPath = std::string(chFileName).append("-far.jpg");
            SaveImgToDisk(strPath.c_str(), uchFarImg, dwFarImgSize);
            GetDlgItem(IDC_STATIC_FARPATH)->SetWindowText(strPath.c_str());
        }
    }

    if (uchBin)
    {
        delete[] uchBin;
        uchBin = NULL;
    }
    if (uchPlateImg)
    {
        delete[] uchPlateImg;
        uchPlateImg = NULL;
    }
    if (uchCarImg)
    {
        delete[] uchCarImg;
        uchCarImg = NULL;
    }
    if (uchFarImg)
    {
        delete[] uchFarImg;
        uchFarImg = NULL;
    }


    char chLog[260] = { 0 };
    sprintf_s(chLog, "GDW_GetVehicleInfo 返回值为 %d", iRet);
    Dlg_WriteLog(chLog);
    MessageBox(chLog);
}


void CTest_GDW_VPDCommDllDlg::OnBnClickedButtongetinfo2ad()
{
    // TODO:  在此添加控件通知处理程序代码
    char chPlateNO[128] = { 0 };
    char chPlateTime[128] = { 0 };
    UINT dwPlateSize = 0, dwCarImgSize = 0, dwFarImgSize = 0;
    Dlg_WriteLog("Call OnBnClickedButtongetinfo2ad .");
    unsigned char* uchBin = new unsigned char[BIN_IMG_SIZE];
    unsigned char* uchPlateImg = new unsigned char[PLATE_IMG_SIZE];
    unsigned char* uchCarImg = new unsigned char[BIG_IMG_SIZE];


    memset(uchBin, 0, BIN_IMG_SIZE);
    memset(uchPlateImg, 0, PLATE_IMG_SIZE);
    memset(uchCarImg, 0, BIG_IMG_SIZE);
    Dlg_WriteLog("ask for memery.");

    int iRet = GDW_GetVehicleInfo2AD(chPlateNO, chPlateTime, uchBin,
        uchPlateImg, dwPlateSize,
        uchCarImg, dwCarImgSize);

    if (iRet == 0 || iRet == 1)
    {
        GetDlgItem(IDC_STATIC_PLATENO)->SetWindowText(chPlateNO);
        GetDlgItem(IDC_STATIC_PTIME)->SetWindowText(chPlateTime);
        char chFileName[MAX_PATH] = { 0 };
        sprintf_s(chFileName, ".\\Result\\%ld-%s-%s", GetTickCount(), chPlateNO, chPlateTime);
        if (!strstr(chPlateNO, "无"))
        {
            std::string strPath = std::string(chFileName).append("-bin.bin");
            SaveImgToDisk(strPath.c_str(), uchBin, 280);
            GetDlgItem(IDC_STATIC_BINPATH)->SetWindowText(strPath.c_str());
        }
        if (1)
        {
            std::string strPath = std::string(chFileName).append(".txt");
            SaveImgToDisk(strPath.c_str(), (BYTE*)chPlateNO, strlen(chPlateNO));
        }
        if (dwPlateSize > 0)
        {
            std::string strPath = std::string(chFileName).append("-plate.jpg");
            SaveImgToDisk(strPath.c_str(), uchPlateImg, dwPlateSize);
            GetDlgItem(IDC_STATIC_PlatePath)->SetWindowText(strPath.c_str());
        }
        if (dwCarImgSize > 0)
        {
            std::string strPath = std::string(chFileName).append("-near.jpg");
            SaveImgToDisk(strPath.c_str(), uchCarImg, dwCarImgSize);
            GetDlgItem(IDC_STATIC_CARPATH)->SetWindowText(strPath.c_str());
        }
    }

    if (uchBin)
    {
        delete[] uchBin;
        uchBin = NULL;
    }
    if (uchPlateImg)
    {
        delete[] uchPlateImg;
        uchPlateImg = NULL;
    }
    if (uchCarImg)
    {
        delete[] uchCarImg;
        uchCarImg = NULL;
    }
    char chLog[260] = { 0 };
    sprintf_s(chLog, "GDW_GetVehicleInfo2AD 返回值为 %d", iRet);
    Dlg_WriteLog(chLog);
    MessageBox(chLog);
}


void CTest_GDW_VPDCommDllDlg::OnBnClickedButtonDiscon()
{
    // TODO:  在此添加控件通知处理程序代码
    int iRet = GDW_Disconnect();
    char chLog[260] = { 0 };
    sprintf_s(chLog, "GDW_Disconnect 返回指为 %d", iRet);
    MessageBox(chLog);
}

void CTest_GDW_VPDCommDllDlg::Dlg_WriteLog(const char* chLog)
{
    static time_t starttime = time(NULL);
    static DWORD starttick = GetTickCount();
    DWORD dwNowTick = GetTickCount() - starttick;
    time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
    //struct tm *pTM = localtime(&nowtime);
    struct tm pTM;
    localtime_s(&pTM, &nowtime);
    DWORD dwMS = dwNowTick % 1000;

    const int MAXPATH = 260;

    TCHAR szFileName[MAXPATH] = { 0 };
    GetModuleFileName(NULL, szFileName, MAXPATH);	//取得包括程序名的全路径
    PathRemoveFileSpec(szFileName);				//去掉程序名

    char chLogPath[MAXPATH] = { 0 };
    sprintf_s(chLogPath, "%s\\XLWLog\\%d-%02d-%02d\\", szFileName, pTM.tm_year + 1900, pTM.tm_mon + 1, pTM.tm_mday);
    MakeSureDirectoryPathExists(chLogPath);

    char chLogFileName[MAXPATH] = { 0 };
    sprintf_s(chLogFileName, "%s\\Dlg.log", chLogPath);

    FILE *file = NULL;
    //file = fopen(chLogFileName, "a+");
    fopen_s(&file, chLogFileName, "a+");
    if (file)
    {
        fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d:%03d : %s\n", pTM.tm_year + 1900, pTM.tm_mon + 1, pTM.tm_mday,
            pTM.tm_hour, pTM.tm_min, pTM.tm_sec, dwMS, chLog);
        fclose(file);
        file = NULL;
    }
}

bool CTest_GDW_VPDCommDllDlg::SaveImgToDisk(const char* chImgPath, BYTE* pImgData, DWORD dwImgSize)
{
    //WriteLog("begin SaveImgToDisk");
    if (NULL == pImgData)
    {
        //WriteLog("end1 SaveImgToDisk");
        return false;
    }
    bool bRet = false;
    size_t iWritedSpecialSize = 0;
    std::string tempFile(chImgPath);
    size_t iPosition = tempFile.rfind("\\");
    std::string tempDir = tempFile.substr(0, iPosition + 1);
    if (MakeSureDirectoryPathExists(tempDir.c_str()))
    {
        FILE* fp = NULL;
        //fp = fopen(chImgPath, "wb+");
        fopen_s(&fp, chImgPath, "wb+");
        if (fp)
        {
            //iWritedSpecialSize = fwrite(pImgData, dwImgSize , 1, fp);
            iWritedSpecialSize = fwrite(pImgData, 1, dwImgSize, fp);
            fclose(fp);
            fp = NULL;
            bRet = true;
        }
        if (iWritedSpecialSize == dwImgSize)
        {
            char chLogBuff[MAX_PATH] = { 0 };
            sprintf_s(chLogBuff, "%s save success\n", chImgPath);
            //WriteLog(chLogBuff);
            OutputDebugStringA(chLogBuff);
        }
    }
    else
    {
        char chLogBuff[MAX_PATH] = { 0 };
        sprintf_s(chLogBuff, "%s save failed\n", chImgPath);
        //WriteLog(chLogBuff);
        OutputDebugStringA(chLogBuff);
        bRet = false;
    }
    //WriteLog("end SaveImgToDisk");
    return bRet;
}


void CTest_GDW_VPDCommDllDlg::OnBnClickedRadio1()
{
    // TODO:  在此添加控件通知处理程序代码
    if (IsDlgButtonChecked(IDC_RADIO1))
    {
        m_bMsgEnable = false;
    }
}


void CTest_GDW_VPDCommDllDlg::OnBnClickedRadio2()
{
    // TODO:  在此添加控件通知处理程序代码
    if (IsDlgButtonChecked(IDC_RADIO2))
    {
        m_bMsgEnable = true;
    }
}


LRESULT CTest_GDW_VPDCommDllDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO:  在此添加专用代码和/或调用基类
    if (message == DATA_MESSAGE_NO)
    {
        OutputDebugStringA("Receive DATA_MESSAGE_NO\n");
        Dlg_WriteLog("Receive DATA_MESSAGE_NO.");
        if (m_bMsgEnable)
        {
            char chPlateNO[128] = { 0 };
            char chPlateTime[128] = { 0 };
            UINT dwPlateSize = 0, dwCarImgSize = 0, dwFarImgSize = 0;

            memset(m_uchBin, 0, BIN_IMG_SIZE);
            memset(m_uchPlateImg, 0, PLATE_IMG_SIZE);
            memset(m_uchCarImg, 0, BIG_IMG_SIZE);
            memset(m_uchFarImg, 0, BIG_IMG_SIZE);

            Dlg_WriteLog("Call GDW_GetVehicleInfo function.");

            int iRet = GDW_GetVehicleInfo(chPlateNO, chPlateTime, m_uchBin,
                m_uchPlateImg, dwPlateSize,
                m_uchCarImg, dwCarImgSize,
                m_uchFarImg, dwFarImgSize);

            Dlg_WriteLog("Finish call GDW_GetVehicleInfo function.");
            if (iRet == 0 || iRet == 1)
            {
                GetDlgItem(IDC_STATIC_PLATENO)->SetWindowText(chPlateNO);
                GetDlgItem(IDC_STATIC_PTIME)->SetWindowText(chPlateTime);
                char chFileName[MAX_PATH] = { 0 };
                //sprintf_s(chFileName, ".\\Result\\%ld-%s-%s", GetTickCount(), chPlateNO, chPlateTime);
                sprintf_s(chFileName, ".\\Result\\%ld-%s", GetTickCount(), chPlateTime);
                if (!strstr(chPlateNO, "无") && !strstr(chPlateNO, "*"))
                {
                    std::string strPath = std::string(chFileName).append("-bin.bin");
                    SaveImgToDisk(strPath.c_str(), m_uchBin, BIN_IMG_SIZE);
                    GetDlgItem(IDC_STATIC_BINPATH)->SetWindowText(strPath.c_str());
                }
                if (1)
                {
                    std::string strPath = std::string(chFileName).append(".txt");
                    SaveImgToDisk(strPath.c_str(), (BYTE*)chPlateNO, strlen(chPlateNO));
                }
                if (dwPlateSize > 0)
                {
                    std::string strPath = std::string(chFileName).append("-plate.jpg");
                    SaveImgToDisk(strPath.c_str(), m_uchPlateImg, dwPlateSize);
                    GetDlgItem(IDC_STATIC_PlatePath)->SetWindowText(strPath.c_str());
                }
                if (dwCarImgSize > 0)
                {
                    std::string strPath = std::string(chFileName).append("-near.jpg");
                    SaveImgToDisk(strPath.c_str(), m_uchCarImg, dwCarImgSize);
                    GetDlgItem(IDC_STATIC_CARPATH)->SetWindowText(strPath.c_str());
                }
                if (dwFarImgSize > 0)
                {
                    std::string strPath = std::string(chFileName).append("-far.jpg");
                    SaveImgToDisk(strPath.c_str(), m_uchFarImg, dwFarImgSize);
                    GetDlgItem(IDC_STATIC_FARPATH)->SetWindowText(strPath.c_str());
                }
            }
            char chLog[260] = { 0 };
            sprintf_s(chLog, "message 调用 GDW_GetVehicleInfo 返回值为 %d\n", iRet);
            OutputDebugStringA(chLog);
            Dlg_WriteLog(chLog);
        }
    }
    else if (message == RE_CONNECT_MSG)
    {
        OutputDebugStringA("Receive ReConnectMsg.\n");
        //GetDlgItem(IDC_BUTTON_Capture)->EnableWindow(TRUE);
        //GetDlgItem(IDC_BUTTON_GetInfo)->EnableWindow(TRUE);
        //GetDlgItem(IDC_BUTTON_getInfo2AD)->EnableWindow(TRUE);
        //GetDlgItem(IDC_BUTTON_SetTime)->EnableWindow(TRUE);

        GetDlgItem(IDC_STATIC_CONNectState)->SetWindowTextA("设备连接正常");
    }
    else if (message == DIS_CONNECT_MSG)
    {
        OutputDebugStringA("Receive DisConnectMsg.\n");
        GetDlgItem(IDC_STATIC_CONNectState)->SetWindowTextA("设备连接断开");

        //GetDlgItem(IDC_BUTTON_Capture)->EnableWindow(FALSE);
        //GetDlgItem(IDC_BUTTON_GetInfo)->EnableWindow(FALSE);
        //GetDlgItem(IDC_BUTTON_getInfo2AD)->EnableWindow(FALSE);
        //GetDlgItem(IDC_BUTTON_SetTime)->EnableWindow(FALSE);
    }

    return CDialogEx::DefWindowProc(message, wParam, lParam);
}


void CTest_GDW_VPDCommDllDlg::OnClose()
{
    // TODO:  在此添加消息处理程序代码和/或调用默认值
    if (m_uchBin)
    {
        delete[] m_uchBin;
        m_uchBin = NULL;
    }

    if (m_uchPlateImg)
    {
        delete[] m_uchPlateImg;
        m_uchPlateImg = NULL;
    }

    if (m_uchCarImg)
    {
        delete[] m_uchCarImg;
        m_uchCarImg = NULL;
    }

    if (m_uchFarImg)
    {
        delete[] m_uchFarImg;
        m_uchFarImg = NULL;
    }

    CDialogEx::OnClose();
}
