#include <cmath>
#include <windowsx.h>
#include <Strsafe.h>
#include "common.h"
#include "logging.h"
#include "MainWindow.h"


LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//LOG(INFO) << "HandleMessage event is " << uMsg << ".";

    switch (uMsg)
    {
	case WM_CREATE:
		{
			if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
			{
				LOG(ERROR) << "D2D1CreateFactory failed.";

				return -1;
			}
			return 0;
		}
    case WM_DESTROY:
		{
			DiscardGraphicsResources();
			SafeRelease(&pFactory);
			PostQuitMessage(0);
			return 0;
		}
	case WM_SIZE:
		{
			OnResize();
			return 0;
		}
    case WM_PAINT:
        {
			OnPaint();
			return 0;
        }
	case WM_LBUTTONDOWN:
		{
			OnLeftButtonDown(lParam);
			return 0;
		}
	case WM_LBUTTONUP:
		{
			OnLeftButtonUp(lParam);
			return 0;
		}
	case WM_LBUTTONDBLCLK:
		{
			OnLeftButtonDoubleClicked(lParam);
			return 0;
		}
	case WM_RBUTTONDOWN:
		{
			OnRightButtonDown(lParam);
			return 0;
		}
	case WM_RBUTTONUP:
		{
			OnRightButtonUp(lParam);
			return 0;
		}
	case WM_RBUTTONDBLCLK:
		{
			OnRightButtonDoubleClicked(lParam);
			return 0;
		}
    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }

    return TRUE;
}

// Recalculate drawing layout when the size of the window changes.

void MainWindow::CalculateLayout()
{
	if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		const FLOAT width = size.width / cols;
		const FLOAT height = size.height / rows;
		FLOAT xr = width / 2;
		FLOAT yr = height / 2;
		FLOAT x = -xr;
		FLOAT y = -yr;

		for (int i = 0; i < rows; i++)
		{
			x = -width / 2;
			y += height;

			for (int j = 0; j < cols; j++)
			{
				x += width;

				ELLIPSEList::value_type e = D2D1::Ellipse(D2D1::Point2F(x, y), xr, yr);

				ellipses.push_back(e);
			}
		}
	}
	else
	{
		LOG(ERROR) << "pRenderTarget is NULL.";
	}
}

// 延迟初始化成员。每次使用之前都尝试着初始化一次
HRESULT MainWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect(m_hwnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

	if (pRenderTarget == NULL)
	{
		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

			if (SUCCEEDED(hr))
			{
				CalculateLayout();

				return hr;
			}
			else
			{
				LOG(ERROR) << "CreateSolidColorBrush failed.";
			}
		}
		else
		{
			LOG(ERROR) << "CreateHwndRenderTarget failed.";
		}
	}
	else
	{
		//pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
		//pRenderTarget->Clear();
	}

	return hr;
}

void MainWindow::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
}

void MainWindow::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();

	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();

		pRenderTarget->Clear( D2D1::ColorF(D2D1::ColorF::SkyBlue) );

		for (ELLIPSEList::const_iterator it = ellipses.begin(); it != ellipses.end(); it++)
		{
			pRenderTarget->DrawEllipse(*it, pBrush);

			DrawStar(pRenderTarget, *it, pBrush);
		}

		hr = pRenderTarget->EndDraw();

		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			LOG(ERROR) << "Paint failed, release all graphic resources.";

			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
	}
	else
	{
		LOG(ERROR) << "CreateGraphicsResources failed.";
	}
}

void MainWindow::OnResize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
	else
	{
		LOG(ERROR) << "pRenderTarget is NULL.";
	}
}

void MainWindow::OnLeftButtonDown(LPARAM lParam)
{
	OnButtonEventFired(L"OnLeftButtonDown", lParam);
}

void MainWindow::OnLeftButtonUp(LPARAM lParam)
{
	OnButtonEventFired(L"OnLeftButtonUp", lParam);
}

void MainWindow::OnLeftButtonDoubleClicked(LPARAM lParam)
{
	OnButtonEventFired(L"OnLeftButtonDoubleClicked", lParam);
}

void MainWindow::OnRightButtonDown(LPARAM lParam)
{
	OnButtonEventFired(L"OnRightButtonDown", lParam);
}

void MainWindow::OnRightButtonUp(LPARAM lParam)
{
	OnButtonEventFired(L"OnRightButtonUp", lParam);
}

void MainWindow::OnRightButtonDoubleClicked(LPARAM lParam)
{
	OnButtonEventFired(L"OnRightButtonDoubleClicked", lParam);
}

void MainWindow::OnButtonEventFired( PCWSTR eventName, LPARAM lParam )
{
	if(lParam == NULL)
	{
		LOG(ERROR) << "lParam is NULL.";

		return;
	}

	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	WCHAR buf[256] = { 0 };

	if(S_OK != StringCbPrintf(buf, 256, L"Event %s is fired, pos is (%d, %d).", eventName, x, y))
	{
		LOG(ERROR) << "StringCchVPrintf failed.";

		return;
	}

	MessageBox(m_hwnd, buf, L"Button Event Fired", MB_OK);
}

void MainWindow::DrawStar( ID2D1HwndRenderTarget * pRenderTarget, const D2D1_ELLIPSE& it, ID2D1SolidColorBrush * pBrush )
{
	const FLOAT PI = 3.14159265;
	const FLOAT radian = PI / 180;
	D2D1_POINT_2F points[5];
	int angle = 0;
	FLOAT x = 0;
	FLOAT y = 0;

	for(int i = 0; i < 5; i++)
	{
		x = it.point.x + it.radiusX * cos(angle);
		y = it.point.y + it.radiusY * sin(angle);

		points[i].x = x;
		points[i].y = y;

		D2D1_ELLIPSE smallCircle;
		smallCircle.point = points[i];
		smallCircle.radiusX = 5;
		smallCircle.radiusY = 5;

		pRenderTarget->DrawEllipse(smallCircle, pBrush);

		angle += 72;
	}

	int ii = 0, jj = 0;

	for (int i = 0; i < 5; i++)
	{
		ii = i;
		jj = (ii + 2) % 5;

		pRenderTarget->DrawLine(points[ii], points[jj], pBrush);
	}
}
