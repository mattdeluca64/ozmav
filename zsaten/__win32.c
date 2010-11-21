#include "globals.h"

#ifdef WIN32
WNDCLASSEX wcex;
HWND hwnd;
HDC hDC;
HGLRC hRC;
MSG msg;

int WinAPIInit(char * WndTitle, int Width, int Height)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = APP_TITLE;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wcex)) return EXIT_SUCCESS;

	hwnd = CreateWindowEx(0, APP_TITLE, WndTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, true);

	EnableOpenGL(hwnd, &hDC, &hRC);

	// resize window for OpenGL area itself to be ex. 640x480, not the actual window
	WinAPISetWindowSize((Width + GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXFRAME)), (Height + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYFRAME)));

	return EXIT_SUCCESS;
}

int WinAPIMain()
{
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if(msg.message == WM_QUIT) {
			vProgram.isRunning = false;
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return -1;

	} else {
		if(vProgram.key[VK_ESCAPE]) PostQuitMessage(0);

		if(vProgram.key['W']) ca_Movement(false, 2.0f);
		if(vProgram.key['S']) ca_Movement(false, -2.0f);
		if(vProgram.key['A']) ca_Movement(true, -2.0f);
		if(vProgram.key['D']) ca_Movement(true, 2.0f);

		if(vProgram.key['T']) ca_Movement(false, 0.5f);
		if(vProgram.key['G']) ca_Movement(false, -0.5f);
		if(vProgram.key['F']) ca_Movement(true, -0.5f);
		if(vProgram.key['H']) ca_Movement(true, 0.5f);

		if(vProgram.key[VK_UP]) vCamera.actorRotX -= 2.0f;
		if(vProgram.key[VK_DOWN]) vCamera.actorRotX += 2.0f;
		if(vProgram.key[VK_LEFT]) vCamera.actorRotY -= 2.0f;
		if(vProgram.key[VK_RIGHT]) vCamera.actorRotY += 2.0f;

		if(vProgram.key[VK_TAB]) {
			vProgram.showBones ^= 1;
			vProgram.key[VK_TAB] = false;
		}

		if(vProgram.key[VK_SPACE]) {
			vProgram.animPlay ^= 1;
			vProgram.key[VK_SPACE] = false;
		}

		if(vProgram.key[VK_F1]) {
			if(vCurrentActor.actorNumber > 0) {
				vCurrentActor.actorNumber--;
				vCurrentActor.animCurrent = 0;
				vCurrentActor.frameCurrent = 0;
				initActorParsing(-1);
			}
			vProgram.key[VK_F1] = false;
		}

		if(vProgram.key[VK_F2]) {
			if(vCurrentActor.actorNumber < vZeldaInfo.actorCount - 1) {
				vCurrentActor.actorNumber++;
				vCurrentActor.animCurrent = 0;
				vCurrentActor.frameCurrent = 0;
				initActorParsing(-1);
			}
			vProgram.key[VK_F2] = false;
		}

		if(vProgram.key[VK_F3]) {
			if(vCurrentActor.animCurrent > 0) {
				vCurrentActor.animCurrent--;
			} else {
				vCurrentActor.animCurrent = vCurrentActor.animTotal;
			}
			vCurrentActor.frameCurrent = 0;
			vProgram.key[VK_F3] = false;
		}

		if(vProgram.key[VK_F4]) {
			if(vCurrentActor.animCurrent < vCurrentActor.animTotal) {
				vCurrentActor.animCurrent++;
			} else {
				vCurrentActor.animCurrent = 0;
			}
			vCurrentActor.frameCurrent = 0;
			vProgram.key[VK_F4] = false;
		}

		if(vProgram.key[VK_F5]) {
			if(vCurrentActor.boneSetupCurrent > 0) {
				vCurrentActor.boneSetupCurrent--;
				vCurrentActor.frameCurrent = 0;
			}
			vProgram.key[VK_F5] = false;
		}

		if(vProgram.key[VK_F6]) {
			if(vCurrentActor.boneSetupCurrent < vCurrentActor.boneSetupTotal) {
				vCurrentActor.boneSetupCurrent++;
				vCurrentActor.frameCurrent = 0;
			}
			vProgram.key[VK_F6] = false;
		}

		if(vProgram.key[VK_F7]) {
			vCurrentActor.linkUseDetailModel ^= 1;
			vProgram.key[VK_F7] = false;
		}

		if(vProgram.key[VK_F8]) {
			vCurrentActor.linkAgeSwitch ^= 1;
			if(!vCurrentActor.linkAgeSwitch) {
				vActors[0].ObjectNumber = 0x14;
				vActors[0].AltObjectNumber = 0x15;
			} else {
				vActors[0].ObjectNumber = 0x15;
				vActors[0].AltObjectNumber = 0x14;
			}
			initActorParsing(-1);
			vProgram.key[VK_F8] = false;
		}

		if(vProgram.key[VK_F9]) {
			ca_Reset();
			vProgram.key[VK_F9] = false;
		}

		if(vProgram.key[VK_F12]) {
			vProgram.enableHUD ^= 1;
			vProgram.key[VK_F12] = false;
		}

		if(vProgram.key[VK_DIVIDE]) {
			if(vProgram.targetFPS > 15.0f) vProgram.targetFPS -= 15.0f;
			vProgram.key[VK_DIVIDE] = false;
		}

		if(vProgram.key[VK_MULTIPLY]) {
			if(vProgram.targetFPS <= 60.0f) vProgram.targetFPS += 15.0f;
			vProgram.key[VK_MULTIPLY] = false;
		}

		if(vProgram.key[VK_ADD]) {
			if(vProgram.animPlay) vProgram.animPlay = false;
			if(vCurrentActor.frameCurrent < vCurrentActor.frameTotal) vCurrentActor.frameCurrent++;
			else vCurrentActor.frameCurrent = 0;
			vProgram.key[VK_ADD] = false;
		}

		if(vProgram.key[VK_SUBTRACT]) {
			if(vProgram.animPlay) vProgram.animPlay = false;
			if(vCurrentActor.frameCurrent > 0) vCurrentActor.frameCurrent--;
			else vCurrentActor.frameCurrent = vCurrentActor.frameTotal;
			vProgram.key[VK_SUBTRACT] = false;
		}

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

int WinAPIExit()
{
	DisableOpenGL(hwnd, hDC, hRC);

	DestroyWindow(hwnd);

	return EXIT_SUCCESS;
}

int WinAPISetWindowTitle(char * WndTitle)
{
	return SetWindowText(hwnd, WndTitle);
}

int WinAPISetWindowSize(int Width, int Height)
{
	vProgram.windowWidth = Width;
	vProgram.windowHeight = Height;
	return SetWindowPos(hwnd, NULL, 0, 0, vProgram.windowWidth, vProgram.windowHeight, SWP_NOMOVE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case WM_DESTROY:
			return EXIT_SUCCESS;

		case WM_SIZE: {
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			vProgram.windowWidth = rcClient.right;
			vProgram.windowHeight = rcClient.bottom;
			gl_SetupScene3D(vProgram.windowWidth, vProgram.windowHeight);
			break; }

		case WM_KEYDOWN:
			vProgram.key[wParam] = true;
			break;
		case WM_KEYUP:
			vProgram.key[wParam] = false;
			break;

		case WM_LBUTTONDOWN: {
			SetFocus(hwnd);
			vProgram.mouseButtonLDown = true;
			vProgram.mouseCenterX = (signed int)LOWORD(lParam);
			vProgram.mouseCenterY = (signed int)HIWORD(lParam);
			break; }
		case WM_LBUTTONUP: {
			vProgram.mouseButtonLDown = false;
			break; }

		case WM_MOUSEMOVE: {
			if(vProgram.mouseButtonLDown) {
				vProgram.mousePosX = (signed int)LOWORD(lParam);
				vProgram.mousePosY = (signed int)HIWORD(lParam);
				ca_MouseMove(vProgram.mousePosX, vProgram.mousePosY);
				ca_Orientation(vCamera.angleX, vCamera.angleY);
			}
			break; }

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return EXIT_SUCCESS;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	*hDC = GetDC(hwnd);

	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
				  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}
#endif
