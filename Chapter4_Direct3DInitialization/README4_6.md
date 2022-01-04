# Chapter4_6. Direct3D의 초기화 - Direct3D 응용 프로그램의 디버깅

### Direct3D 응용 프로그램의 디버깅
--------------------------------------------------
* Direct3D의 여러 함수는 HRESULT 형식의 error code를 돌려준다.
```C++
#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif
```
* ThrowIfFailed는 함수가 아닌 매크로로 정의해야 한다. 
* ThrowIfFailed를 함수로 구현하면 __FILE__과 __LINE__이 ThrowIfFailed 호출이 있는 파일과 행 번호가 아닌 함수가 구현된 파일과 행 번호로 치환된다.
* 만약 응용 프로그램이 실패하면, ThrowIfFailed가 예외를 던지며, 응용 프로그램 전체가 try/catch 블록 안에 존재한다면, catch 절의 MessageBox 함수를 이용하여 오류 메시지를 출력할 수 있다.

