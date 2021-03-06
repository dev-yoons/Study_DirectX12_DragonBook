# Chapter5_IA_VS. 렌더링 파이프라인 (테셀레이션 ~ 출력병합기)

### 테셀레이션 단계들
--------------------------------------------------
* 테셀레이션은 주어진 메시의 삼각형들을 더 잘게 쪼개서 새로운 삼각형을 만드는 과정을 말한다. 새 삼각형들을 새로운 위치로 이동하여 세부적인 특징을 만들어낼 수 있다.
* 테셀레이션은 다음과 같은 장점이 있다.
  * 카메라에 가까운 삼각형들에 테셀레이션을 적용해서 세부도를 높이고, 먼 삼각형들에는 테셀레이션을 적용하지 않는 방식의 세부수준 메커니즘을 구현할 수 있다.
  * 메모리에는 저다각형 메시 (적은 수의 삼각형들로 이루어진 메시)를 담고, 즉석에서 삼각형을 추가하여 메모리를 절약할 수 있다.
  * 애니메이션이나 물리 처리 같은 연산들을 저다각형 메시에 대해 수행하고, 테셀레이션된 고다각형 메시는 렌더링에만 사용하여 계산량을 줄일 수 있다.
* 테셀레이션 단계는 Direct3D 11에 새로 도입되었다.
* Direct3D 11 이전에는 CPU에서 삼각형을 분할하여 새 기하구조를 만들고, 그것을 다시 GPU에 올려서 렌더링 했어야 했다. 하지만 새 기하구조를 CPU메모리에서 GPU 메모리로 올리는 연산이 느리고, CPU에서 테셀레이션을 계산하는 것이 부담되기 때문에 잘 사용되지 않았다. 
* 하지만 Direct3D 11 이 제공하는 API를 이용하면 테셀레이션이 전적으로 하드웨어에서 일어난다.
* 테셀레이션 단계들은 선택적이고, 생략할 수 있다.

### 기하 셰이더 단계
--------------------------------------------------
* 기하 셰이더 단계도 선택적이다.
* 기하 셰이더는 하나의 온전한 기본도형을 입력받아 임의로 변형한다. 예를 들어 삼각형 목록을 그리는 경우, 정점 셰이더의 처리를 거친 정점 세 개가 기하 셰이더에 입력된다. 
* 기하 셰이더의 주된 장점은 기하구조를 GPU에서 생성/파괴할 수 있다는 것이다. 입력 기하구조를 여러 개의 기하구조들로 확장할 수도 있고, 폐기할 수도 있다.
* 기하 셰이더의 흔한 용도는 점이나 선분을 사각형으로 확장하는 것이다.
* 기하 셰이더의 출력을 스트림 출력 단계를 통해 메모리의 버퍼에 저장해두고 나중에 활용할 수 있다.

### 절단
--------------------------------------------------
* 시야 절두체 밖의 기하구조는 폐기해야 하며, 절두체의 경계면과 교차하는 기하구조는 잘라내야 한다. 이를 절단 연산이라고 부른다.
* 시야 절두체는 여섯 개의 평면으로 볼 수 있다. (상,하,좌,우,가까운,먼 평면) 다각형을 절두체로 절단하려면 여섯 평면으로 각각 절단해야 한다. 
* 절단 연산은 서덜런드-호지먼 절단 알고리즘을 살펴보면 좋다.
  
### 래스터화 단계
--------------------------------------------------
* 래스터화 단계의 주 임무는 투영된 3차원 삼각형으로부터 픽셀 색상들을 계산하는 것이다.
#### 뷰포트 변환
* 절단을 마치고 나면 하드웨어는 원근 나누기를 수행해서 동차 절단 공간 좌표를 정규화된 장치 좌표(NDC)로 변환할 수 있다. 일단 정점들이 NDC 공간에 들어오면 점들의 2차원 x,y 좌표 성분들이 후면 버퍼의 한 직사각형 영역으로 변환된다. 그 직사각형 영역이 **뷰포트**이다.
* 뷰포트 변환을 마치고 나면 x,y 성분은 픽셀 단위의 값이 된다. (z성분은 깊이 버퍼링에 사용하므로 변경하지 않는다.)
#### 후면 선별
* 하나의 삼각형에는 면이 두 개가 있다. 두 면을 구분하기 위해 Direct3D는 법선을 사용한다.
* 삼각형 정점들이 $v_0, v_1, v_2$ 의 순서로 감길 때, 삼각형의 법선 $n$은 다음과 같다.
$$
e_0 = v_1 - v_0 \\
e_1 = v_2 - v_0 \\
n = \frac{e_0 \times e_1}{e_0 \times e_1}
$$
* 법선이 가리키는 방향이 삼각형의 앞쪽, 반대쪽이 뒤쪽이 된다.
* 관찰자에게 보이는 면이 앞쪽 면인 삼각형을 *전면 삼각형* 이라고 불느다. 반대는 *후면 삼각형* 이다. 전면 삼각형은 시계방향으로, 후면 삼각형은 반시계방향으로 감긴다.
* 물체를 구성하는 삼각형들의 법선이 항상 물체의 바깥쪽을 향하도록 모형을 만들 때, 카메라에는 다면체의 후면 삼각형들이 보이지 않는다. 후면 삼각형들은 모두 전면 삼각형들에 가려지기 때문이다. 
* 파이프라인에서 그러한 후면 삼각형을 골라서 폐기하는 공정을 **후면 선별** 이라고 부른다.
* 기본적으로 Direct3D는 (관찰자기준) 시계방향으로 감긴 삼각형을 전면, 반시계방향으로 감긴 삼각형을 후면으로 간주한다. 설정에 따라 반대도 가능하다.

#### 정점 특성의 보간
* 뷰포트 변환을 거친 후에는 정점의 특성들 (법선 벡터, 텍스처 좌표...)을 삼각형을 덮는 각 픽셀에 대해 보간해야 한다. 
* 정점의 깊이 값도 그런 식으로 보간해서, 각 픽셀에 깊이 버퍼링 알고리즘을 위한 깊이 값을 부여해야 한다.
* 정점 특성들은 3차원 공간에서 삼각형의 면을 따라 선형으로 보간되지만, 보간된 특성을 그대로 화면 공간에 사상하면 간격이 고르지 않다.
* 하지만 Direct3D에 쓰이는 **원근 보정 보간**이라고 하는 보간 방법에서는 그런 문제가 발생하지 않는다. 이러한 정점 특성 보간 덕분에 삼각형 내부 픽셀들을 위한 값들을 정점에 부착된 값들로부터 계산할 수 있다.


### 픽셀 셰이더 단계
--------------------------------------------------
* 픽셀 셰이더(PS)는 프로그래머가 작성하고 GPU가 실행하는 프로그램이다.
* 픽셀 셰이더는 각각의 픽셀 단편 (최종적인 픽셀 색상을 계산하기 위한 중간 픽셀 자료)에 적용된다.
* 픽셀 셰이더는 보간된 정점 특성들을 입력받아 하나의 색상을 출력한다. 

### 출력 병합기 단계
--------------------------------------------------
* 픽셀 셰이더가 생성한 픽셀 단편들은 렌더링 파이프라인의 출력 병합기 단계로 입력된다.
* 이 단계에서 일부 픽셀 단편들이 기각(폐기)될 수 있다. 폐기되지 않은 픽셀 단편들은 후면 버퍼에 기록된다.
* 혼합(blending) 도 이 단계에서 일어난다. 혼합이란 새 픽셀이 후면 버퍼의 기존 픽셀을 완전히 덮어쓰지 않고 두 픽셀을 일정한 공식에 따라 섞은 결과를 기록하는 것을 말한다. 주로 반투명과 같은 특수 효과를 내는데 사용된다.
