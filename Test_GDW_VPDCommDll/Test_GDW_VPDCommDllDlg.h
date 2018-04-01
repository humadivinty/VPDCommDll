
// Test_GDW_VPDCommDllDlg.h : ͷ�ļ�
//

#pragma once


// CTest_GDW_VPDCommDllDlg �Ի���
class CTest_GDW_VPDCommDllDlg : public CDialogEx
{
// ����
public:
	CTest_GDW_VPDCommDllDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TEST_GDW_VPDCOMMDLL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonConnet();
    afx_msg void OnBnClickedButtonSetip();
    afx_msg void OnBnClickedButtonSettime();
    afx_msg void OnBnClickedButtonCapture();
    afx_msg void OnBnClickedButtonGetinfo();
    afx_msg void OnBnClickedButtongetinfo2ad();
    afx_msg void OnBnClickedButtonDiscon();


private:
    unsigned char* m_uchBin;
    unsigned char* m_uchPlateImg;
    unsigned char* m_uchCarImg;
    unsigned char* m_uchFarImg;
    bool m_bMsgEnable;

    void Dlg_WriteLog(const char*);
    bool SaveImgToDisk(const char* chImgPath, BYTE* pImgData, DWORD dwImgSize);
public:
    afx_msg void OnBnClickedRadio1();
    afx_msg void OnBnClickedRadio2();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    afx_msg void OnClose();
};
