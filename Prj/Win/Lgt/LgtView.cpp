
// LgtView.cpp: CLgtView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Lgt.h"
#endif

#include "LgtDoc.h"
#include "LgtView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLgtView

IMPLEMENT_DYNCREATE(CLgtView, CView)

BEGIN_MESSAGE_MAP(CLgtView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CLgtView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CLgtView 构造/析构

CLgtView::CLgtView() noexcept
{
	// TODO: 在此处添加构造代码

}

CLgtView::~CLgtView()
{
}

BOOL CLgtView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CLgtView 绘图

void CLgtView::OnDraw(CDC* /*pDC*/)
{
	CLgtDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CLgtView 打印


void CLgtView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CLgtView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CLgtView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CLgtView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CLgtView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CLgtView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CLgtView 诊断

#ifdef _DEBUG
void CLgtView::AssertValid() const
{
	CView::AssertValid();
}

void CLgtView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLgtDoc* CLgtView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLgtDoc)));
	return (CLgtDoc*)m_pDocument;
}
#endif //_DEBUG


// CLgtView 消息处理程序
