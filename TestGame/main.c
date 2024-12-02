#include "ElfMath.h"
#include "Elf2D.h"
#include "ElfTime.h"
#include "ElfObject.h"
#include "stdio.h"
#include <stdlib.h>
#include <windows.h>

#define WIDTH 40
#define HEIGHT 24

char screenBuffer[WIDTH * HEIGHT * 2 + 1]; // null 문자를 위해 +1 추가

// 스크린 버퍼 초기화
void bufferClear() {
    for (int i = 0; i < WIDTH * HEIGHT * 2; i++) {
        screenBuffer[i] = ' '; // 초기값은 공백 문자로 채움
    }
    screenBuffer[WIDTH * HEIGHT * 2] = '\0'; // 문자열 끝을 null로 설정

    //커서 숨기기
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;  // 커서를 숨김
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void Initialize(GameObject_Line* obj, int objNum)
{
    //객체 초기화(관절 3개)
    int i = 0;
    for (i = 0; i < 3; i++)
    {
        obj[i].Position.x = 0;
        obj[i].Position.y = 0;

        obj[i].Rotation = 0;


        //(0,0) ~ (3,0) 을 잇는 직선
        obj[i].Line[0].x = 0;
        obj[i].Line[0].y = 0;

        obj[i].Line[1].x = WIDTH;
        obj[i].Line[1].y = HEIGHT ;
    }

    obj[1].Rotation = 45;
    obj[2].Rotation = 0;

    //화면 출력 심볼 입력
    obj[0].Symbol = "11";
    obj[1].Symbol = "22";
    obj[2].Symbol = "33";

    //크기 입력
    obj[0].Scale.x = 1;
    obj[0].Scale.y = 1;
    obj[1].Scale.x = 2;
    obj[1].Scale.y = 2;
    obj[2].Scale.x = 2;
    obj[2].Scale.y = 2;
}

// 0: 입력없음
// 99 : 게임 종료
int Input()
{
    // ESC 키를 눌렀는지 확인
    if (_kbhit()) {
        char ch = _getch();
        if (ch == 27) {  // ESC 키가 눌리면 게임 종료
            return 99;
        }
    }

    return 0;
}

void Update(GameObject_Line* obj, int objNum, int e)
{    
    
}

void Render(GameObject_Line* obj, int objNum, char* Buf, int width, int height)
{
    Vector3 lineA, lineB;
    Matrix3x3 world = identity_matrix();    //단위행렬로 초기화

    lineB.x = 0;
    lineB.y = 0;

    int i = 0;
   
        obj[i].Position.x = lineB.x;
        obj[i].Position.y = lineB.y;

        lineA.x = obj[i].Line[0].x;
        lineA.y = obj[i].Line[0].y;
        lineA.z = 1;
        lineB.x = obj[i].Line[1].x;
        lineB.y = obj[i].Line[1].y;
        lineB.z = 1;

        Matrix3x3 scale = scale_matrix(obj[i].Scale.x, obj[i].Scale.y); // 크기조정 행렬 적용
        Matrix3x3 rotation = rotation_matrix(obj[i].Rotation);// 회전 행렬 생성
        Matrix3x3 translation = translation_matrix(obj[i].Position.x, obj[i].Position.y); // 이동 행렬 적용
        
        world = multiply_matrices(scale, world);
        world = multiply_matrices(rotation, world);
        world = multiply_matrices(translation, world);
        lineA = multiply_matrix_vector(world, lineA);  // 점과 회전 행렬 곱셈 (Matrix3x3 * Vector3)
        lineB = multiply_matrix_vector(world, lineB);  // 점과 회전 행렬 곱셈 (Matrix3x3 * Vector3)

        
        Elf2DDrawLine2((float)lineA.x, (float)lineA.y, (float)lineB.x, (float)lineB.y, Buf, width, height);

}


// 게임 루프
int main() {
    int fps = 30;
    double frameTime = 1000.0 / fps;

    // 전역 변수로 스크린 버퍼 선언
    char screenBuffer[WIDTH * HEIGHT + 1];
    int screenWidth = WIDTH ;
    int screenHeight = HEIGHT;

    GameObject_Line LineObj[3];

    // 게임 초기화
    bufferClear();
    Initialize(LineObj, 3);
    
    Elf2DInitScreen();
    Elf2DClearScreen(screenBuffer, screenWidth, screenHeight);
    Elf2DDrawBuffer(screenBuffer);
    
    ElfTimeInitialize();
    
    int isGameRunning = 1;
    while (isGameRunning) {
        
        int gameEvent = Input();
        
        //입력 계산
        if (gameEvent == 99)
        {
            isGameRunning = 0;
        }

        // 게임 업데이트
        Update(LineObj, 3, gameEvent);
        
        // 스크린버퍼에 게임 렌더링
        Render(LineObj, 3, screenBuffer, screenWidth, screenHeight);

        // 화면 그리기
        Elf2DDrawBuffer(screenBuffer);
        
        ElfTimeCalculateDeltaTime();
        Elf2DSleep( frameTime - ElfTimeGetDeltaTime() );
    }

    return 0;
}