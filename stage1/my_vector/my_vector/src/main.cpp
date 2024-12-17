#include<iostream>
#include<string>

//container space allocater
template<typename T>
class Allocator 
{
public:
	T* allocate(int size) { //Ϊ���������ڴ�ռ�
		return (T*)malloc(sizeof(T) * size);
	}
	void deallocate(T* ptr) { //�ͷ����������ڴ�ռ�
		free(ptr);
	}
	void construct(T* adr, const T& val) { //ִ�������͵Ŀ�������  ����һ��Ԫ��
		new (adr) T(val);
	}
	void destroy(T* p) { //�����������
		p->~T();
	}
};



//vector class template   ģ�����Ͳ���   ģ������Ͳ���
template <typename T, typename ALLOC = Allocator<T>>
class Vector
{
public:
	Vector(int size=10) {
		//_first = (T*)malloc(sizeof(T) * size);
		_first = alloc.allocate(size);
		_last = _first;
		_end = _first + size;
	}
	~Vector() {
		//����ÿһ��Ԫ��
		for (T* p = _first; p != _last; ++p) {
			alloc.destroy(p);
		}
		//�ͷ��ڴ�
		alloc.deallocate(_first);
		_first = _last = _end = nullptr;
	}
	//�������캯��
	Vector(const Vector<T>& src) {
		_first = alloc.allocate(src._end-src._first); //�����ڴ�
		int len = src._last - src._first;  //Ԫ�ظ���
		for (int i = 0; i < len; i++) { //������ͬ��Ԫ��
			alloc.construct(_first + i, src._first[i]);
		}
		_last = _first + len;
		_end = _first + (src._end - src._first);
	}
	//��ֵ���غ���
	Vector<T>& operator= (const Vector<T>& src){
		if (this == &src) { //����ȡ��ַ��
			return *this;
		}
		// ���������ڵ�Ԫ�ض���
		for (T* p = _last; p != _first; --p) {
			alloc.destroy(p);
		}
		// �ͷ������ڴ�ռ�
		alloc.deallocate(_first);
		// Ϊ�����������ڴ�
		_first = alloc.allocate(src._end - src._first); //�����ڴ�
		int len = src._last - src._first;  //Ԫ�ظ���
		for (int i = 0; i < len; i++) { //Ϊ��������src������ͬ��Ԫ��
			alloc.construct(_first + i, src._first[i]);
		}
		_last = _first + len;
		_end = _first + (src._end -src._first);
		return *this;
	}
	//������β������Ԫ��
	void push_back(const T& val) {
		if (full()) {
			expand();
		}
		//����Ԫ�� ��������
		alloc.construct(_last, val);
		_last ++;
	}
	//ɾ������ĩβԪ��
	void pop_back() {
		if (!empty()) {
			//alloc.destroy(--_last); //˳��
			--_last;
			alloc.destroy(_last);
		}
		return;
	}
	//��������ĩβԪ��
	T back() {
		T tmp(*(_last-1));
		return tmp;
	}
	bool full() const { return _last == _end; }
	bool empty() const { return _first == _last; }
	int size() const { return _end - _first; }
	
private:
	T* _first;
	T* _last;
	T* _end;
	ALLOC alloc; //���������Ŀռ�����������
	void expand() {
		int size = _end - _first;
		int len = _last - _first;
		T* tmp = alloc.allocate(size * 2);
		for (T* p = _first; p != _last; ++p) {
			alloc.construct(tmp++, *(p));
			alloc.destroy(p);
		}
		alloc.deallocate(_first);
		_first = tmp;
		_last = _first + len;
		_end = _first + 2*size;
	}
};

class Test {
public:
	Test() {
		std::cout << "Test()" << std::endl;
	}
	~Test() {
		std::cout << "~Test()" << std::endl;
	}
	//��������
	Test(const Test&) {
		std::cout << "Test(const Test&)" << std::endl;
	}
};

int main() {
	Test t1;
	Test t2, t3;
	std::cout << "---------------------" << std::endl;
	Vector<Test> vec;
	vec.push_back(t1);
	vec.push_back(t2);
	vec.push_back(t3);
	std::cout << "---------------------" << std::endl;
	vec.pop_back();
	vec.pop_back();
	std::cout << "---------------------" << std::endl;
	std::cout << "-------�������������main()----------" << std::endl;
	std::cin.get();
	return 0;
}