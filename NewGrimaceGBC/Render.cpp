
#include "Class.h"

// The output buffer
GLuint* ImgData;

HGLRC hGLRC;
BOOL OpenGLRunning;
GLenum GetError;

int Width, Height;
int XFactor, YFactor;
float DrawRasterPosX, DrawRasterPosY, DrawZoom;
BOOL SetView = FALSE;
BOOL OpenGLInit = FALSE;

// Construct object and create a new thread
void GrimOpenGL::BeginOpenGL(HDC hDC) {

    // Begin the rendering thread
    CreateThread(NULL, 0, RunProc, (LPVOID)hDC, NULL, NULL);

}

// Close OpenGL context
void GrimOpenGL::CloseOpenGL() {
    
    // Tell thread to end and wait for it to finish
    OpenGLRunning = FALSE;
    if (WaitForSingleObject(hGLRunning, 500) == WAIT_TIMEOUT) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hGLRC);
    }

}

// Signal a resize
void GrimOpenGL::SetResize(int SetWidth, int SetHeight) {

    Width = SetWidth;
    Height = SetHeight;
    SetView = TRUE;

}

// The running thread
DWORD WINAPI GrimOpenGL::RunProc(LPVOID lpvParam) {
    
    // Initialise the variables
    OpenGLRunning = FALSE;

    // Cast argument to local variable
    HDC hDC = (HDC)lpvParam;

    // Set up an OpenGL rendering context
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        24,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        32,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    int NewPixFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, NewPixFormat, &pfd);
    hGLRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hGLRC);
    
    OpenGLRunning = TRUE;
    Sleep(500);
    while (OpenGLRunning) {

        // Wait for a draw event
        WaitForSingleObject(hEventFrameReady, INFINITE);
		OutputDebugString(L"Rendering frame.\n");

        // Check for resize
        if (SetView) {
            SetView = FALSE;
            glViewport(0, 0, Width, Height);
            glLoadIdentity();
            glOrtho(0.0f, 1.0, 0.0, 1.0, -1.0, 1.0);
            XFactor = Width / 160;
            YFactor = Height / 144;
            DrawZoom = (XFactor < YFactor) ? (float)XFactor : (float)YFactor;
            DrawRasterPosX = ((float)Width - DrawZoom * 160) / (2.0f * (float)Width);
            DrawRasterPosY = 1 - ((float)Height - DrawZoom * 144) / (2.0f * (float)Height);
        }
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glRasterPos2f (DrawRasterPosX, DrawRasterPosY);
        glPixelZoom (DrawZoom, -DrawZoom);
        glDrawPixels(160, 144, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, ImgData);
        
        SwapBuffers(hDC);

        // Signal frame drawn
        SetEvent(hEventFrameComplete);

    }
    
    // Delete the context
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hGLRC);

    ReleaseMutex(hGLRunning);

    return 0;

}
