#include <Windows.h>
#include "logging.h"
#include "MainWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	// Initialize Google's logging library.
	google::InitGoogleLogging("WinMain");
	google::SetLogDestination(google::GLOG_INFO, "D:\\repo\\win32demo\\Debug\\log\\helloworld.log");

    MainWindow win;

    if (!win.Create(L"Learn to Program Windows", WS_OVERLAPPEDWINDOW))
    {
		LOG(ERROR) << "Create main window failed.";

        return 0;
    }

	LOG(INFO) << "Create main window successfully.";

    ShowWindow(win.Window(), nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}