
// CaptureToolDlg.h : header file
//

#pragma once
#include "afxwin.h"


// const value
#define PADDING	8
#define MOVEUNIT 10

// mouse cursor pos
enum CURSOR_STATE
{
	STATE_OUTSIDE = 0,
	STATE_INSIDE,
	STATE_TOP,
	STATE_BOTTOM,
	STATE_LEFT,
	STATE_RIGHT,
	STATE_TOP_LEFT,
	STATE_TOP_RIGHT,
	STATE_BOTTOM_LEFT,
	STATE_BOTTOM_RIGHT
};


// CCaptureToolDlg dialog
class CCaptureToolDlg : public CDialogEx
{
// Construction
public:
	CCaptureToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CAPTURETOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()


private:
	void CaptureFullScreen();
	void WriteBmpToFile(HBITMAP hBitmap, LPCTSTR lpszFileName);
	void CopyBmpToClipBoard(HBITMAP hBitmap);
	void SetMouseCursor(POINT& curPos);
	void CaptureRect(BOOL bSave = FALSE, LPCTSTR lpszFileName = NULL);

private:
	CStatic m_CapturePicBox;

private:
	CRect m_Rect;
	CURSOR_STATE m_CursorState;
	HBITMAP m_hBitmapScreen;
	HPEN m_hPen;
	HFONT m_hFont;
	int m_nScreenWidth;
	int m_nScreenHeight;

	// status
private:
	BOOL m_bIsCapturing;
	BOOL m_bIsResizing;
	BOOL m_bIsMoving;

	// Point
private:
	POINT m_ptLeftBtnDown;
	POINT m_ptMoveBtnDown;
	LONG m_lLeft;
	LONG m_lRight;
	LONG m_lTop;
	LONG m_lBottom;
};
