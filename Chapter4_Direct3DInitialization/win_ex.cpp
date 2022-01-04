#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

LRESULT CALLBACK WindowProc(
    HWND hwnd, // 창 핸들
    UINT uMsg, // 메시지 코드
    WPARAM wParam, // 메시지에 대한 추가 데이터
    LPARAM lParam  // 메시지에 대한 추가 데이터
);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Example Window";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc; // 창 프로시저
    wc.hInstance = hInstance; // 응용 프로그램 인스턴스 핸들
    wc.lpszClassName = CLASS_NAME; // 창 클래스 식별 문자열

    RegisterClass(&wc); // windows 클래스를 운영체제에 등록한다.

    HWND hwnd = CreateWindowEx(
        0, // 창의 동작 지정 
        CLASS_NAME, // 창 클래스 이름
        L"LALALALA", // 창 텍스트 (제목 표시줄에 표기됨)
        WS_OVERLAPPEDWINDOW, // 창의 모양 중 일부를 정의하는 플래그

        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // 위치, 크기

        NULL, // 부모 윈도우
        NULL, // 메뉴
        hInstance, // 인스턴스 핸들
        NULL // 데이터 전달 포인터
    );

    if (hwnd == NULL) return 0;
    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg); // 키 입력을 문자로 변환
        DispatchMessage(&msg); // 창 프로시저 호출
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}