#ifndef __MULTI_ARRAY__
#define __MULTI_ARRAY__

#include "defs.h"

class Array {
private:
	/* define all the type */
	enum class Type : unsigned int {
		DEFAULT_TYPE,
		CHAR,
		UNSIGNED_CHAR,
		SHORT,
		UNSIGNED_SHORT,
		INT,
		UNSIGNED_INT,
		LONG,
		UNSIGNED_LONG,
		LL,
		UNSIGNED_LL,
		FLOAT,
		DOUBLE,
		LONG_DOUBLE,
		WCHAR,
		STRING,
		PDATA
	};

	wstring * array;
	Type * decoder;
	int dimension;
	int size;
	int * array_size;

	int refer_pos_start;
	int refer_pos_end;
	int refer_dim;
	mt19937 gen_1;
	int precision;

#ifdef _WIN32
	void gotoxy(short x, short y);
	short wherey();
	short wherex();
#endif

	wstring To_wstr(const string & str);
	string To_str(const wstring & wstr);
	wstring Type2str(Type type);
	void init_param();

	template <typename T>
	Type declare_type(const T & value);

	template <typename T>
	T gen_randn(T min, T max);

	template <typename T>
	wstring to_string_with_precision(const T a_value, const int n = 6);

	void Alloc_Array(vector<unsigned int> info);
	void Dealloc_Array();

	vector<wstring> copy_value();
	vector<int> copy_size();
	vector<Type> copy_decoder();

	/* Copy */
	void copy(vector<wstring> data, int dim, vector<int> array_size, vector<Type> decoder);

	/* Convert value from wstring to its original type */
	void Convert_Value(int & in, int idx);
	void Convert_Value(char & in, int idx);
	void Convert_Value(wchar_t & in, int idx);
	void Convert_Value(short & in, int idx);
	void Convert_Value(unsigned short & in, int idx);
	void Convert_Value(unsigned char & in, int idx);
	void Convert_Value(unsigned int & in, int idx);
	void Convert_Value(long & in, int idx);
	void Convert_Value(unsigned long & in, int idx);
	void Convert_Value(long long & in, int idx);
	void Convert_Value(unsigned long long & in, int idx);
	void Convert_Value(float & in, int idx);
	void Convert_Value(double & in, int idx);
	void Convert_Value(long double & in, int idx);
	void Convert_Value(string & in, int idx);
	void Convert_Value(wstring & in, int idx);

	/* Give out value */
	template <typename T>
	void Push_Value(T & data, int idx);

protected:
	wstring name;
public:

	/* Constructor and Destructor */
	Array(wstring name);
	Array(const Array & new_array);
	~Array();

	/* Array info */
	const int dim();
	const int get_size(const int idx);
	const int get_size();
	wstring get_name();
	void info();
	int type();
	vector<unsigned int> shape();

	/* Array private function */
	void print(int enter_posx = 90);
	void set_precision(unsigned int num);
	wstring get_raw();
	wstring Array2str();
	void change_name(wstring new_name);

	/* Initializer */
	void zeros(unsigned int args, ...);
	void ones(unsigned int args, ...);
	void randn(unsigned int args, ...);
	void init(vector<unsigned int> size);

	/* idx access */
	Array& operator[](int idx);

	/*********** operator ************/

	/* Get value */
	Array& operator=(Array & in);
	Array& operator<<(Array & in);

	template <typename T>
	Array& operator=(const T in);
	Array& operator=(string & in);
	Array& operator=(wstring & in);
	Array& operator=(const char * in);
	Array& operator=(const wchar_t * in);

	template <typename T>
	Array& operator<< (const T in);
	Array& operator<< (string & in);
	Array& operator<< (wstring & in);
	Array& operator<< (const char * in);
	Array& operator<< (const wchar_t * in);

	template <typename T>
	void put_value(const T in, unsigned int pos);
	void put_value(string & in, unsigned int pos);
	void put_value(wstring & in, unsigned int pos);
	void put_value(const char * in, unsigned int pos);
	void put_value(const wchar_t * in, unsigned int pos);
	void put_value(wstring & in, unsigned int pos, unsigned int type);

	/* Give out Value */
	template <typename T>
	Array& operator>> (T & in);
	Array& operator>> (Array & value);
	template <typename T>
	void get_value(T & in, unsigned int pos);

	/* Compare Value */
	template <typename T>
	bool operator== (const T in);
	bool operator== (string & in);
	bool operator== (wstring & in);
	bool operator== (const char * in);
	bool operator== (const wchar_t * in);
	bool operator==(Array & in);

	template <typename T>
	bool operator!= (const T in);
	bool operator!= (string & in);
	bool operator!= (wstring & in);
	bool operator!= (const char * in);
	bool operator!= (const wchar_t * in);
	bool operator!=(Array & in);

	template <typename T>
	bool operator>= (const T in);

	template <typename T>
	bool operator<= (const T in);

	template <typename T>
	bool operator> (const T in);

	template <typename T>
	bool operator< (const T in);
};

/* Template function */

/* Get value */
template <typename T>
inline void Array::Push_Value(T & data, int idx) {
	try {
		if (idx >= this->size || idx < 0)
			throw runtime_error(":MA0011");
		if (decoder[idx] != declare_type(data))
			throw runtime_error(":MA0012");
	}
	catch (exception& e) {
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
	}
	Convert_Value(data, idx);
}

template <typename T>
inline Array& Array::operator=(const T in) {
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0013");
		this->decoder[refer_pos_start] = declare_type(in);
		this->array[refer_pos_start] = to_string_with_precision(in, precision);
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return (*this); // Error
	}
	init_param();
	return (*this);
}

template<typename T>
inline Array & Array::operator<<(const T in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0014");
		this->decoder[refer_pos_start] = declare_type(in);
		this->array[refer_pos_start] = to_string_with_precision(in, precision);
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return (*this); // Error
	}
	init_param();
	return (*this);
}

template<typename T>
inline void Array::put_value(const T in, unsigned int pos)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (pos > this->size)
			throw runtime_error(":MA0018");
		this->decoder[pos] = declare_type(in);
		this->array[pos] = to_string_with_precision(in, precision);
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
	}
	init_param();
}

template <typename T>
inline Array::Type Array::declare_type(const T & value) {
	Type type;
	if (typeid(value) == typeid(char))
		type = Type::CHAR;
	else if (typeid(value) == typeid(unsigned char))
		type = Type::UNSIGNED_CHAR;
	else if (typeid(value) == typeid(short))
		type = Type::SHORT;
	else if (typeid(value) == typeid(unsigned short))
		type = Type::UNSIGNED_SHORT;
	else if (typeid(value) == typeid(int))
		type = Type::INT;
	else if (typeid(value) == typeid(unsigned int))
		type = Type::UNSIGNED_INT;
	else if (typeid(value) == typeid(long))
		type = Type::LONG;
	else if (typeid(value) == typeid(unsigned long))
		type = Type::UNSIGNED_LONG;
	else if (typeid(value) == typeid(long long))
		type = Type::LL;
	else if (typeid(value) == typeid(unsigned long long))
		type = Type::UNSIGNED_LL;
	else if (typeid(value) == typeid(float))
		type = Type::FLOAT;
	else if (typeid(value) == typeid(double))
		type = Type::DOUBLE;
	else if (typeid(value) == typeid(long double))
		type = Type::LONG_DOUBLE;
	else if (typeid(value) == typeid(wchar_t *))
		type = Type::DEFAULT_TYPE;
	else if (typeid(value) == typeid(char *))
		type = Type::STRING;
	else if (typeid(value) == typeid(wchar_t))
		type = Type::WCHAR;
	else if (typeid(value) == typeid(string))
		type = Type::STRING;
	else
		type = Type::DEFAULT_TYPE;
	return type;
}

template <typename T>
inline T Array::gen_randn(T min, T max) {
	uniform_real_distribution<T> dist(min, max);
	return dist(gen_1);
}

template <typename T>
inline wstring Array::to_string_with_precision(const T a_value, const int n)
{
	wostringstream out;
	out << setprecision(n) << a_value;
	return out.str();
}

template <typename T>
inline Array& Array::operator>> (T & in) {
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0015");
		Push_Value(in, refer_pos_start);
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return (*this); // Error
	}
	init_param();
	return (*this);
}

template<typename T>
inline void Array::get_value(T & in, unsigned int pos)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (pos > this->size)
			throw runtime_error(":MA0023");
		Push_Value(in, pos);
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
	}
	init_param();
}

template<typename T>
inline bool Array::operator==(const T in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0031"); // 
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (in == this->array[this->refer_pos_start]) {
		init_param();
		return true;
	}
	else {
		init_param();
		return false;
	}

}

template<typename T>
inline bool Array::operator!=(const T in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0032"); // 
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (in == this->array[this->refer_pos_start]) {
		init_param();
		return false;
	}
	else {
		init_param();
		return true;
	}
}

template<typename T>
inline bool Array::operator>=(const T in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0033"); // 
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (in <= this->array[this->refer_pos_start]) {
		init_param();
		return true;
	}
	else {
		init_param();
		return false;
	}
}

template<typename T>
inline bool Array::operator<=(const T in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0034"); // 
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (in >= this->array[this->refer_pos_start]) {
		init_param();
		return true;
	}
	else {
		init_param();
		return false;
	}
}

template<typename T>
inline bool Array::operator>(const T in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0035"); // 
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (in < this->array[this->refer_pos_start]) {
		init_param();
		return true;
	}
	else {
		init_param();
		return false;
	}
}

template<typename T>
inline bool Array::operator<(const T in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0036"); // 
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (in > this->array[this->refer_pos_start]) {
		init_param();
		return true;
	}
	else {
		init_param();
		return false;
	}
}

#endif
