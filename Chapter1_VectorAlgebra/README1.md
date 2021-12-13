# Chapter1. 벡터 대수

## 요약
-------------------------------------------------

* 벡터의 정의, 덧셈뺄셈, 스칼라곱셈, 길이, 정규화, 내적, 외적
* DirectX Math, XMVECTOR

### 벡터의 정의, 덧셈뺄셈, 스칼라곱셈, 길이, 정규화, 내적, 외적
-------------------------------------------------

- Direct 3D는 왼손잡이 좌표계를 사용한다. (양의 방향 Z축이 안쪽을 향한다.)
- 따로 적지 않고 넘어간다.

### DirectX Math, XMVECTOR
-------------------------------------------------

- DirectX Math 라이브러리는 SSE2 명령 집합을 활용한다.

- SIMD 명령을 사용하여 32비트 float 네 개를 하나의 명령에서 처리한다. -> 벡터를 효율적으로 처리할 수 있다.

- XMVECTOR 라는 128 비트 (32비트 부동소수점 x 4) 크기의 벡터 형식을 사용한다. 

- 지역 변수, 전역 변수에서는 XMVECTOR를 사용하며, 클래스 자료 멤버에는 XMFLOAT2, XMFLOAT3, XMFLOAT4 를 사용한다. 계산 전에 XMFLOATn을 XMVECTOR로 변환하여 계산한다.

- 상수 벡터를 정의할 때에는 XMVECTORF32 형식을 사용한다.

```C++
static const XMVECTORF32 g_vHalfVector = { 0.5f, 0.5f, 0.5f, 0.5f };
```

- XMVECTOR 인스턴스를 인수로 함수를 호출할 때, 효율성을 위해 스택이 아닌 SSE/SSE2 레지스터를 통해 함수에 전달해야 한다. 이를 플랫폼 독립적으로 처리하기 위해, XMVECTOR 매개변수에 FXMVECOTR, GXMVECTOR, HXMVECTOR 형식을 지정해야한다. 

1~3번째 XMVECTOR 매개변수에는 FXMVECTOR,
4번째 XMVECTOR 매개변수에는 GXMVECTOR,
5~6번째 XMVECTOR에는 HXMVECTOR,
그 이상의 XMVECTOR 매개변수에는 CXMVECTOR를 사용한다.

```C++
inline XMMATRIX XM_CALLCONV XMMatrixTransformation(
    FXMVECTOR ScalingOrigin,
    FXMVECTOR ScalingOrientationQuaternion,
    FXMVECTOR Scaling,
    GXMVECTOR RotationOrigin,
    HXMVECTOR RotationQuaternion,
    HXMVECTOR Translation
);
```

- DirectXMath 라이브러리는 XMVECTOR를 이용한 벡터 덧셈, 뺄셈 등등을 위해 연산자들을 제공한다. 