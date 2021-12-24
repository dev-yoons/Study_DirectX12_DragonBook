# Chapter4_3. Direct3D의 초기화 - Direct3D의 초기화

## 요약
--------------------------------------------------

### Direct3D 초기화
--------------------------------------------------
#### 1. 장치 생성
* Direct3D 초기화는 Direct3D 12 장치 (ID3D12Device)를 생성하는 것으로 시작한다. 장치는 디스플레이 어댑터를 나타내는 객체이다. 
장치를 생성할 때에는 다음과 같은 함수를 사용한다.

```C++
HRESULT WINAPI D3D12CreateDevice(
    IUnknown* pAdapter,
    D3D_FEATURE_LEVEL MinimumFeatureLevel,
    REFIID riid,
    void** ppDevice
);
```
* pAdapter : 장치가 나타내는 디스플레이 어댑터를 지정한다. 널 포인터를 지정하면 시스템의 기본 디스플레이 어댑터가 사용된다.
* MinimumFeatureLevel : 응용 프로그램이 요구하는 최소 기능 수준 (디스플레이 어댑터가 이 수준을 지원하지 않으면 장치 생성이 실패한다)
* riid : 생성하고자 하는 ID3D12Device 인터페이스의 COM ID
* ppDevice : 생성된 장치가 이 매개변수에 설정된다.

  ```C++
  HRESULT hardwareResult = D3D12CreateDevice(
    nullptr,
    D3D_FEATURE_LEVEL_11_0,
    IID_PPV_ARGS(&md3dDevice)
  );

  if (FAILED(hardwareResult)) { ... }
  ```

#### 2. 울타리 생성과 서술자 크기 얻기
* 장치 생성 후엔 CPU와 GPU의 동기화를 위한 울타리 객체를 생성한다.
* 또한, 이후에 필요한 서술자들의 크기도 미리 조회하여 설정해둔다.

#### 3. 4X MSAA 품질 수준 지원 점검
#### 4. 명령 대기열과 명령 목록 생성
#### 5. 교환 사슬의 서술과 생성
```C++
typedef struct DXGI_SWAP_CHAIN_DESC
{
    DXGI_MODE_DESC BufferDesc;
    DXGI_SAMPLE_DESC SampleDesc;
    DXGI_USAGE BufferUsage;
    UINT BufferCount;
    HWND OutputWindow;
    BOOL Windowed;
    DXGI_SWAP_EFFECT SwapEffect;
    UINT Flags;
} DXGI_SWAP_CHAIN_DESC;
```
  * BufferDesc : 생성하고자 하는 후면 버퍼의 속성들을 서술한다.
  * SampleDesc : 다중표본화 표본 개수와 품질 수준을 서술한다.
  * BufferUsage : 후면 버퍼에 렌더링할 것, DXGI_USAGE_RENDER_TARGET_OUTPUT을 지정한다.
  * BufferCount : 교환 사슬이 사용할 버퍼 개수 (이중 버퍼링이라면 2)
  * OutputWindow : 렌더링 결과가 표시될 창의 핸들
  * Windowed : 창 모드이면 true, 전체화면 모드이면 false
  * SwapEffect : 이 멤버에는 DXGI_SWAP_EFFECT_FLIP_DISCARD 지정
  * Flags : 여기에 DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH 를 지정하면, 응용 프로그램이 전체화면으로 전환될 때 Direct3D가 응용 프로그램 창의 현재 크기에 가장 잘 맞는 디스플레이 모드를 선택한다. 지정하지 않으면 현재 데스크톱 디스플레이 모드를 사용한다.
##
* 교환 사슬을 서술하는 구조체를 다 채우면, IDXGIFactory::CreateSwapChain 메서드를 호출하여 교환 사슬을 생성한다.

#### 6. 서술자 힙 생성
* 응용 프로그램에 필요한 서술자/뷰 들을 담을 서술자 힙을 만들어야 한다.
* 서술자 힙은 ID3D12DescriptorHeap 인터페이스로 대표된다.
* 힙을 생성하는 메서드는 ID3D12Device::CreateDescriptorHeap이다.
* 이번엔 SwapChainBufferCount에 설정된 개수만큼의 렌더대상뷰(RTV)들과 하나의 깊이.스텐실뷰(DSV)가 필요하다. 렌더대상뷰는 교환 사슬에서 렌더링의 대상이 되는 버퍼 자원을, 깊이.스텐실뷰는 깊이 판정을 위한 버퍼 자원을 서술한다.
* 서술자 힙은 서술자의 종류마다 따로 만들어야 한다. 따라서, RTV들을 담을 힙 하나와 하나의 DSV를 담을 힙이 필요하다.

```C++
    ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    ComPtr<ID3D12DescriptorHeap> mDsvHeap;
    void D3DApp::CreateRtvAndDsvDescriptorHeaps()
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
        rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask = 0;
        ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dsvHeapDesc.NodeMask = 0;
        ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
    }
```

#### 7. RTV 생성
* 자원 자체를 직접 파이프라인의 단계에 묶지 않지만, 반드시 자원에 대한 뷰(서술자)를 생성하여 파이프라인 단계에 묶어야 한다.
* 우선 교환 사슬에 저장되어 있는 버퍼 자원을 얻기 위해, 다음과 같은 메서드를 사용한다.
```C++
    HRESULT IDXGISwapChain::GetBuffer(
        UINT Buffer, // 후면 버퍼를 식별
        REFIID riid, // 후면버퍼를 나타내는 ID3D12Resource인터페이스의 COM ID
        void **ppSurface // 후면버퍼를 나타내는 ID3D12Resource 포인터 반환
    );
```
* 이 함수를 호출하면 후면 버퍼의 COM 참조 횟수가 증가하므로 사용한 후에는 반드시 해제해야 한다. ComPtr을 사용하면 자동으로 해제처리된다.
* 렌더 대상 뷰를 생성할 때에는 ID3D12Device::CreateRenderTargetView 메서드를 사용한다.
```C++
    void ID3D12Device::CreateRenderTargetView(
        ID3D12Resource *pResource, // 렌더 대상으로 사용할 자원을 가리키는 포인터
        const D3D12_RENDER_TARGET_VIEW_DESC *pDesc, // 자원에 담긴 원소들의 자료 형식에 관한 멤버를 가지고 있는 구조체에 대한 포인터
        D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor // 렌더 대상 뷰가 저장될 서술자 핸들
    );
```

#### 8. 깊이.스텐실 버퍼와 뷰 생성
* 깊이 버퍼는 가장 가까운 가시 물체들의 깊이 정보를 저장하는 2차원 텍스처이다.
* 따라서 텍스처 자원을 서술하는 D3D12_RESOURCE_DESC 구조체를 채운 후 ID3D12Device::CreateCommittedResource를 호출하면 깊이.스텐실 버퍼를 생성할 수 있다.
* D3D12_RESOURCE_DESC 구조체의 정의는 다음과 같다.


#### 9. 뷰포트 설정

