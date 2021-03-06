
// WeChatView.cpp : CWeChatView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "WeChat.h"
#endif

#include "WeChatDoc.h"
#include "WeChatView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const CSize scrollPageSize = { 40,40 };
const CSize scrollLineSize = { 40,40 };

CString g_str_data = L"在给定的矩形内调用该成员函数格式化文本。";

CString g_str_data2 = L"在给定的矩形内调用该成员函数格式化文本。\n\
		通过将制表值扩展到适当大小，使文本在给定矩形内左对齐、右对齐\
		或居中，使文本断成多行以适应给定矩形来格式化文本，格式类型由\
		nFormat指定。\n该成员函数适应设备上下文中选取的字体、文本颜色\
		、背景色来显示本。";

CImage g_img_data;

UINT WM_LOAD_MORE_DATA = RegisterWindowMessage(L"WM_LOAD_MORE_DATA");

// CWeChatView

IMPLEMENT_DYNCREATE(CWeChatView, CScrollView)

BEGIN_MESSAGE_MAP(CWeChatView, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_SAVE, &CWeChatView::OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, &CWeChatView::OnFileOpen)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_NEW, &CWeChatView::OnFileNew)
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONUP()
	ON_REGISTERED_MESSAGE(WM_LOAD_MORE_DATA, OnLoadMoreData)
END_MESSAGE_MAP()

// CWeChatView 构造/析构

CWeChatView::CWeChatView()
{
	// TODO: 在此处添加构造代码

}

CWeChatView::~CWeChatView()
{
}

BOOL CWeChatView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
}

int paintCount = 0;

void CWeChatView::OnDraw(CDC* pDC)
{
	CPaintDC* dc = (CPaintDC*)pDC;

	CRect rcView;
	GetClientRect(rcView);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rcView.Width(), rcView.Height());

	memDC.SelectObject(&bmp);
	memDC.SelectObject(&m_text_font);

	memDC.FillSolidRect(rcView, RGB(245, 245, 245));

	CPoint pt = GetScrollPosition();

	DWORD paint = GetTickCount();

	if (dataList.size() > 0) {

		int offsetY = 0 - pt.y;
		int maxY = rcView.Height();

		CString str;
		str.Format(L"offsetY:%d, maxY:%ld\n", pt.y, maxY);
		OutputDebugString(str);

		auto& iter = dataList.begin();

		DWORD start = GetTickCount();

		int firstItemIndex = 0;

		for (; iter != dataList.end(); iter++) {
			offsetY += (*iter)->GetHeight();
			if (offsetY >= 0) {
				offsetY -= (*iter)->GetHeight();
				break;
			}
			firstItemIndex++;
		}

		str.Format(L"find first item %d use time:%ld\n", firstItemIndex, GetTickCount() - start);
		OutputDebugString(str);

		CPoint ptHover;
		GetCursorPos(&ptHover);
		ScreenToClient(&ptHover);

		start = GetTickCount();

		int drawItemCount = 0;

		for (; iter != dataList.end(); iter++) {
			offsetY += (*iter)->Draw(&memDC, offsetY, ptHover);
			drawItemCount++;
			if (offsetY >= maxY) {
				break;
			}
		}

		str.Format(L"draw %d items use time:%ld\n", drawItemCount, GetTickCount() - start);
		OutputDebugString(str);
	}

	pDC->BitBlt(pt.x, pt.y, rcView.Width(), rcView.Height(), &memDC, 0, 0, SRCCOPY);

	CString str;
	str.Format(L"x:%d, y:%d, w:%d, h:%d, paintCount:%d time:%ld\n",
		pt.x, pt.y, rcView.Width(), rcView.Height(),
		paintCount++, GetTickCount() - paint);
	OutputDebugString(str);
}

void CWeChatView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	if (g_img_data.IsNull()) {
		g_img_data.Load(L"learn_xml.gif");
	}

	LOGFONT logFont = { 0 };
	GetGlobalData()->fontRegular.GetLogFont(&logFont);

	logFont.lfHeight = 24;
	_tcscpy_s(logFont.lfFaceName, _T("微软雅黑"));

	m_text_font.CreateFontIndirect(&logFont);

	CSize sizeTotal = { 0,0 };
	SetScrollSizes(MM_TEXT, sizeTotal, scrollPageSize, scrollLineSize);
}


// CWeChatView 打印

BOOL CWeChatView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CWeChatView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CWeChatView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CWeChatView 诊断

#ifdef _DEBUG
void CWeChatView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CWeChatView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CWeChatDoc* CWeChatView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWeChatDoc)));
	return (CWeChatDoc*)m_pDocument;
}
#endif //_DEBUG


/**
 * 测试1万条数据计算各自的位置所需的时间
 */
void testCalcSize(CDC* pDC) {

	list<unique_ptr<ChatData>> dataList;

	int strCount = 0;
	int imgCount = 0;

	DWORD start = GetTickCount();

	for (int i = 0; i < 10000; i++) {
		ChatData* data = nullptr;

		if (rand() % 2 == 0) {
			strCount++;
			data = new TextChatData(g_str_data);
		} else {
			imgCount++;
			data = new ImageChatData(g_img_data);
		}

		dataList.push_back(unique_ptr<ChatData>(data));
	}

	CString str;
	str.Format(L"strCount:%d imgCount:%d initTime:%d\n", strCount, imgCount, GetTickCount() - start);
	OutputDebugString(str);

	start = GetTickCount();

	int offsetY = 0;
	for (auto iter = dataList.begin(); iter != dataList.end(); iter++) {
		offsetY += (*iter)->CalcSize(pDC);
		offsetY += 8;
	}

	str.Format(L"(1) calcSizeTime:%d\n", GetTickCount() - start);
	OutputDebugString(str);

	start = GetTickCount();

	offsetY = 0;
	for (auto iter = dataList.begin(); iter != dataList.end(); iter++) {
		offsetY += (*iter)->CalcSize(pDC);
		offsetY += 8;
	}

	str.Format(L"(2) calcSizeTime:%d\n", GetTickCount() - start);
	OutputDebugString(str);
}

int CWeChatView::loadMoreData(InsertPos pos) {

	CDC* pDC = GetDC();
	CFont* pOldFont = pDC->SelectObject(&m_text_font);

	int newItemsHeight = 0;

	for (int i = 0; i < 20; i++) {
		ChatData* data = nullptr;

		if (rand() % 2 == 0) {
			data = new TextChatData((rand() % 2) ? g_str_data : g_str_data2);
		} else {
			data = new ImageChatData(g_img_data);
		}

		if (pos == InsertPos::Front) {
			dataList.push_front(unique_ptr<ChatData>(data));
		} else {
			dataList.push_back(unique_ptr<ChatData>(data));
		}

		newItemsHeight += data->CalcSize(pDC);
	}

	pDC->SelectObject(pOldFont);

	return newItemsHeight;
}

void CWeChatView::OnFileSave()
{
	int newItemsHeight = loadMoreData(InsertPos::Back);

	CSize size = GetTotalSize();
	size.cy += newItemsHeight;

	SetScrollSizes(MM_TEXT, size, scrollPageSize, scrollLineSize);
}

void CWeChatView::OnFileOpen()
{
	int newItemsHeight = loadMoreData(InsertPos::Front);

	CSize oldSize = GetTotalSize();

	CSize newSize = GetTotalSize();
	newSize.cy += newItemsHeight;

	CPoint pt = GetScrollPosition();
	pt.y += newItemsHeight;

	paintCount = 0;

	SetRedraw(FALSE);
	{
		SetScrollSizes(MM_TEXT, newSize, scrollPageSize, scrollLineSize);
		ScrollToPosition(pt);
	}
	SetRedraw(TRUE);

	CString str;
	str.Format(L"oldW:%d, oldH:%d, newW:%d, newH:%d\n",
		oldSize.cx, oldSize.cy, newSize.cx, newSize.cy);
	OutputDebugString(str);

	Invalidate();
}

BOOL CWeChatView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CWeChatView::OnFileNew()
{
	testCalcSize(GetDC());
}

void CWeChatView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_THUMBTRACK) {

		SCROLLINFO si;
		GetScrollInfo(SB_VERT, &si, SIF_TRACKPOS);

		nPos = si.nTrackPos;
	}

	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

ChatData* CWeChatView::HitTest(CPoint point)
{
	int offsetY = 0 - GetScrollPosition().y;
	int newOffsetY = offsetY;

	for (auto& chat : dataList) {
		newOffsetY += chat->GetHeight();
		if (point.y >= offsetY && point.y <= newOffsetY) {
			if (chat->HitTest(offsetY, point)) {
				return chat.get();
			} else {
				break;
			}
		}
		offsetY = newOffsetY;
	}

	return nullptr;
}

void CWeChatView::OnMouseMove(UINT nFlags, CPoint point)
{
	ChatData* chat = HitTest(point);

	if (chat != m_last_hover_chat) {
		m_last_hover_chat = chat;
		Invalidate();
	}

	CScrollView::OnMouseMove(nFlags, point);
}

void CWeChatView::OnLButtonDown(UINT nFlags, CPoint point)
{
	ChatData* chat = HitTest(point);
	if (chat != m_last_press_chat) {

		if (m_last_press_chat != nullptr) {
			m_last_press_chat->SetPressed(false);
		}

		m_last_press_chat = chat;

		if (m_last_press_chat != nullptr) {
			m_last_press_chat->SetPressed(true);
		}

		Invalidate();
	}

	CScrollView::OnLButtonDown(nFlags, point);
}

BOOL CWeChatView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 每次滚动之后，窗口会设置剪裁区，而这个剪裁区很小，所以需要
	// 重新设置一下窗口的剪裁区。

	Invalidate();

	BOOL result = CScrollView::OnMouseWheel(nFlags, zDelta, pt);

	CPoint ptCursor = pt;
	ScreenToClient(&ptCursor);

	ChatData* chat = HitTest(ptCursor);
	if (chat != m_last_hover_chat) {
		m_last_hover_chat = chat;
	}

	return result;
}

void CWeChatView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CScrollView::OnLButtonUp(nFlags, point);
}

BOOL CWeChatView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{

	return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
}

BOOL CWeChatView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	int offsetY = GetScrollPosition().y;

	if (sizeScroll.cy < 0) {
		sizeScroll.cy = 0 - scrollLineSize.cy;

		if (offsetY + sizeScroll.cy < 0) {
			PostMessage(WM_LOAD_MORE_DATA, InsertPos::Front);
		}

	} else {

		CRect rcView;
		GetClientRect(&rcView);

		CSize size = GetTotalSize();

		if (offsetY + rcView.Height() + sizeScroll.cy > size.cy) {
			PostMessage(WM_LOAD_MORE_DATA, InsertPos::Back);
		}
	}

	return CScrollView::OnScrollBy(sizeScroll, bDoScroll);
}

LRESULT CWeChatView::OnLoadMoreData(WPARAM wParam, LPARAM lParam)
{
	if (wParam == InsertPos::Back) {
		OnFileSave();
	} else {
		OnFileOpen();
	}

	return TRUE;
}