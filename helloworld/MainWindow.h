#pragma once
#pragma comment(lib, "d2d1")

#include <vector>
#include <d2d1.h>
#include "BaseWindow.h"

typedef std::vector<D2D1_ELLIPSE> ELLIPSEList;

class MainWindow : public BaseWindow<MainWindow>
{
public:
	MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL), rows(5), cols(10)
	{
	}

    PCWSTR  ClassName() const { return L"Sample Window Class"; }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	void    DiscardGraphicsResources();
	void    OnPaint();
	void    OnResize();
	void OnLeftButtonDown(LPARAM lParam);
	void OnLeftButtonUp(LPARAM lParam);
	void OnLeftButtonDoubleClicked(LPARAM lParam);
	void OnRightButtonDown(LPARAM lParam);
	void OnRightButtonUp(LPARAM lParam);
	void OnRightButtonDoubleClicked(LPARAM lParam);

private:
	void OnButtonEventFired(PCWSTR eventName, LPARAM lParam);
	void DrawStar( ID2D1HwndRenderTarget * pRenderTarget, const D2D1_ELLIPSE& it, ID2D1SolidColorBrush * pBrush );
private:
	ID2D1Factory            *pFactory;
	ID2D1HwndRenderTarget   *pRenderTarget;
	ID2D1SolidColorBrush    *pBrush;
	ELLIPSEList            ellipses;
	const int rows;
	const int cols;
};