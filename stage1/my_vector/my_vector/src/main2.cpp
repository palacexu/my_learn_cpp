#include<iostream>


//container space allocater
template<typename T>
class Allocator 
{
public:
	T* allocate(int size) { //为容器开辟内存空间
		return (T*)malloc(sizeof(T) * size);
	}
	void deallocate(T* ptr) { //释放容器所在内存空间
		free(ptr);
	}
	void construct(T* adr, const T& val) { //执行类类型的拷贝构造  构造一个元素
		new (adr) T(val);
	}
	void destroy(T* p) { //负责对象析构
		p->~T();
	}
};



//vector class template   模版类型参数   模版非类型参数
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
		//析构每一个元素
		for (T* p = _first; p != _last; ++p) {
			alloc.destroy(p);
		}
		//释放内存
		alloc.deallocate(_first);
		_first = _last = _end = nullptr;
	}
	//拷贝构造函数
	Vector(const Vector<T>& src) {
		_first = alloc.allocate(src._end-src._first); //开辟内存
		int len = src._last - src._first;  //元素个数
		for (int i = 0; i < len; i++) { //构造相同的元素
			alloc.construct(_first + i, src._first[i]);
		}
		_last = _first + len;
		_end = _first + (src._end - src._first);
	}
	//赋值重载函数
	Vector<T>& operator= (const Vector<T>& src){
		if (this == &src) { //可以取地址吗？
			return *this;
		}
		// 销毁容器内的元素对象
		for (T* p = _last; p != _first; --p) {
			alloc.destroy(p);
		}
		// 释放容器内存空间
		alloc.deallocate(_first);
		// 为容器开辟新内存
		_first = alloc.allocate(src._end - src._first); //开辟内存
		int len = src._last - src._first;  //元素个数
		for (int i = 0; i < len; i++) { //为容器构造src容器相同的元素
			alloc.construct(_first + i, src._first[i]);
		}
		_last = _first + len;
		_end = _first + (src._end -src._first);
		return *this;
	}
	//向容器尾部加入元素
	void push_back(const T& val) {
		if (full()) {
			expand();
		}
		//构造元素 加入容器
		alloc.construct(_last, val);
		_last ++;
	}
	//删除容器末尾元素
	void pop_back() {
		if (!empty()) {
			//alloc.destroy(--_last); //顺序
			--_last;
			alloc.destroy(_last);
		}
		return;
	}
	//返回容器末尾元素
	T back() {
		T tmp(*(_last-1));
		return tmp;
	}
	bool full() const { return _last == _end; }
	bool empty() const { return _first == _last; }
	int size() const { return _end - _first; }
	T& operator[](int index) {
		if (index < 0 || index >= size()) {
			throw "OutOfRangeException";
		}
		else {
			return *(_first + index);
		}
	}
	//迭代器一般实现为容器的嵌套类型
	class Iterator {
	public:
		//构造函数
		//Iterator(const T* &p)
		//	:_ptr(p) {}
		Iterator(T* p)
			:_ptr(p) {}
		bool operator!=(const Iterator& iter) const {
			return _ptr != iter._ptr;
		}
		// TUDO: 能设置为2个？
		T& operator*() { // T var2();  T var1 = *_ptr;  *_ptr = var2;
			return *_ptr;
		}
		const T& operator*() const { // T var1 = *_ptr
			return *_ptr;
		}
		void operator++() {
			 ++_ptr;
		}
	private:
		T* _ptr;
	};
	//需要给容器提供begin和end方法
	Iterator begin() { return Iterator(_first); }
	Iterator end() { return Iterator(_end); }

private:
	T* _first;
	T* _last;
	T* _end;
	ALLOC alloc; //定义容器的空间配置器对象
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
	Test(const int&a=10, const int& b=20, const int& c=30) 
	:x(a),
	y(b),
	z(c) { std::cout << "Test()" << std::endl; }
	~Test() {
		std::cout << "~Test()" << std::endl;
	}
	//拷贝构造
	Test(const Test& src) {
		x = src.x;
		y = src.y;
		z = src.z;
		std::cout << "Test(const Test&)" << std::endl;
	}

private:
	int x, y, z;
	friend std::ostream& operator<<(std::ostream& out, const Test& src);
};

std::ostream& operator<<(std::ostream& out, const Test& src) {
	std::cout << src.x << "  " << src.y << "  " << src.z;
	return out;
}

int main() {
	Test t1(1,2,3);
	Test t2(90), t3(3,6,9);
	std::cout << "--------Test类对象初始化完成----------" << std::endl;
	Vector<Test> vec(3);
	vec.push_back(t1);
	vec.push_back(t2);
	vec.push_back(t3);
	for (int i = 0; i < vec.size(); i++) {
		std::cout << vec[i] << std::endl;
	}
	std::cout << "-----------迭代器-----------" << std::endl;
	Vector<Test>::Iterator a = vec.begin();
	for (auto p = vec.begin(); p != vec.end(); ++p) {
		std::cout << *p << std::endl;
	}
	std::cout << "--------范围基的 for 循环---------" << std::endl;
	for (auto &element :vec) {
		std::cout << element << std::endl;
	}
	std::cout << "--------出Vector容器2个---------" << std::endl;
	vec.pop_back();
	vec.pop_back();
	std::cout << "--------出Vector容器2个完成------" << std::endl;
	std::cin.get();
	return 0;
}
