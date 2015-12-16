
// CaptureToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaptureTool.h"
#include "CaptureToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CCaptureToolDlg dialog



CCaptureToolDlg::CCaptureToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCaptureToolDlg::IDD, pParent)
	, m_bIsCapturing(FALSE)
	, m_bIsResizing(FALSE)
	, m_bIsMoving(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCaptureToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CAPTURE, m_CapturePicBox);
}

BEGIN_MESSAGE_MAP(CCaptureToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCaptureToolDlg message handlers

BOOL CCaptureToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(0, 0, m_nScreenWidth, m_nScreenHeight, false);
	::MoveWindow(m_CapturePicBox, 0, 0, m_nScreenWidth, m_nScreenHeight, true);

	LONG styleValue = ::GetWindowLong(m_CapturePicBox, GWL_STYLE);
	styleValue |= SS_BITMAP | SS_CENTERIMAGE;
	SetWindowLong(m_CapturePicBox, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_Rect.SetRectEmpty();
	m_hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	m_hFont = CreateFont(14, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Consolas"));

	// Capture the full screen
	CaptureFullScreen();

	return TRUE;  // return TRUE  unless you set the focus to a control
}


BOOL CCaptureToolDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (IsWindow(m_hWnd) && (::IsDialogMessage(m_hWnd, pMsg) || ::IsChild(m_hWnd, pMsg->hwnd)))
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			::SendMessage(m_hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CCaptureToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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


BOOL CCaptureToolDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (ID_RBTNDOWN_SAVEAS == LOWORD(wParam))
	{
		// SaveTofile
		TCHAR path[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, path, MAX_PATH);
		CString szInitDir(path);
		szInitDir = szInitDir.Left(szInitDir.ReverseFind('\\') + 1);
		szInitDir.Append(_T("Images\\"));
		::CreateDirectory(szInitDir, NULL);

		GUID guid;
		CoCreateGuid(&guid);
		CString fileName;
		fileName.Format(_T("%d-%d-%d-%d"), guid.Data1, guid.Data2, guid.Data3, guid.Data4);

		CString filter = _T("BMP(*.bmp)|*.bmp|") \
			_T("JPEG(*.jpg;*.jpeg)|*.jpg;*.jpeg|") \
			_T("PNG(*.png)|*.png|") \
			_T("GIF(*.gif)|*.gif||");

		CFileDialog openFileDlg(FALSE, _T("bmp"), fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
		openFileDlg.GetOFN().lpstrInitialDir = szInitDir;
		INT_PTR result = openFileDlg.DoModal();
		if (IDOK == result)
		{
			CString filePath = openFileDlg.GetPathName();
			CaptureRect(TRUE, filePath);
			PostMessage(WM_CLOSE);
		}
	}

	return CDialogEx::OnCommand(wParam, lParam);
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCaptureToolDlg::OnPaint()
{
	//if (IsIconic())
	//{
	//	CPaintDC dc(this); // device context for painting

	//	SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

	//	// Center icon in client rectangle
	//	int cxIcon = GetSystemMetrics(SM_CXICON);
	//	int cyIcon = GetSystemMetrics(SM_CYICON);
	//	CRect rect;
	//	GetClientRect(&rect);
	//	int x = (rect.Width() - cxIcon + 1) / 2;
	//	int y = (rect.Height() - cyIcon + 1) / 2;

	//	// Draw the icon
	//	dc.DrawIcon(x, y, m_hIcon);
	//}
	//else
	//{
	//	CDialogEx::OnPaint();
	//}

	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint(m_CapturePicBox, &ps);
	HDC memdc = CreateCompatibleDC(hdc);
	::SelectObject(memdc, m_hBitmapScreen);
	::BitBlt(hdc, 0, 0, m_nScreenWidth, m_nScreenHeight, memdc, 0, 0, SRCCOPY);
	::SelectObject(hdc, m_hPen);
	::SelectObject(hdc, GetStockObject(NULL_BRUSH));
	::Rectangle(hdc, m_Rect.left, m_Rect.top, m_Rect.right, m_Rect.bottom);
	TCHAR text[16] = { 0 };
	wsprintf(text, _T("%dx%d"), m_Rect.right - m_Rect.left + 1, m_Rect.bottom - m_Rect.top + 1);
	::SetBkColor(hdc, RGB(0, 0, 0));
	::SetTextColor(hdc, RGB(255, 255, 255));
	::SelectObject(hdc, m_hFont);
	::TextOut(hdc, m_Rect.right + 2, m_Rect.bottom + 2, text, wcslen(text));
	::DeleteObject(hdc);
	::DeleteDC(memdc);
	::EndPaint(m_CapturePicBox, &ps);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCaptureToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCaptureToolDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_CursorState == STATE_OUTSIDE)
	{
		m_bIsCapturing = TRUE;
		::GetCursorPos(&m_ptLeftBtnDown);
	}
	else if (m_CursorState == STATE_INSIDE)
	{
		m_bIsMoving = TRUE;
		::GetCursorPos(&m_ptMoveBtnDown);
	}
	else
	{
		m_bIsResizing = TRUE;
		m_lLeft = m_Rect.left;
		m_lRight = m_Rect.right;
		m_lTop = m_Rect.top;
		m_lBottom = m_Rect.bottom;
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CCaptureToolDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bIsCapturing = FALSE;
	m_bIsMoving = FALSE;
	m_bIsResizing = FALSE;

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CCaptureToolDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_CursorState == STATE_INSIDE)
	{
		// Capture Picture and copy to the ClipBoard
		CaptureRect();

		// exit
		PostMessage(WM_CLOSE);
	}

	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CCaptureToolDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: Add your message handler code here
	if (m_CursorState == STATE_INSIDE)
	{
		CMenu menu;
		menu.LoadMenu(IDR_MENU_RBTNDOWN);
		CMenu* pSubMenu = menu.GetSubMenu(0);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);
		menu.Detach();
	}
}


void CCaptureToolDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bIsCapturing)
	{
		POINT curMousePoint;
		::GetCursorPos(&curMousePoint);

		m_Rect.left = min(curMousePoint.x, m_ptLeftBtnDown.x);
		m_Rect.top = min(curMousePoint.y, m_ptLeftBtnDown.y);
		m_Rect.right = max(curMousePoint.x, m_ptLeftBtnDown.x);
		m_Rect.bottom = max(curMousePoint.y, m_ptLeftBtnDown.y);

		InvalidateRect(NULL, false);
	}
	else if (m_bIsResizing)
	{
		POINT curMousePoint;
		::GetCursorPos(&curMousePoint);

		if (m_CursorState == STATE_TOP)
		{
			m_Rect.top = min(curMousePoint.y, m_lBottom);
			m_Rect.bottom = max(curMousePoint.y, m_lBottom);
		}
		else if (m_CursorState == STATE_BOTTOM)
		{
			m_Rect.top = min(curMousePoint.y, m_lTop);
			m_Rect.bottom = max(curMousePoint.y, m_lTop);
		}
		else if (m_CursorState == STATE_LEFT)
		{
			m_Rect.left = min(curMousePoint.x, m_lRight);
			m_Rect.right = max(curMousePoint.x, m_lRight);
		}
		else if (m_CursorState == STATE_RIGHT)
		{
			m_Rect.left = min(curMousePoint.x, m_lLeft);
			m_Rect.right = max(curMousePoint.x, m_lLeft);
		}
		else if (m_CursorState == STATE_TOP_LEFT)
		{
			m_Rect.left = min(curMousePoint.x, m_lRight);
			m_Rect.top = min(curMousePoint.y, m_lBottom);
			m_Rect.right = max(curMousePoint.x, m_lRight);
			m_Rect.bottom = max(curMousePoint.y, m_lBottom);
		}
		else if (m_CursorState == STATE_BOTTOM_RIGHT)
		{
			m_Rect.left = min(curMousePoint.x, m_lLeft);
			m_Rect.top = min(curMousePoint.y, m_lTop);
			m_Rect.right = max(curMousePoint.x, m_lLeft);
			m_Rect.bottom = max(curMousePoint.y, m_lTop);
		}
		else if (m_CursorState == STATE_TOP_RIGHT)
		{
			m_Rect.left = min(curMousePoint.x, m_lLeft);
			m_Rect.top = min(curMousePoint.y, m_lBottom);
			m_Rect.right = max(curMousePoint.x, m_lLeft);
			m_Rect.bottom = max(curMousePoint.y, m_lBottom);
		}
		else if (m_CursorState == STATE_BOTTOM_LEFT)
		{
			m_Rect.left = min(curMousePoint.x, m_lRight);
			m_Rect.top = min(curMousePoint.y, m_lTop);
			m_Rect.right = max(curMousePoint.x, m_lRight);
			m_Rect.bottom = max(curMousePoint.y, m_lTop);
		}

		InvalidateRect(NULL, false);
	}
	else if (m_bIsMoving)
	{
		POINT curMousePoint;
		::GetCursorPos(&curMousePoint);

		LONG dx = curMousePoint.x - m_ptMoveBtnDown.x;
		LONG dy = curMousePoint.y - m_ptMoveBtnDown.y;

		if (dx < 0 && m_Rect.left + dx < 0)
		{
			dx = -m_Rect.left;
		}
		else if (dx > 0 && m_Rect.right + dx > m_nScreenWidth)
		{
			dx = m_nScreenWidth - m_Rect.right;
		}
		if (dy < 0 && m_Rect.top + dy < 0)
		{
			dy = -m_Rect.top;
		}
		else if (dy > 0 && m_Rect.bottom + dy > m_nScreenHeight)
		{
			dy = m_nScreenHeight - m_Rect.bottom;
		}

		m_ptMoveBtnDown.x += dx;
		m_ptMoveBtnDown.y += dy;

		::OffsetRect(&m_Rect, dx, dy);

		InvalidateRect(NULL, false);
	}
	else
	{
		POINT curMousePoint;
		::GetCursorPos(&curMousePoint);
		SetMouseCursor(curMousePoint);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


void CCaptureToolDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch (nChar)
	{
	case VK_ESCAPE:
		PostMessage(WM_CLOSE);
		break;
	case VK_UP:
		if (!m_Rect.IsRectEmpty())
		{
			// control + shift + up
			if ((GetAsyncKeyState(VK_SHIFT) & 0x8000)
				&& (GetAsyncKeyState(VK_CONTROL) & 0x8000))
			{
				if (m_Rect.bottom > m_Rect.top + MOVEUNIT + 1)
				{
					m_Rect.bottom -= MOVEUNIT;
					InvalidateRect(NULL, false);
				}
				else if (m_Rect.bottom > m_Rect.top + 1)
				{
					m_Rect.bottom = m_Rect.top + 1;
					InvalidateRect(NULL, false);
				}
			}
			// shift + up
			else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				if (m_Rect.bottom > m_Rect.top + 1)
				{
					m_Rect.bottom -= 1;
					InvalidateRect(NULL, false);
				}
			}
			// control + up
			else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				if (m_Rect.top > MOVEUNIT)
				{
					m_Rect.top -= MOVEUNIT;
					m_Rect.bottom -= MOVEUNIT;
					InvalidateRect(NULL, false);
				}
				else if (m_Rect.top > 0)
				{
					m_Rect.bottom -= m_Rect.top;
					m_Rect.top -= m_Rect.top;
					InvalidateRect(NULL, false);
				}
			}
			// up
			else
			{
				if (m_Rect.top > 0)
				{
					m_Rect.top -= 1;
					m_Rect.bottom -= 1;
					InvalidateRect(NULL, false);
				}
			}
		}
		break;
	case VK_DOWN:
		if (!IsRectEmpty(&m_Rect))
		{
			// control + shift + down
			if ((GetAsyncKeyState(VK_SHIFT) & 0x8000)
				&& (GetAsyncKeyState(VK_CONTROL) & 0x8000))
			{
				if (m_Rect.bottom < m_nScreenHeight - MOVEUNIT)
				{
					m_Rect.bottom += MOVEUNIT;
					InvalidateRect(NULL, false);
				}
				else if (m_Rect.bottom < m_nScreenHeight)
				{
					m_Rect.bottom = m_nScreenHeight;
					InvalidateRect(NULL, false);
				}
			}
			// shift + down
			else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				if (m_Rect.bottom < m_nScreenHeight)
				{
					m_Rect.bottom += 1;
					InvalidateRect(NULL, false);
				}
			}
			// control + down
			else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				if (m_Rect.bottom < m_nScreenHeight - MOVEUNIT)
				{
					m_Rect.top += MOVEUNIT;
					m_Rect.bottom += MOVEUNIT;
					InvalidateRect(NULL, false);
				}
				else if (m_Rect.bottom < m_nScreenHeight)
				{
					m_Rect.top += m_nScreenHeight - m_Rect.bottom;
					m_Rect.bottom += m_nScreenHeight - m_Rect.bottom;
					InvalidateRect(NULL, false);
				}
			}
			// down
			else
			{
				if (m_Rect.bottom < m_nScreenHeight)
				{
					m_Rect.top += 1;
					m_Rect.bottom += 1;
					InvalidateRect(NULL, false);
				}
			}
		}
		break;
	case VK_LEFT:
		if (!IsRectEmpty(&m_Rect))
		{
			// control + shift + left
			if ((GetAsyncKeyState(VK_SHIFT) & 0x8000)
				&& (GetAsyncKeyState(VK_CONTROL) & 0x8000))
			{
				if (m_Rect.right > m_Rect.left + MOVEUNIT + 1)
				{
					m_Rect.right -= MOVEUNIT;
					InvalidateRect(NULL, false);
				}
				else if (m_Rect.right > m_Rect.left + 1)
				{
					m_Rect.right = m_Rect.left + 1;
					InvalidateRect(NULL, false);
				}
			}
			// shift + left
			else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				if (m_Rect.right > m_Rect.left + 1)
				{
					m_Rect.right -= 1;
					InvalidateRect(NULL, false);
				}
			}
			// control + left
			else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				if (m_Rect.left > MOVEUNIT)
				{
					m_Rect.left -= MOVEUNIT;
					m_Rect.right -= MOVEUNIT;
					InvalidateRect(NULL, false);
				}
				else if (m_Rect.left > 0)
				{
					m_Rect.right -= m_Rect.left;
					m_Rect.left -= m_Rect.left;
					InvalidateRect(NULL, false);
				}
			}
			// left
			else
			{
				if (m_Rect.left > 0)
				{
					m_Rect.left -= 1;
					m_Rect.right -= 1;
					InvalidateRect(NULL, false);
				}
			}
		}
		break;
	case VK_RIGHT:
		if (!IsRectEmpty(&m_Rect))
		{
			// control + shift + right
			if ((GetAsyncKeyState(VK_SHIFT) & 0x8000)
				&& (GetAsyncKeyState(VK_CONTROL) & 0x8000))
			{
				if (m_Rect.right < m_nScreenWidth - MOVEUNIT)
				{
					m_Rect.right += MOVEUNIT;
					InvalidateRect(NULL, false);
				}
				else if (m_Rect.right < m_nScreenWidth)
				{
					m_Rect.right = m_nScreenWidth;
					InvalidateRect(NULL, false);
				}
			}
			// shift + right
			else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				if (m_Rect.right < m_nScreenWidth)
				{
					m_Rect.right += 1;
					InvalidateRect(NULL, false);
				}
			}
			// control + right
			else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				if (m_Rect.right < m_nScreenWidth - MOVEUNIT)
				{
					m_Rect.left += MOVEUNIT;
					m_Rect.right += MOVEUNIT;
					InvalidateRect(NULL, false);
				}
				else if (m_Rect.right < m_nScreenWidth)
				{
					m_Rect.left += m_nScreenWidth - m_Rect.right;
					m_Rect.right += m_nScreenWidth - m_Rect.right;
					InvalidateRect(NULL, false);
				}
			}
			// right
			else
			{
				if (m_Rect.right < m_nScreenWidth)
				{
					m_Rect.left += 1;
					m_Rect.right += 1;
					InvalidateRect(NULL, false);
				}
			}
		}
		break;
	}

	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CCaptureToolDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnClose();
}


void CCaptureToolDlg::CaptureFullScreen()
{
	HWND hWnd = ::GetDesktopWindow();
	HDC hdcSrc = ::GetDC(hWnd);
	HDC hdcMem = ::CreateCompatibleDC(hdcSrc);
	m_hBitmapScreen = ::CreateCompatibleBitmap(hdcSrc, m_nScreenWidth, m_nScreenHeight);
	HGDIOBJ hOldbmp = ::SelectObject(hdcMem, m_hBitmapScreen);
	::BitBlt(hdcMem, 0, 0, m_nScreenWidth, m_nScreenHeight, hdcSrc, 0, 0, SRCCOPY);
	::SelectObject(hdcMem, hOldbmp);
	::DeleteObject(hdcMem);
	::ReleaseDC(hWnd, hdcSrc);
}


void CCaptureToolDlg::WriteBmpToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	HDC hDC = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	int iBits = ::GetDeviceCaps(hDC, BITSPIXEL) * ::GetDeviceCaps(hDC, PLANES);
	::DeleteDC(hDC);

	WORD   wBitCount;
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = iBits;

	DWORD   dwPaletteSize = 0;
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) *	sizeof(RGBQUAD);


	BITMAP  bm;
	::GetObject(hBitmap, sizeof(bm), (LPSTR)&bm);


	BITMAPINFOHEADER   bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmBitsSize = ((bm.bmWidth * wBitCount + 31) / 32) * 4 * bm.bmHeight;
	HANDLE hDib = ::GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	HANDLE hPal = ::GetStockObject(DEFAULT_PALETTE);
	HANDLE  hOldPal = NULL;
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	::GetDIBits(hDC, hBitmap, 0, (UINT)bm.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	BITMAPFILEHEADER   bmfHdr;
	bmfHdr.bfType = 0x4D42;
	DWORD dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;

	HANDLE hFile = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	DWORD dwWritten;
	WriteFile(hFile, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);

	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(hFile);
}


void CCaptureToolDlg::CopyBmpToClipBoard(HBITMAP hBitmap)
{
	::OpenClipboard(NULL);
	::EmptyClipboard();
	::SetClipboardData(CF_BITMAP, hBitmap);
	::CloseClipboard();
}


void CCaptureToolDlg::SetMouseCursor(POINT& curPos)
{
	if (PtInRect(&m_Rect, curPos))
	{
		// top-left
		if (curPos.x > m_Rect.left && curPos.x < m_Rect.left + PADDING
			&& curPos.y > m_Rect.top && curPos.y < m_Rect.top + PADDING)
		{
			SetClassLong(m_hWnd, GCLP_HCURSOR, (LONG)(LONG_PTR)LoadCursor(NULL, IDC_SIZENWSE));
			m_CursorState = STATE_TOP_LEFT;
		}
		// top-right
		else if (curPos.x <= m_Rect.right && curPos.x > m_Rect.right - PADDING
			&& curPos.y >= m_Rect.top && curPos.y < m_Rect.top + PADDING)
		{
			SetClassLong(m_hWnd, GCLP_HCURSOR, (LONG)(LONG_PTR)LoadCursor(NULL, IDC_SIZENESW));
			m_CursorState = STATE_TOP_RIGHT;
		}
		// bottom-right
		else if (curPos.x <= m_Rect.right && curPos.x > m_Rect.right - PADDING
			&& curPos.y <= m_Rect.bottom && curPos.y > m_Rect.bottom - PADDING)
		{
			SetClassLong(m_hWnd, GCLP_HCURSOR, (LONG)(LONG_PTR)LoadCursor(NULL, IDC_SIZENWSE));
			m_CursorState = STATE_BOTTOM_RIGHT;
		}
		// bottom-left
		else if (curPos.x >= m_Rect.left && curPos.x < m_Rect.left + PADDING
			&& curPos.y <= m_Rect.bottom && curPos.y > m_Rect.bottom - PADDING)
		{
			SetClassLong(m_hWnd, GCLP_HCURSOR, (LONG)(LONG_PTR)LoadCursor(NULL, IDC_SIZENESW));
			m_CursorState = STATE_BOTTOM_LEFT;
		}
		// top
		else if (curPos.x >= m_Rect.left + PADDING && curPos.x <= m_Rect.right - PADDING
			&& curPos.y >= m_Rect.top && curPos.y < m_Rect.top + PADDING)
		{
			SetClassLong(m_hWnd, GCLP_HCURSOR, (LONG)(LONG_PTR)LoadCursor(NULL, IDC_SIZENS));
			m_CursorState = STATE_TOP;
		}
		// bottom
		else if (curPos.x >= m_Rect.left + PADDING && curPos.x <= m_Rect.right - PADDING
			&& curPos.y > m_Rect.bottom - PADDING && curPos.y <= m_Rect.bottom)
		{
			SetClassLong(m_hWnd, GCLP_HCURSOR, (LONG)(LONG_PTR)LoadCursor(NULL, IDC_SIZENS));
			m_CursorState = STATE_BOTTOM;
		}
		// left
		else if (curPos.x >= m_Rect.left && curPos.x <= m_Rect.left + PADDING
			&& curPos.y >= m_Rect.top + PADDING && curPos.y <= m_Rect.bottom - PADDING)
		{
			SetClassLong(m_hWnd, GCLP_HCURSOR, (LONG)(LONG_PTR)LoadCursor(NULL, IDC_SIZEWE));
			m_CursorState = STATE_LEFT;
		}
		// right
		else if (curPos.x >= m_Rect.right - PADDING && curPos.x <= m_Rect.right
			&& curPos.y >= m_Rect.top + PADDING && curPos.y <= m_Rect.bottom - PADDING)
		{
			SetClassLong(m_hWnd, GCLP_HCURSOR, (LONG)(LONG_PTR)LoadCursor(NULL, IDC_SIZEWE));
			m_CursorState = STATE_RIGHT;
		}
		else
		{
			SetClassLong(m_hWnd, GCLP_HCURSOR, (LONG)(LONG_PTR)LoadCursor(NULL, IDC_SIZEALL));
			m_CursorState = STATE_INSIDE;
		}
	}
	else
	{
		SetClassLong(m_hWnd, GCLP_HCURSOR, (LONG)(LONG_PTR)LoadCursor(NULL, IDC_ARROW));
		m_CursorState = STATE_OUTSIDE;
	}
}


void CCaptureToolDlg::CaptureRect(BOOL bSave, LPCTSTR lpszFileName)
{
	int widht = m_Rect.Width();
	int height = m_Rect.Height();

	// Capture Picture
	HDC hdc = ::GetDC(m_CapturePicBox);
	HDC memdc = ::CreateCompatibleDC(hdc);
	::SelectObject(memdc, m_hBitmapScreen);

	HBITMAP hBitmap = CreateCompatibleBitmap(memdc, widht, height);

	HDC memdc_new = CreateCompatibleDC(hdc);
	::SelectObject(memdc_new, hBitmap);

	::BitBlt(memdc_new, 0, 0, widht, height, memdc, m_Rect.left, m_Rect.top, SRCCOPY);

	::DeleteObject(memdc_new);
	::DeleteObject(memdc);
	::ReleaseDC(m_CapturePicBox, hdc);

	// Save file
	if (bSave && NULL != lpszFileName)
	{
		WriteBmpToFile(hBitmap, lpszFileName);
	}
	// Copy to clipborad
	else
	{
		CopyBmpToClipBoard(hBitmap);
	}
}
