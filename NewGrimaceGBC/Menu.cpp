
#include "Class.h"
#include <sstream>

extern GGBC ggbc1;
extern GGBC ggbc2;

#define MID_EXIT          1
#define MID_ROM_LOAD1     2
#define MID_ROM_AUDIO     3
#define MID_ROM_RESET1    4
#define MID_DEBUG_SHOW    5
#define MID_ROM_LOAD2     6
#define MID_ROM_RESET2    7
#define MID_ROM_CLOSE2    8

HINSTANCE hGlobalInstance;
HWND hDebugWindow = NULL;
HWND hDebugStatus;
HWND hROMText;
HWND hDebugText;
HWND hStaticText;
HWND hPauseButton;
HWND hRefreshButton;
HWND hDropdownBox;
HWND hDropdownBank;
HWND hBreakSRAMEnable;
HWND hBreakSRAMDisable;
HWND hBreakPC;
HWND hBreakPCAddr;
HWND hBreakWrite;
HWND hBreakWriteAddr;
HWND hBreakRead;
HWND hBreakReadAddr;

LRESULT CALLBACK WndProcDebug (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcPauseButton (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcRefreshButton (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcDropdown (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcDropdownBank (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcCheckBox (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcBreakPCAddr (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcBreakWriteAddr (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcBreakReadAddr (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

WNDPROC DefPauseButtonProc;
WNDPROC DefRefreshButtonProc;
WNDPROC DefDropdownProc;
WNDPROC DefDropdownBankProc;
WNDPROC DefBreakSRAMEnableProc;
WNDPROC DefBreakSRAMDisableProc;
WNDPROC DefBreakPCProc;
WNDPROC DefBreakPCAddrProc;
WNDPROC DefBreakWriteProc;
WNDPROC DefBreakWriteAddrProc;
WNDPROC DefBreakReadProc;
WNDPROC DefBreakReadAddrProc;

void MenuUtil::AddGGBCMenu(HWND hWnd, HINSTANCE hInstance) {

    hGlobalInstance = hInstance;

    // Add a menu system
    HMENU Menu_Main = CreateMenu ();
    HMENU Menu_ROM = CreateMenu ();
    HMENU Menu_Debug = CreateMenu ();
    InsertMenu (Menu_Main, 0-1, MF_BYPOSITION | MF_ENABLED | MF_STRING | MF_POPUP, (UINT_PTR)Menu_ROM, L"ROM");
    InsertMenu (Menu_Main, 0-1, MF_BYPOSITION | MF_ENABLED | MF_STRING | MF_POPUP, (UINT_PTR)Menu_Debug, L"Debug");
    AppendMenu (Menu_Main, MF_ENABLED | MF_STRING, MID_EXIT, L"Exit");
    AppendMenu (Menu_ROM, MF_ENABLED | MF_STRING, MID_ROM_LOAD1, L"Load ROM");
    AppendMenu (Menu_ROM, MF_ENABLED | MF_STRING, MID_ROM_RESET1, L"Reset ROM 1");
    AppendMenu (Menu_ROM, MF_SEPARATOR, 0, L"");
    AppendMenu (Menu_ROM, MF_ENABLED | MF_STRING, MID_ROM_LOAD2, L"Load ROM 2");
    AppendMenu (Menu_ROM, MF_ENABLED | MF_STRING, MID_ROM_RESET2, L"Reset ROM 2");
    AppendMenu (Menu_ROM, MF_ENABLED | MF_STRING, MID_ROM_CLOSE2, L"Close Second ROM");
    AppendMenu (Menu_ROM, MF_SEPARATOR, 0, L"");
    AppendMenu (Menu_ROM, MF_ENABLED | MF_STRING, MID_ROM_AUDIO, L"Audio On/Off");
    AppendMenu (Menu_Debug, MF_ENABLED | MF_STRING, MID_DEBUG_SHOW, L"Show Debug Window");
    if (SetMenu (hWnd, Menu_Main) == 0)
        MessageBox (hWnd, L"Couldn't apply menu to window.", L"Error", MB_OK);

}

LRESULT CALLBACK MenuUtil::Command(HWND hWnd, WORD Comm) {

    static OPENFILENAME* ofn;
    static wchar_t namebuff[256];

    switch (Comm) {
    case MID_EXIT:
        PostQuitMessage (0);
        break;
    case MID_ROM_LOAD1:
        ofn = new OPENFILENAME {
            sizeof (OPENFILENAME), // lStructSize
            hWnd, // hwndOwner
            NULL, // hInstance
            L"Gameboy ROM\0*.gb;*.gbc\0\0", // lpstrFilter
            NULL, // lpstrCustomFilter
            0, // nMaxCustFilter
            1, // nFilterIndex
            namebuff, // lpstrFile
            256, // nMaxFile
            NULL, // lpstrFileTitle
            0, // nMaxFileTitle
            NULL, // lpstrInitialDir
            L"Open ROM file", // lpstrTitle
            OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, // Flags
            0, // nFileOffset
            0, // nFileExtension
            L"gb", // lpstrDefExt
            0, // lCustData
            NULL, // lpfnHook
            NULL // lpTemplateName
            #if (_WIN32_WINNT >= 0x0500)
            ,0, // *pwReserved
            0, // dwReserved
            0 // FlagsEx
            #endif
        };
        if (GetOpenFileName (ofn)) {
            ggbc1.Close();
            ggbc1.LoadROM (namebuff, hWnd);
            if (ggbc1.ROM_Valid) {
                if (hDebugWindow != NULL)
                    LoadROMDetails (&ggbc1);
                ggbc1.Begin();
            }
        }
        delete ofn;
        break;
    case MID_ROM_RESET1:
        ggbc1.Close();
        ggbc1.Begin();
        break;
    case MID_ROM_LOAD2:
        ofn = new OPENFILENAME {
            sizeof (OPENFILENAME), // lStructSize
            hWnd, // hwndOwner
            NULL, // hInstance
            L"Gameboy ROM\0*.gb;*.gbc\0\0", // lpstrFilter
            NULL, // lpstrCustomFilter
            0, // nMaxCustFilter
            1, // nFilterIndex
            namebuff, // lpstrFile
            256, // nMaxFile
            NULL, // lpstrFileTitle
            0, // nMaxFileTitle
            NULL, // lpstrInitialDir
            L"Open ROM file", // lpstrTitle
            OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, // Flags
            0, // nFileOffset
            0, // nFileExtension
            L"gb", // lpstrDefExt
            0, // lCustData
            NULL, // lpfnHook
            NULL // lpTemplateName
            #if (_WIN32_WINNT >= 0x0500)
            ,0, // *pwReserved
            0, // dwReserved
            0 // FlagsEx
            #endif
        };
        if (GetOpenFileName (ofn)) {
            /*
            ggbc2->LoadROM (namebuff, hWnd);
            if (ggbc2->ROM_Valid) {
                ggbc2->Running = TRUE;
                ggbc2->Reset ();
                XFactor = WindowWidth / 320;
                YFactor = WindowHeight / 144;
                OutZoomFactor = (XFactor < YFactor) ? XFactor : YFactor;
                OutCoordX = (WindowWidth - OutZoomFactor * 320) / (2 * WindowWidth);
                OutCoordY = 1 - (WindowHeight - OutZoomFactor * 144) / (2 * WindowHeight);
            }
            */
        }
        delete ofn;
        break;
    case MID_ROM_RESET2:
        /*
        ggbc2.Close();
        ggbc2.Begin ();
        */
        break;
    case MID_ROM_CLOSE2:
        ggbc2.Running = FALSE;
        /*
        XFactor = WindowWidth / 160;
        YFactor = WindowHeight / 144;
        OutZoomFactor = (XFactor < YFactor) ? XFactor : YFactor;
        OutCoordX = (WindowWidth - OutZoomFactor * 160) / (2 * WindowWidth);
        OutCoordY = 1 - (WindowHeight - OutZoomFactor * 144) / (2 * WindowHeight);
        */
        break;
    case MID_ROM_AUDIO:
        
        break;
    case MID_DEBUG_SHOW:
        if (hDebugWindow == NULL) {
            // New window
            hDebugWindow = CreateWindow (
                L"DebugClass", L"Grimace GBC - Debug Window",
                WS_CAPTION | WS_VISIBLE | WS_POPUPWINDOW | WS_CHILD | WS_TABSTOP,
                5, 5, 638, 692,
                hWnd, NULL, hGlobalInstance, NULL
            );
            // Status bar
            hDebugStatus = CreateWindowEx (
                WS_EX_WINDOWEDGE | WS_EX_TRANSPARENT,
                L"STATIC", (LPCWSTR)NULL,
                WS_DLGFRAME | WS_VISIBLE | WS_CHILD,
                10, 630, 610, 32,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            // ROM details box
            hROMText = CreateWindowEx (
                WS_EX_WINDOWEDGE | WS_EX_TRANSPARENT,
                L"STATIC", (LPCWSTR)NULL,
                WS_DLGFRAME | WS_VISIBLE | WS_CHILD,
                10, 10, 300, 146,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            // Debug text ouput box
            hDebugText = CreateWindowEx (
                WS_EX_WINDOWEDGE | WS_EX_TRANSPARENT,
                L"STATIC", (LPCWSTR)NULL,
                WS_DLGFRAME | WS_VISIBLE | WS_CHILD,
                320, 10, 300, 146,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            // Pause button
            hPauseButton = CreateWindow (
                L"BUTTON", L"Pause Emulator",
                WS_DLGFRAME | WS_VISIBLE | WS_CHILD,
                10, 166, 145, 32,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefPauseButtonProc = (WNDPROC)SetWindowLongPtr(hPauseButton,GWLP_WNDPROC,(LONG)WndProcPauseButton);
            // Refresh button
            hRefreshButton = CreateWindow (
                L"BUTTON", L"Refresh Data",
                WS_DLGFRAME | WS_VISIBLE | WS_CHILD,
                165, 166, 145, 32,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefRefreshButtonProc = (WNDPROC)SetWindowLongPtr(hRefreshButton,GWLP_WNDPROC,(LONG)WndProcRefreshButton);
            // Memory range dropdown box
            hDropdownBox = CreateWindow (
                L"COMBOBOX", (LPCWSTR)NULL,
                CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP,
                320, 166, 145, 200,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefDropdownProc = (WNDPROC) SetWindowLongPtr (hDropdownBox, GWLP_WNDPROC, (LONG)WndProcDropdown);
            SendMessage (hDropdownBox, CB_ADDSTRING, 0, (LPARAM) L"ROM");
            SendMessage (hDropdownBox, CB_ADDSTRING, 0, (LPARAM) L"VRAM");
            SendMessage (hDropdownBox, CB_ADDSTRING, 0, (LPARAM) L"External RAM");
            SendMessage (hDropdownBox, CB_ADDSTRING, 0, (LPARAM) L"Working RAM");
            SendMessage (hDropdownBox, CB_ADDSTRING, 0, (LPARAM) L"Sprite table");
            SendMessage (hDropdownBox, CB_ADDSTRING, 0, (LPARAM) L"IO ports");
            SendMessage (hDropdownBox, CB_ADDSTRING, 0, (LPARAM) L"Hi RAM");
            SendMessage (hDropdownBox, CB_ADDSTRING, 0, (LPARAM) L"Misc");
            SendMessage (hDropdownBox, CB_SETCURSEL, 5, 0);
            // Memory bank dropdown box
            hDropdownBank = CreateWindow (
                L"COMBOBOX", L"",
                CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP,
                475, 166, 145, 200,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefDropdownBankProc = (WNDPROC)SetWindowLongPtr(hDropdownBank,GWLP_WNDPROC,(LONG)WndProcDropdownBank);
            SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 0");
            SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
            // Text window
            hStaticText = CreateWindowEx (
                WS_EX_WINDOWEDGE | WS_EX_TRANSPARENT,
                L"EDIT", L"OK",
                WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP | ES_READONLY | ES_AUTOVSCROLL | ES_MULTILINE,
                10, 208, 610, 292,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            HFONT hf = (HFONT) GetStockObject (OEM_FIXED_FONT);
            SendMessage (hStaticText, WM_SETFONT, (WPARAM)hf, 0);
            EnableScrollBar (hStaticText, SB_HORZ, ESB_ENABLE_BOTH);
            // Breakpoint type checkboxes
            hBreakSRAMEnable = CreateWindow (
                L"BUTTON", L"Break on SRAM enable",
                BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
                10, 510, 195, 20,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefBreakSRAMEnableProc = (WNDPROC)SetWindowLongPtr(hBreakSRAMEnable,GWLP_WNDPROC,(LONG)WndProcCheckBox);
            hBreakSRAMDisable = CreateWindow (
                L"BUTTON", L"Break on SRAM disable",
                BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
                215, 510, 195, 20,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefBreakSRAMDisableProc = (WNDPROC)SetWindowLongPtr(hBreakSRAMDisable,GWLP_WNDPROC,(LONG)WndProcCheckBox);
            hBreakPC = CreateWindow (
                L"BUTTON", L"Break on PC address",
                BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
                10, 540, 195, 20,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefBreakPCProc = (WNDPROC)SetWindowLongPtr(hBreakPC,GWLP_WNDPROC,(LONG)WndProcCheckBox);
            hBreakWrite = CreateWindow (
                L"BUTTON", L"Break on write to address",
                BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
                10, 570, 195, 20,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefBreakWriteProc = (WNDPROC)SetWindowLongPtr(hBreakWrite,GWLP_WNDPROC,(LONG)WndProcCheckBox);
            hBreakRead = CreateWindow (
                L"BUTTON", L"Break on read from address",
                BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
                10, 600, 195, 20,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefBreakReadProc = (WNDPROC)SetWindowLongPtr(hBreakRead,GWLP_WNDPROC,(LONG)WndProcCheckBox);
            // The address typing field for PC breaks
            hBreakPCAddr = CreateWindow (
                L"EDIT", L"0100",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_LOWERCASE | ES_READONLY,
                215, 540, 195, 20,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefBreakPCAddrProc = (WNDPROC)SetWindowLongPtr(hBreakPCAddr,GWLP_WNDPROC,(LONG)WndProcBreakPCAddr);
            // The address typing field for write breaks
            hBreakWriteAddr = CreateWindow (
                L"EDIT", L"0000",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_LOWERCASE | ES_READONLY,
                215, 570, 195, 20,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefBreakWriteAddrProc = (WNDPROC)SetWindowLongPtr(hBreakWriteAddr,GWLP_WNDPROC,(LONG)WndProcBreakWriteAddr);
            // The address typing field for read breaks
            hBreakReadAddr = CreateWindow (
                L"EDIT", L"0000",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_LOWERCASE | ES_READONLY,
                215, 600, 195, 20,
                hDebugWindow, NULL, hGlobalInstance, NULL
            );
            DefBreakReadAddrProc = (WNDPROC)SetWindowLongPtr(hBreakReadAddr,GWLP_WNDPROC,(LONG)WndProcBreakReadAddr);
            // Fill data in text boxes
            LoadROMDetails (&ggbc1);
            LoadMemoryDetails (1, &ggbc1);
            SendMessage (hDebugStatus, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"Running");
            SendMessage (hDebugText, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"Awaiting breakpoint for info display");
        }
        break;
    }
    return 0;
}

/******************
 * Load ROM details
 * into debug window
 ******************/

void MenuUtil::LoadROMDetails (GGBC* ggbc) {
    
    std::stringstream text;
    text << "Internal name: " << ggbc->ROM_Title << "\n";
    text << "ROM size: ";
    if (ggbc->ROM_ROMSizeBytes >= 1048576) text << (ggbc->ROM_ROMSizeBytes / 1048576) << " MB\n";
    else text << (ggbc->ROM_ROMSizeBytes / 1024) << " KB\n";
    text << "Internal RAM size: ";
    if (ggbc->ROM_RAMSizeBytes >= 1024) text << (ggbc->ROM_RAMSizeBytes / 1024) << " KB\n";
    else text << ggbc->ROM_RAMSizeBytes << " bytes\n";
    text << "ROM type: ";
    if (ggbc->ROM_GBCFlag != 0) text << "Color\n";
    else if (ggbc->ROM_SGBFlag != 0) text << "Super Gameboy\n";
    else text << "Standard Gameboy\n";
    text << "Has battery backup: ";
    if (ggbc->ROM_HasBattery != 0) text << "Yes\n";
    else text << "No\n";
    text << "Has internal timer: ";
    if (ggbc->ROM_HasTimer != 0) text << "Yes\n";
    else text << "No\n";
    text << "Memory controller type: ";
    if (ggbc->ROM_MBC == MBC_NONE) text << "None\n";
    else if (ggbc->ROM_MBC == MBC1) text << "MBC1\n";
    else if (ggbc->ROM_MBC == MBC2) text << "MBC2\n";
    else if (ggbc->ROM_MBC == MBC3) text << "MBC3\n";
    else if (ggbc->ROM_MBC == MBC5) text << "MBC5\n";
    else text << "Unknown\n";
    std::string pudge = text.str();
    SendMessageA (hROMText, WM_SETTEXT, (WPARAM)NULL, (LPARAM)pudge.data());
    
}

/******************
 * Load memory details
 * into debug window
 ******************/

void MenuUtil::LoadMemoryDetails (int StatusChanged, GGBC* ggbc) {
    
    // Pass StatusChanged = 1 when the game changed, of the memory range changed
    
    int Item = SendMessage ((HWND)hDropdownBox, CB_GETCURSEL, 0, 0);
    int Bank;
    
    std::stringstream text;
    std::string pudge;
    
    int col;
    int line;
    
    int Insert;
    int Size;
    
    switch (Item) {
        case 0:
            if (StatusChanged) {
                SendMessage (hDropdownBank, CB_RESETCONTENT, 0, 0);
                Size = (unsigned int)ggbc->ROM_ROMBankSelectMask + 1;
                if (Size == 1) Size = 2;
                for (line = 0; line < Size; line++) {
                    text << "Bank ";
                    text << line;
                    pudge = text.str();
                    SendMessageA (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) pudge.data());
                    text.str(std::string());
                }
                EnableWindow (hDropdownBank, TRUE);
                SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
            }
            Bank = SendMessage ((HWND)hDropdownBank, CB_GETCURSEL, 0, 0);
            if (Bank == 0) text << "0x0000 ";
            else text << "0x4000 ";
            for (line = 0; line < 1024; line++) {
                for (col = 0; col < 16; col++) {
                    Insert = (int)(ggbc->ROM[0x4000*Bank + line*16 + col]);
                    text << Insert;
                    if (Insert < 10) text << "   ";
                    else if (Insert < 100) text << "  ";
                    else text << " ";
                }
                if ((line % 4) == 3) {
                    line++;
                    if (line == 1024) break;
                    if (Bank == 0)
                        text << "\r\n0x" << (line / 256);
                    else
                        text << "\r\n0x" << (line / 256) + 4;
                    Insert = (line & 0x00f0) / 16;
                    if (Insert < 10)
                        text << Insert;
                    else
                        text << (char)(Insert + 87);
                    Insert = line & 0x000f;
                    if (Insert < 10)
                        text << Insert;
                    else
                        text << (char)(Insert + 87);
                    text << "0 ";
                    line--;
                }
                else
                    text << "\r\n       ";
            }
            break;
        case 1:
            if (StatusChanged) {
                if (ggbc->ROM_GBCFlag != 0) {
                    EnableWindow (hDropdownBank, TRUE);
                    SendMessage (hDropdownBank, CB_RESETCONTENT, 0, 0);
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 0");
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 1");
                    SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
                }
                else {
                    SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
                    EnableWindow (hDropdownBank, FALSE);
                }
            }
            text << "0x8000 ";
            Bank = SendMessage ((HWND)hDropdownBank, CB_GETCURSEL, 0, 0);
            for (line = 0; line < 512; line++) {
                for (col = 0; col < 16; col++) {
                    Insert = (int)(ggbc->VRAM[0x2000*Bank + line*16 + col]);
                    text << Insert;
                    if (Insert < 10) text << "   ";
                    else if (Insert < 100) text << "  ";
                    else text << " ";
                }
                if ((line % 4) == 3) {
                    line++;
                    if (line == 512) break;
                    if (line < 256)
                        text << "\r\n0x8";
                    else
                        text << "\r\n0x9";
                    Insert = (line & 0x00f0) / 16;
                    if (Insert < 10)
                        text << Insert;
                    else
                        text << (char)(Insert + 87);
                    Insert = line & 0x000f;
                    if (Insert < 10)
                        text << Insert;
                    else
                        text << (char)(Insert + 87);
                    text << "0 ";
                    line--;
                }
                else
                    text << "\r\n       ";
            }
            break;
        case 2:
            if (StatusChanged) {
                if ((ggbc->ROM_HasSRAM != 0) && (ggbc->ROM_RAMSizeBytes > 8192)) {
                    EnableWindow (hDropdownBank, TRUE);
                    SendMessage (hDropdownBank, CB_RESETCONTENT, 0, 0);
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 0");
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 1");
                    if (ggbc->ROM_RAMSizeBytes > 16384) {
                        SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 2");
                        SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 3");
                    }
                    SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
                }
                else {
                    SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
                    EnableWindow (hDropdownBank, FALSE);
                }
            }
            if (ggbc->ROM_RAMSizeBytes == 0)
                text << "No external RAM exists.";
            else {
                Size = ggbc->ROM_RAMSizeBytes;
                text << "0xa000 ";
                if (Size <= 8192) {
                    for (line = 0; line < Size / 16; line++) {
                        for (col = 0; col < 16; col++) {
                            Insert = (int)(ggbc->SRAM[line*16 + col]);
                            text << Insert;
                            if (Insert < 10) text << "   ";
                            else if (Insert < 100) text << "  ";
                            else text << " ";
                        }
                        if ((line % 4) == 3) {
                            line++;
                            if ((line * 16) == ggbc->ROM_RAMSizeBytes) break;
                            if (line == 512) break;
                            if (line < 256)
                                text << "\r\n0xa";
                            else
                                text << "\r\n0xb";
                            Insert = (line & 0x00f0) / 16;
                            if (Insert < 10)
                                text << Insert;
                            else
                                text << (char)(Insert + 87);
                            Insert = line & 0x000f;
                            if (Insert < 10)
                                text << Insert;
                            else
                                text << (char)(Insert + 87);
                            text << "0 ";
                            line--;
                        }
                        else
                            text << "\r\n       ";
                    }
                }
                else {
                    Bank = SendMessage ((HWND)hDropdownBank, CB_GETCURSEL, 0, 0);
                    for (line = 0; line < 512; line++) {
                        for (col = 0; col < 16; col++) {
                            Insert = (int)(ggbc->SRAM[0x1000*Bank + line*16 + col]);
                            text << Insert;
                            if (Insert < 10) text << "   ";
                            else if (Insert < 100) text << "  ";
                            else text << " ";
                        }
                        if ((line % 4) == 3) {
                            line++;
                            if (line == 512) break;
                            if (line < 256)
                                text << "\r\n0xa";
                            else
                                text << "\r\n0xb";
                            Insert = (line & 0x00f0) / 16;
                            if (Insert < 10)
                                text << Insert;
                            else
                                text << (char)(Insert + 87);
                            Insert = line & 0x000f;
                            if (Insert < 10)
                                text << Insert;
                            else
                                text << (char)(Insert + 87);
                            text << "0 ";
                            line--;
                        }
                        else
                            text << "\r\n       ";
                    }
                }
            }
            break;
        case 3:
            if (StatusChanged) {
                EnableWindow (hDropdownBank, TRUE);
                if (ggbc->ROM_GBCFlag != 0) {
                    SendMessage (hDropdownBank, CB_RESETCONTENT, 0, 0);
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 0");
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 1");
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 2");
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 3");
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 4");
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 5");
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 6");
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 7");
                    SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
                }
                else {
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 0");
                    SendMessage (hDropdownBank, CB_ADDSTRING, 0, (LPARAM) L"Bank 1");
                    SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
                }
            }
            Bank = SendMessage ((HWND)hDropdownBank, CB_GETCURSEL, 0, 0);
            if (Bank == 0) text << "0xc000 ";
            else text << "0xd000 ";
            for (line = 0; line < 256; line++) {
                for (col = 0; col < 16; col++) {
                    Insert = (int)(ggbc->SRAM[0x1000*Bank + line*16 + col]);
                    text << Insert;
                    if (Insert < 10) text << "   ";
                    else if (Insert < 100) text << "  ";
                    else text << " ";
                }
                if ((line % 4) == 3) {
                    line++;
                    if (line == 256) break;
                    if (Bank == 0) text << "\r\n0xc";
                    else text << "\r\n0xd";
                    Insert = (line & 0x00f0) / 16;
                    if (Insert < 10)
                        text << Insert;
                    else
                        text << (char)(Insert + 87);
                    Insert = line & 0x000f;
                    if (Insert < 10)
                        text << Insert;
                    else
                        text << (char)(Insert + 87);
                    text << "0 ";
                    line--;
                }
                else
                    text << "\r\n       ";
            }
            break;
        case 4:
            if (StatusChanged) {
                SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
                EnableWindow (hDropdownBank, FALSE);
            }
            text << "0xe000 ";
            for (line = 0; line < 10; line++) {
                for (col = 0; col < 16; col++) {
                    Insert = (int)(ggbc->OAM[line*16 + col]);
                    text << Insert;
                    if (Insert < 10) text << "   ";
                    else if (Insert < 100) text << "  ";
                    else text << " ";
                }
                if ((line % 4) == 3) {
                    line++;
                    if (line == 10) break;
                    text << "\r\n0xe";
                    Insert = (line & 0x00f0) / 16;
                    if (Insert < 10)
                        text << Insert;
                    else
                        text << (char)(Insert + 87);
                    Insert = line & 0x000f;
                    if (Insert < 10)
                        text << Insert;
                    else
                        text << (char)(Insert + 87);
                    text << "0 ";
                    line--;
                }
                else
                    text << "\r\n       ";
            }
            break;
        case 5:
            if (StatusChanged) {
                SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
                EnableWindow (hDropdownBank, FALSE);
            }
            text << "0xff00 ";
            for (line = 0; line < 16; line++) {
                for (col = 0; col < 8; col++) {
                    Insert = (int)(ggbc->IOPorts[line*8 + col]);
                    text << Insert;
                    if (Insert < 10) text << "   ";
                    else if (Insert < 100) text << "  ";
                    else text << " ";
                }
                if (line == 15) break;
                if (line == 3)
                    text << "\r\n0xff20 ";
                else if (line == 7)
                    text << "\r\n0xff40 ";
                else if (line == 11)
                    text << "\r\n0xff60 ";
                else
                    text << "\r\n       ";
            }
            break;
        case 6:
            if (StatusChanged) {
                SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
                EnableWindow (hDropdownBank, FALSE);
            }
            text << "0xff80 ";
            for (line = 0; line < 16; line++) {
                for (col = 0; col < 8; col++) {
                    if (line + col == 22) break;
                    Insert = (int)(ggbc->IOPorts[0x80 + line*8 + col]);
                    text << Insert;
                    if (Insert < 10) text << "   ";
                    else if (Insert < 100) text << "  ";
                    else text << " ";
                }
                if (line == 15) break;
                if (line == 3)
                    text << "\r\n0xffa0 ";
                else if (line == 7)
                    text << "\r\n0xffc0 ";
                else if (line == 11)
                    text << "\r\n0xffe0 ";
                else
                    text << "\r\n       ";
            }
            text << (int)(ggbc->IOPorts[0xff]);
            break;
        case 7:
            if (StatusChanged) {
                SendMessage (hDropdownBank, CB_SETCURSEL, 0, 0);
                EnableWindow (hDropdownBank, FALSE);
            }
            text << "CPU regs AFBCDEHL ";
            Insert = (int)(ggbc->CPU_A); text << Insert;
            if (Insert < 10) text << "   "; else if (Insert < 100) text << "  "; else text << " ";
            Insert = (int)(ggbc->CPU_F); text << Insert;
            if (Insert < 10) text << "   "; else if (Insert < 100) text << "  "; else text << " ";
            Insert = (int)(ggbc->CPU_B); text << Insert;
            if (Insert < 10) text << "   "; else if (Insert < 100) text << "  "; else text << " ";
            Insert = (int)(ggbc->CPU_C); text << Insert;
            if (Insert < 10) text << "   "; else if (Insert < 100) text << "  "; else text << " ";
            Insert = (int)(ggbc->CPU_D); text << Insert;
            if (Insert < 10) text << "   "; else if (Insert < 100) text << "  "; else text << " ";
            Insert = (int)(ggbc->CPU_E); text << Insert;
            if (Insert < 10) text << "   "; else if (Insert < 100) text << "  "; else text << " ";
            Insert = (int)(ggbc->CPU_H); text << Insert;
            if (Insert < 10) text << "   "; else if (Insert < 100) text << "  "; else text << " ";
            Insert = (int)(ggbc->CPU_L); text << Insert;
            if (Insert < 10) text << "   "; else if (Insert < 100) text << "  "; else text << " ";
            text << "\r\nPC " << ggbc->CPU_PC;
            text << "\r\nSP " << ggbc->CPU_SP;
            text << "\r\n\r\nBG color palettes (8x4xRGB):\r\n";
            for (line = 0; line < 8; line++) {
                text << " Palette " << line << ": ";
                text << ggbc->CGB_BGPalette[line][0][0] << ", ";
                text << ggbc->CGB_BGPalette[line][0][1] << ", ";
                text << ggbc->CGB_BGPalette[line][0][2] << "\r\n            ";
                text << ggbc->CGB_BGPalette[line][1][0] << ", ";
                text << ggbc->CGB_BGPalette[line][1][1] << ", ";
                text << ggbc->CGB_BGPalette[line][1][2] << "\r\n            ";
                text << ggbc->CGB_BGPalette[line][2][0] << ", ";
                text << ggbc->CGB_BGPalette[line][2][1] << ", ";
                text << ggbc->CGB_BGPalette[line][2][2] << "\r\n            ";
                text << ggbc->CGB_BGPalette[line][3][0] << ", ";
                text << ggbc->CGB_BGPalette[line][3][1] << ", ";
                text << ggbc->CGB_BGPalette[line][3][2] << "\r\n";
            }
            text << "Sprite color palettes (8x4xRGB):\r\n" << ggbc->CPU_SP;
            for (line = 0; line < 8; line++) {
                text << " Palette " << line << ": ";
                text << ggbc->CGB_OBJPalette[line][0][0] << ", ";
                text << ggbc->CGB_OBJPalette[line][0][1] << ", ";
                text << ggbc->CGB_OBJPalette[line][0][2] << "\r\n            ";
                text << ggbc->CGB_OBJPalette[line][1][0] << ", ";
                text << ggbc->CGB_OBJPalette[line][1][1] << ", ";
                text << ggbc->CGB_OBJPalette[line][1][2] << "\r\n            ";
                text << ggbc->CGB_OBJPalette[line][2][0] << ", ";
                text << ggbc->CGB_OBJPalette[line][2][1] << ", ";
                text << ggbc->CGB_OBJPalette[line][2][2] << "\r\n            ";
                text << ggbc->CGB_OBJPalette[line][3][0] << ", ";
                text << ggbc->CGB_OBJPalette[line][3][1] << ", ";
                text << ggbc->CGB_OBJPalette[line][3][2] << "\r\n";
            }
            break;
    }
    
    pudge = text.str();
    SetWindowTextA (hStaticText, (LPCSTR)pudge.data());
    
}

void MenuUtil::LoadBreakDetails (GGBC *ggbc) {
    
    char BreakPointText[256];
    char Spare;
    
    strcpy_s (BreakPointText, "At last breakpoint:\n                        \nPC=0x    \nSP=0x    \nROM bank   \nSRAM bank  \nLast call was to 0x     at 0x    \n");
    strcpy_s (&BreakPointText[20], 24, ggbc->BreakMsg);
    BreakPointText[44] = '\n';
    // Write PC
    Spare = (char)((ggbc->CPU_PC & 0xf000) / 0x1000); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[50] = Spare;
    Spare = (char)((ggbc->CPU_PC & 0xf00) / 0x100); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[51] = Spare;
    Spare = (char)((ggbc->CPU_PC & 0xf0) / 0x10); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[52] = Spare;
    Spare = (char)((ggbc->CPU_PC & 0xf) / 0x1); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[53] = Spare;
    // Write SP
    Spare = (char)((ggbc->CPU_SP & 0xf000) / 0x1000); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[60] = Spare;
    Spare = (char)((ggbc->CPU_SP & 0xf00) / 0x100); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[61] = Spare;
    Spare = (char)((ggbc->CPU_SP & 0xf0) / 0x10); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[62] = Spare;
    Spare = (char)((ggbc->CPU_SP & 0xf) / 0x1); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[63] = Spare;
    // Write ROM bank
    Spare = (char)((ggbc->ROM_BankOffset/0x4000)/10); Spare += 0x30; BreakPointText[74] = Spare;
    Spare = (char)((ggbc->ROM_BankOffset/0x4000)%10); Spare += 0x30; BreakPointText[75] = Spare;
    // Write SRAM bank
    Spare = (char)((ggbc->SRAM_BankOffset/0x1000)); Spare += 0x30; BreakPointText[87] = Spare;
    // Address of most recently called function
    Spare = (char)((ggbc->BreakLastCallTo & 0xf000) / 0x1000); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[108] = Spare;
    Spare = (char)((ggbc->BreakLastCallTo & 0xf00) / 0x100); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[109] = Spare;
    Spare = (char)((ggbc->BreakLastCallTo & 0xf0) / 0x10); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[110] = Spare;
    Spare = (char)((ggbc->BreakLastCallTo & 0xf) / 0x1); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[111] = Spare;
    // Address of most recent call to a function
    Spare = (char)((ggbc->BreakLastCallAt & 0xf000) / 0x1000); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[118] = Spare;
    Spare = (char)((ggbc->BreakLastCallAt & 0xf00) / 0x100); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[119] = Spare;
    Spare = (char)((ggbc->BreakLastCallAt & 0xf0) / 0x10); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[120] = Spare;
    Spare = (char)((ggbc->BreakLastCallAt & 0xf) / 0x1); if (Spare < 0xa) Spare += 0x30; else Spare += 0x57; BreakPointText[121] = Spare;
    // Whether most recent function call returned
    if (ggbc->BreakLastCallReturned) strcpy_s (&BreakPointText[123], 13, "No ret since");
    else strcpy_s (&BreakPointText[123], 15, "Since returned");
    // Show this message in the static text box
    SendMessage (hDebugText, WM_SETTEXT, (WPARAM)NULL, (LPARAM)BreakPointText);
    
}

void MenuUtil::DecodeBreakPCAddress (GGBC* ggbc) {
    
    TCHAR GetText[5];
    ((WORD*) GetText)[0] = 5;
    SendMessage (hBreakPCAddr, EM_GETLINE, (WPARAM)0, (LPARAM)GetText);
    
    unsigned int GetInts[4];
    if (GetText[0] < 0x40) GetInts[0] = (unsigned int)(GetText[0] - 0x30) * 0x1000; else GetInts[0] = (unsigned int)(GetText[0] - 0x57) * 0x1000;
    if (GetText[1] < 0x40) GetInts[1] = (unsigned int)(GetText[1] - 0x30) * 0x100; else GetInts[1] = (unsigned int)(GetText[1] - 0x57) * 0x100;
    if (GetText[2] < 0x40) GetInts[2] = (unsigned int)(GetText[2] - 0x30) * 0x10; else GetInts[2] = (unsigned int)(GetText[2] - 0x57) * 0x10;
    if (GetText[3] < 0x40) GetInts[3] = (unsigned int)(GetText[3] - 0x30) * 0x1; else GetInts[3] = (unsigned int)(GetText[3] - 0x57) * 0x1;
    ggbc->BreakPCAddr = GetInts[0] + GetInts[1] + GetInts[2] + GetInts[3];
    
}

void MenuUtil::DecodeBreakWriteAddress (GGBC* ggbc) {
    
    TCHAR GetText[5];
    ((WORD*) GetText)[0] = 5;
    SendMessage (hBreakWriteAddr, EM_GETLINE, (WPARAM)0, (LPARAM)GetText);
    
    unsigned int GetInts[4];
    if (GetText[0] < 0x40) GetInts[0] = (unsigned int)(GetText[0] - 0x30) * 0x1000; else GetInts[0] = (unsigned int)(GetText[0] - 0x57) * 0x1000;
    if (GetText[1] < 0x40) GetInts[1] = (unsigned int)(GetText[1] - 0x30) * 0x100; else GetInts[1] = (unsigned int)(GetText[1] - 0x57) * 0x100;
    if (GetText[2] < 0x40) GetInts[2] = (unsigned int)(GetText[2] - 0x30) * 0x10; else GetInts[2] = (unsigned int)(GetText[2] - 0x57) * 0x10;
    if (GetText[3] < 0x40) GetInts[3] = (unsigned int)(GetText[3] - 0x30) * 0x1; else GetInts[3] = (unsigned int)(GetText[3] - 0x57) * 0x1;
    ggbc->BreakWriteAddr = GetInts[0] + GetInts[1] + GetInts[2] + GetInts[3];
    
}

void MenuUtil::DecodeBreakReadAddress (GGBC* ggbc) {
    
    TCHAR GetText[5];
    ((WORD*) GetText)[0] = 5;
    SendMessage (hBreakReadAddr, EM_GETLINE, (WPARAM)0, (LPARAM)GetText);
    
    unsigned int GetInts[4];
    if (GetText[0] < 0x40) GetInts[0] = (unsigned int)(GetText[0] - 0x30) * 0x1000; else GetInts[0] = (unsigned int)(GetText[0] - 0x57) * 0x1000;
    if (GetText[1] < 0x40) GetInts[1] = (unsigned int)(GetText[1] - 0x30) * 0x100; else GetInts[1] = (unsigned int)(GetText[1] - 0x57) * 0x100;
    if (GetText[2] < 0x40) GetInts[2] = (unsigned int)(GetText[2] - 0x30) * 0x10; else GetInts[2] = (unsigned int)(GetText[2] - 0x57) * 0x10;
    if (GetText[3] < 0x40) GetInts[3] = (unsigned int)(GetText[3] - 0x30) * 0x1; else GetInts[3] = (unsigned int)(GetText[3] - 0x57) * 0x1;
    ggbc->BreakReadAddr = GetInts[0] + GetInts[1] + GetInts[2] + GetInts[3];
    
}

////////////////////////////
// WndProc overrides
////////////////////////////

LRESULT CALLBACK WndProcDebug (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {

    case WM_DESTROY:
        hDebugWindow = NULL;
        return 0;

    default:
        return DefWindowProc (hWnd, message, wParam, lParam);

    }

}

LRESULT CALLBACK WndProcPauseButton (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {
    
        case WM_LBUTTONDOWN:
            if (ggbc1.Running) {
                ggbc1.Close();
                SendMessage (hPauseButton, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"Unpause Emulator");
                SendMessage (hDebugStatus, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"Emulation paused");
            }
            else {
                ggbc1.Begin();
                SendMessage (hPauseButton, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"Pause Emulator");
                SendMessage (hDebugStatus, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"Running");
            }

    }
    
    return CallWindowProc (DefPauseButtonProc, hPauseButton, message, wParam, lParam);
    
}

LRESULT CALLBACK WndProcRefreshButton (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {

        case WM_LBUTTONDOWN:
            MenuUtil::LoadMemoryDetails (0, &ggbc1);
            
    }
    
    return CallWindowProc (DefRefreshButtonProc, hRefreshButton, message, wParam, lParam);
    
}

LRESULT CALLBACK WndProcDropdown (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {

    case WM_COMMAND:
        
        if (HIWORD (wParam) == CBN_SELCHANGE)
            MenuUtil::LoadMemoryDetails (1, &ggbc1);
        break;

    }
    
    return CallWindowProc (DefDropdownProc, hDropdownBox, message, wParam, lParam);
    
}

LRESULT CALLBACK WndProcDropdownBank (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_COMMAND:
            if (HIWORD (wParam) == CBN_SELCHANGE)
                MenuUtil::LoadMemoryDetails (0, &ggbc1);
            break;
    }
    return CallWindowProc (DefDropdownBankProc, hDropdownBank, message, wParam, lParam);
}

LRESULT CALLBACK WndProcCheckBox (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (hWnd == hBreakSRAMEnable) {
        switch (message) {
            case WM_LBUTTONDOWN:
                if (SendMessage (hWnd, BM_GETCHECK, (WPARAM)0, (LPARAM)0) == BST_CHECKED) {
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)BST_UNCHECKED, (LPARAM)0);
                    ggbc1.BreakOnSRAMEnable = 0;
                }
                else {
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)BST_CHECKED, (LPARAM)0);
                    ggbc1.BreakOnSRAMEnable = 1;
                }
                ggbc1.TotalBreakEnables = ggbc1.BreakOnSRAMEnable + ggbc1.BreakOnSRAMDisable + ggbc1.BreakOnPC + ggbc1.BreakOnWrite + ggbc1.BreakOnRead;
                break;
        }
        return CallWindowProc (DefBreakSRAMEnableProc, hWnd, message, wParam, lParam);
    }
    else if (hWnd == hBreakSRAMDisable) {
        switch (message) {
            case WM_LBUTTONDOWN:
                if (SendMessage (hWnd, BM_GETCHECK, (WPARAM)0, (LPARAM)0) == BST_CHECKED) {
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)BST_UNCHECKED, (LPARAM)0);
                    ggbc1.BreakOnSRAMDisable = 0;
                }
                else {
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)BST_CHECKED, (LPARAM)0);
                    ggbc1.BreakOnSRAMDisable = 1;
                }
                ggbc1.TotalBreakEnables = ggbc1.BreakOnSRAMEnable + ggbc1.BreakOnSRAMDisable + ggbc1.BreakOnPC + ggbc1.BreakOnWrite + ggbc1.BreakOnRead;
                break;
        }
        return CallWindowProc (DefBreakSRAMEnableProc, hWnd, message, wParam, lParam);
    }
    else if (hWnd == hBreakPC) {
        switch (message) {
            case WM_LBUTTONDOWN:
                if (SendMessage (hWnd, BM_GETCHECK, (WPARAM)0, (LPARAM)0) == BST_CHECKED) {
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)BST_UNCHECKED, (LPARAM)0);
                    ggbc1.BreakOnPC = 0;
                }
                else {
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)BST_CHECKED, (LPARAM)0);
                    ggbc1.BreakOnPC = 1;
                }
                ggbc1.TotalBreakEnables = ggbc1.BreakOnSRAMEnable + ggbc1.BreakOnSRAMDisable + ggbc1.BreakOnPC + ggbc1.BreakOnWrite + ggbc1.BreakOnRead;
                break;
        }
        return CallWindowProc (DefBreakPCProc, hWnd, message, wParam, lParam);
    }
    else if (hWnd == hBreakWrite) {
        switch (message) {
            case WM_LBUTTONDOWN:
                if (SendMessage (hWnd, BM_GETCHECK, (WPARAM)0, (LPARAM)0) == BST_CHECKED) {
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)BST_UNCHECKED, (LPARAM)0);
                    ggbc1.BreakOnWrite = 0;
                }
                else {
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)BST_CHECKED, (LPARAM)0);
                    ggbc1.BreakOnWrite = 1;
                }
                ggbc1.TotalBreakEnables = ggbc1.BreakOnSRAMEnable + ggbc1.BreakOnSRAMDisable + ggbc1.BreakOnPC + ggbc1.BreakOnWrite + ggbc1.BreakOnRead;
                break;
        }
        return CallWindowProc (DefBreakWriteProc, hWnd, message, wParam, lParam);
    }
    else if (hWnd == hBreakRead) {
        switch (message) {
            case WM_LBUTTONDOWN:
                if (SendMessage (hWnd, BM_GETCHECK, (WPARAM)0, (LPARAM)0) == BST_CHECKED) {
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)BST_UNCHECKED, (LPARAM)0);
                    ggbc1.BreakOnRead = 0;
                }
                else {
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)BST_CHECKED, (LPARAM)0);
                    ggbc1.BreakOnRead = 1;
                }
                ggbc1.TotalBreakEnables = ggbc1.BreakOnSRAMEnable + ggbc1.BreakOnSRAMDisable + ggbc1.BreakOnPC + ggbc1.BreakOnWrite + ggbc1.BreakOnRead;
                break;
        }
        return CallWindowProc (DefBreakReadProc, hWnd, message, wParam, lParam);
    }
    else
        return 0;
    
}

LRESULT CALLBACK WndProcBreakPCAddr (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    
    char* ReplaceText = new char[2];
    DWORD dwStartPos, dwEndPos;
    
    // Capture keys pressed
    if (message == WM_KEYDOWN) {
        if ((wParam >= 0x30 && wParam <= 0x39) || (wParam >= 0x41 && wParam <= 0x46)) {
            SendMessageA (hWnd, EM_GETSEL, (WPARAM)&dwStartPos, (LPARAM)&dwEndPos);
            if ((dwStartPos == dwEndPos) && (dwStartPos < 4)) {
                ReplaceText[0] = (char)wParam;
                ReplaceText[1] = '\0';
                SendMessageA (hWnd, EM_SETSEL, (WPARAM)dwStartPos, (LPARAM)(dwStartPos + 1));
                SendMessageA (hWnd, EM_REPLACESEL, FALSE, (LPARAM)ReplaceText);
                MenuUtil::DecodeBreakPCAddress (&ggbc1);
            }
        }
    }
    
    return CallWindowProc (DefBreakPCAddrProc, hWnd, message, wParam, lParam);
}

LRESULT CALLBACK WndProcBreakWriteAddr (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    
    char* ReplaceText = new char[2];
    DWORD dwStartPos, dwEndPos;
    
    // Capture keys pressed
    if (message == WM_KEYDOWN) {
        if ((wParam >= 0x30 && wParam <= 0x39) || (wParam >= 0x41 && wParam <= 0x46)) {
            SendMessageA (hWnd, EM_GETSEL, (WPARAM)&dwStartPos, (LPARAM)&dwEndPos);
            if ((dwStartPos == dwEndPos) && (dwStartPos < 4)) {
                ReplaceText[0] = (char)wParam;
                ReplaceText[1] = '\0';
                SendMessageA (hWnd, EM_SETSEL, (WPARAM)dwStartPos, (LPARAM)(dwStartPos + 1));
                SendMessageA (hWnd, EM_REPLACESEL, FALSE, (LPARAM)ReplaceText);
                MenuUtil::DecodeBreakWriteAddress (&ggbc1);
            }
        }
    }
    
    return CallWindowProc (DefBreakWriteAddrProc, hWnd, message, wParam, lParam);
}

LRESULT CALLBACK WndProcBreakReadAddr (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    
    char* ReplaceText = new char[2];
    DWORD dwStartPos, dwEndPos;
    
    // Capture keys pressed
    if (message == WM_KEYDOWN) {
        if ((wParam >= 0x30 && wParam <= 0x39) || (wParam >= 0x41 && wParam <= 0x46)) {
            SendMessageA (hWnd, EM_GETSEL, (WPARAM)&dwStartPos, (LPARAM)&dwEndPos);
            if ((dwStartPos == dwEndPos) && (dwStartPos < 4)) {
                ReplaceText[0] = (char)wParam;
                ReplaceText[1] = '\0';
                SendMessageA (hWnd, EM_SETSEL, (WPARAM)dwStartPos, (LPARAM)(dwStartPos + 1));
                SendMessageA (hWnd, EM_REPLACESEL, FALSE, (LPARAM)ReplaceText);
                MenuUtil::DecodeBreakReadAddress (&ggbc1);
            }
        }
    }
    
    return CallWindowProc (DefBreakReadAddrProc, hWnd, message, wParam, lParam);
}
