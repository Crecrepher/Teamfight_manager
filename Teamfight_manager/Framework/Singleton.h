#pragma once

template<typename T>
//�̱����� ��Ƽ������ ���� ����������
//1. �ش� ��ü�� ���� �� �ִ°� �ϳ����߸� �Ѵ�
//2. �ڵ� �ƹ��������� ������ �����ؾ��Ѵ�
class Singleton
{
public:
    static T& Instance()
    {
        static T instance;
        return instance;
    }
    //�����Ͱ� ���α׷� ����/������� �����ȴ�

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    //�⺻�����ڿ� ���Կ������� ��������ڸ� �ڵ����� �߰����� ����


protected:
    Singleton() = default;
    virtual ~Singleton() = default;
    //Singleton() ={}�� ���� �ٸ� ������ �Ҷ� �����ξ���
    //cpp���̻���. �ٸ�protected�� ������ Ŭ���� ���ο�����
    //������ �� �ְ��ϱ������̴�(�ٱ����� ������ ����)
};
