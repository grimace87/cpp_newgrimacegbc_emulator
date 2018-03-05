
#include "Class.h"

LRESULT CALLBACK WindowProcMain (HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK WndProcDebug (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HANDLE hEventFrameReady = NULL;
HANDLE hEventFrameComplete = NULL;
HANDLE hGLRunning = NULL;
GGBC ggbc1;
GGBC ggbc2;

int tryGetLastFileName(wchar_t* path, int buffSize) {

	// Attempt to read running path
	wchar_t localPath[512];
	int size = GetModuleFileNameW(NULL, localPath, buffSize);
	if (size == 0) return 0;

	// FInd last '\' character and put the config filename after that
	int foundIndex = -1;
	for (int t = 0; t < buffSize; t++) {
		if (localPath[t] == L'\\')
			foundIndex = t;
		else if (localPath[t] == L'\0')
			break;
	}
	if (foundIndex < 0) return 0;
	if (foundIndex > buffSize - 9) return 0;

	// Append characters
	foundIndex++;
	const wchar_t* configFile = L"conf.gcf";
	for (int i = 0; i < 9; i++)
		localPath[foundIndex + i] = configFile[i];

	// Open file
	FILE* file;
	_wfopen_s(&file, localPath, L"r");
	if (file == NULL) return 0;
	size_t read = fread((void*) path, 2, 512, file);
	fclose(file);
	return read;

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int iCmdShow) {

	// Create a window class for the main window and register it.
	WNDCLASSEX wcMain = {};
	wcMain.cbSize = sizeof(WNDCLASSEX);
	wcMain.style = 0;
	wcMain.lpfnWndProc = WindowProcMain;
	wcMain.cbClsExtra = 0;
	wcMain.cbWndExtra = 0;
	wcMain.hInstance = hInstance;
	wcMain.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcMain.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcMain.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcMain.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcMain.lpszMenuName = NULL;
	wcMain.lpszClassName = L"GrimaceGBC";
	if (RegisterClassEx(&wcMain) == 0) {
		MessageBox(NULL, L"Could not register the window class.", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Create a window class for the debug popup window
	WNDCLASS wcDebug = {};
	wcDebug.style = CS_OWNDC;
	wcDebug.lpfnWndProc = WndProcDebug;
	wcDebug.cbClsExtra = 0;
	wcDebug.cbWndExtra = 0;
	wcDebug.hInstance = hInstance;
	wcDebug.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcDebug.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcDebug.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wcDebug.lpszMenuName = NULL;
	wcDebug.lpszClassName = L"DebugClass";
	if (RegisterClass(&wcDebug) == 0) {
		MessageBox(NULL, L"Could not register the debug window class.", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Create a window
	HWND hWnd = CreateWindow(
		wcMain.lpszClassName,
		L"Grimace GBC",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL
	);
	if (hWnd == NULL) {
		MessageBox(NULL, L"Could not create the window.", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	MenuUtil::AddGGBCMenu(hWnd, hInstance);
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	// Create drawing synchronisation events
	hEventFrameReady = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEventFrameComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
	hGLRunning = CreateMutex(NULL, FALSE, NULL);

	// Set up OpenGL on this window
	ImgData = new GLuint[160 * 152];
	HDC hDC = GetDC(hWnd);
	if (hDC == NULL) {
		MessageBox(NULL, L"Could not obtain device context.", L"Error", MB_ICONEXCLAMATION | MB_OK);
		CloseHandle(hEventFrameReady);
		CloseHandle(hEventFrameComplete);
		CloseHandle(hGLRunning);
		delete[] ImgData;
		return 0;
	}
	RECT GetClientArea;
	GetClientRect(hWnd, &GetClientArea);
	GrimOpenGL::SetResize(GetClientArea.right - GetClientArea.left, GetClientArea.bottom - GetClientArea.top);
	GrimOpenGL::BeginOpenGL(hDC);

	// Begin the Grimace GBC if a previous file could be loaded
	DrawToFrame1 = TRUE;
	wchar_t fileToOpen[512];
	if (tryGetLastFileName(fileToOpen, 512)) {
		if (ggbc1.LoadROM(fileToOpen, hWnd))
			ggbc1.Begin();
	}
	
    // Main loop. Processes messages as needed.
    MSG msg = {};
    int MsgErrCode;
    while (MsgErrCode = GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Check errors
    if (MsgErrCode < 0)
        MessageBox(NULL, L"Message handler returned\nan error code.", L"Information", MB_OK);
    
    // Close OpenGL
    GrimOpenGL::CloseOpenGL();
    ReleaseDC(hWnd, hDC);
    CloseHandle(hGLRunning);
    
    // Close GGBC objects and their threads
    ggbc1.Close();
    ggbc2.Close();

    // Free image data memory
    delete[] ImgData;

    // Return
    return 0;

}

LRESULT CALLBACK WindowProcMain (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        GrimOpenGL::SetResize(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_COMMAND:
        return MenuUtil::Command(hWnd, LOWORD(wParam));
    case WM_KEYDOWN:
        switch (wParam) {

            case 37: // Left arrow
                ggbc1.KeyDir &= 0x0d;
                ggbc1.KeyStateChanged = TRUE;
                break;
            case 38: // Up arrow
                ggbc1.KeyDir &= 0x0b;
                ggbc1.KeyStateChanged = 1;
                break;
            case 39: // Right arrow
                ggbc1.KeyDir &= 0x0e;
                ggbc1.KeyStateChanged = 1;
                break;
            case 40: // Down arrow
                ggbc1.KeyDir &= 0x07;
                ggbc1.KeyStateChanged = 1;
                break;
            case 13: // Enter
                ggbc1.KeyBut &= 0x07;
                ggbc1.KeyStateChanged = 1;
                break;
            case 16: // Shift
                ggbc1.KeyBut &= 0x0b;
                ggbc1.KeyStateChanged = 1;
                break;
            case 90: // z
                ggbc1.KeyBut &= 0x0d;
                ggbc1.KeyStateChanged = 1;
                break;
            case 88: // x
                ggbc1.KeyBut &= 0x0e;
                ggbc1.KeyStateChanged = 1;
                break;
                
            case 100: // 4
                ggbc2.KeyDir &= 0x0d;
                ggbc2.KeyStateChanged = 1;
                break;
            case 104: // 8
                ggbc2.KeyDir &= 0x0b;
                ggbc2.KeyStateChanged = 1;
                break;
            case 102: // 6
                ggbc2.KeyDir &= 0x0e;
                ggbc2.KeyStateChanged = 1;
                break;
            case 101: // 5
                ggbc2.KeyDir &= 0x07;
                ggbc2.KeyStateChanged = 1;
                break;
            case 17: // Ctrl
                ggbc2.KeyBut &= 0x07;
                ggbc2.KeyStateChanged = 1;
                break;
            case 107: // +
                ggbc2.KeyBut &= 0x0b;
                ggbc2.KeyStateChanged = 1;
                break;
            case 96: // 0
                ggbc2.KeyBut &= 0x0d;
                ggbc2.KeyStateChanged = 1;
                break;
            case 110: // .
                ggbc2.KeyBut &= 0x0e;
                ggbc2.KeyStateChanged = 1;
                break;

            case VK_NUMPAD1:
                if (ggbc1.ClockDivide < 2)
                    ggbc1.ClockMultiply++;
                else
                    ggbc1.ClockDivide--;
                break;

            case VK_NUMPAD2:
                if (ggbc1.ClockMultiply < 2)
                    ggbc1.ClockDivide++;
                else
                    ggbc1.ClockMultiply--;
                break;

        }
        return 0;

    case WM_KEYUP:
        switch (wParam) {
            case 37: // Left arrow
                ggbc1.KeyDir |= 0x02;
                ggbc1.KeyStateChanged = 1;
                break;
            case 38: // Up arrow
                ggbc1.KeyDir |= 0x04;
                ggbc1.KeyStateChanged = 1;
                break;
            case 39: // Right arrow
                ggbc1.KeyDir |= 0x01;
                ggbc1.KeyStateChanged = 1;
                break;
            case 40: // Down arrow
                ggbc1.KeyDir |= 0x08;
                ggbc1.KeyStateChanged = 1;
                break;
            case 13: // Enter
                ggbc1.KeyBut |= 0x08;
                ggbc1.KeyStateChanged = 1;
                break;
            case 16: // Shift
                ggbc1.KeyBut |= 0x04;
                ggbc1.KeyStateChanged = 1;
                break;
            case 90: // z
                ggbc1.KeyBut |= 0x02;
                ggbc1.KeyStateChanged = 1;
                break;
            case 88: // x
                ggbc1.KeyBut |= 0x01;
                ggbc1.KeyStateChanged = 1;
                break;
            
            case 100: // 4
                ggbc2.KeyDir |= 0x02;
                ggbc2.KeyStateChanged = 1;
                break;
            case 104: // 8
                ggbc2.KeyDir |= 0x04;
                ggbc2.KeyStateChanged = 1;
                break;
            case 102: // 6
                ggbc2.KeyDir |= 0x01;
                ggbc2.KeyStateChanged = 1;
                break;
            case 101: // 5
                ggbc2.KeyDir |= 0x08;
                ggbc2.KeyStateChanged = 1;
                break;
            case 17: // Ctrl
                ggbc2.KeyBut |= 0x08;
                ggbc2.KeyStateChanged = 1;
                break;
            case 107: // +
                ggbc2.KeyBut |= 0x04;
                ggbc2.KeyStateChanged = 1;
                break;
            case 96: // 0
                ggbc2.KeyBut |= 0x02;
                ggbc2.KeyStateChanged = 1;
                break;
            case 110: // .
                ggbc2.KeyBut |= 0x01;
                ggbc2.KeyStateChanged = 1;
                break;
        }
        return 0;

    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);

}
