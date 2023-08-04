#pragma once

template<typename T>
//싱글턴은 멀티스레드 사용시 오류가난다
//1. 해당 객체로 만들 수 있는게 하나여야만 한다
//2. 코드 아무곳에서나 선언이 가능해야한다
class Singleton
{
public:
    static T& Instance()
    {
        static T instance;
        return instance;
    }
    //데이터가 프로그램 시작/종료까지 유지된다

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    //기본생성자와 대입연산자의 복사생성자를 자동으로 추가하지 않음


protected:
    Singleton() = default;
    virtual ~Singleton() = default;
    //Singleton() ={}와 같다 다만 컴파일 할때 구현부없이
    //cpp없이생성. 다만protected인 이유는 클래스 내부에서만
    //생성될 수 있게하기위함이다(바깥에서 만들방법 없음)
};
