# "Robot on Arcball Interface"
> OpenGL, GLUT, C++ 이용하여 개발

 
## 1. Robot on Arcball Interface

## a. 프로젝트 설명

#### 1) robot.cpp :

#### 키보드 버튼 누르면 각 case별로 움직임이 달라진다.

- 1 버튼 : Base가 이동되어 로봇 전체가 스페이스 위로 이동 - glTranslated(BaseTransX, 0.00f, BaseTransZ)
- 2 버튼 : Base 위의 Shoulder가 360도로 이동 - glRotated(BaseSpin, 0, 1, 0)
- 3 버튼 : Base 위의 Shoulder와 Elbow가 상하로 이동  - glRotated(ShoulderAng, 0, 0, 1), glRotated(ElbowAng, 0, 0, 1)
- 4 버튼 : 빨간 Wrist가 사방으로 이동 - glRotated(WristAng, 0, 0, 1), glRotated(-WristTwistAng, 1, 0, 0)
- 5 버튼 : 빨간 Finger가 집게처럼 이동 - glRotated(FingerAng1, 0, 0, 1), glRotated(FingerAng2, 0, 0, 1), glRotated(-FingerAng1, 0, 0, 1), glRotated(-FingerAng2, 0, 0, 1)

#### 2) modified_robot.cpp :

robot.cpp을 기본으로 하여 arcball interface를 접목시켰다. (주석에서 for HW4라고 쓰인 부분)

- Arcball 이란?

마우스를 이용한 view 회전으로, 물체 주위에 구를 만들어서 마우스로 물체를 회전시킨다.

arcball.h와 arcball.cpp를 가져와서 이용하였다.


## c. 시연

기본 모습

<img width="500" alt="1" src="https://user-images.githubusercontent.com/41661879/55081785-d43bd500-50e3-11e9-8ca4-3b8766d0cec8.png">

1번, 2번, 3번, 4번, 5번 버튼 모습

<img width="500" alt="2" src="https://user-images.githubusercontent.com/41661879/55081792-d69e2f00-50e3-11e9-9469-d7771e859df4.png">

Arcball

<img width="500" alt="3" src="https://user-images.githubusercontent.com/41661879/55081804-d9991f80-50e3-11e9-8178-82c2e2cf8f62.png">

<img width="500" alt="4" src="https://user-images.githubusercontent.com/41661879/55081803-d9991f80-50e3-11e9-865f-28879ce9093b.png">




## 4. 개발자 정보

홍정수
  
## 5. 최종 업데이트 날짜

2018.10.24 (WED)
