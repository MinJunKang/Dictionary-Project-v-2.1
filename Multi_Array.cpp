#include "Multi_Array.h"

#ifdef _WIN32
void Array::gotoxy(short x, short y)
{
	COORD Pos = { x - 1, y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}
short Array::wherey()
{
	CONSOLE_SCREEN_BUFFER_INFO BufInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufInfo);
	return BufInfo.dwCursorPosition.Y;
}
short Array::wherex()
{
	CONSOLE_SCREEN_BUFFER_INFO BufInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufInfo);
	return BufInfo.dwCursorPosition.X;
}
#endif

wstring Array::To_wstr(const string & str)
{
	unsigned int required_len = str.length();

	wchar_t * dest = new wchar_t[required_len + 1];

	mbstowcs(dest, str.c_str(), required_len);

	dest[required_len] = L'\0';

	wstring converted_str(dest);

	delete[] dest;

	return converted_str;
}

string Array::To_str(const wstring & wstr)
{
	unsigned int required_len = 0;
	wchar_t buf[2];
	char dest_buf[2];
	buf[1] = L'\0';

	unsigned int len = wstr.length();

	for (int i = 0; i < len; i++) {
		buf[0] = wstr[i];
		wcstombs(dest_buf, buf, 2);
		if (dest_buf[0] & 0x80)
			required_len += 2;
		else
			required_len += 1;
	}

	char * dest = new char[required_len + 1];

	wcstombs(dest, wstr.c_str(), required_len);

	dest[required_len] = '\0';

	string converted_str(dest);

	delete[] dest;

	return converted_str;
}

wstring Array::Type2str(Type type)
{
	wstring type_name;

	switch (type) {
	case Type::CHAR:
		type_name = L"CHAR";
		break;
	case Type::UNSIGNED_CHAR:
		type_name = L"UNSIGNED CHAR";
		break;
	case Type::SHORT:
		type_name = L"SHORT";
		break;
	case Type::UNSIGNED_SHORT:
		type_name = L"UNSIGNED SHORT";
		break;
	case Type::INT:
		type_name = L"INT";
		break;
	case Type::UNSIGNED_INT:
		type_name = L"UNSIGNED INT";
		break;
	case Type::LONG:
		type_name = L"LONG";
		break;
	case Type::UNSIGNED_LONG:
		type_name = L"UNSIGNED LONG";
		break;
	case Type::LL:
		type_name = L"LONG LONG";
		break;
	case Type::UNSIGNED_LL:
		type_name = L"UNSIGNED LONG LONG";
		break;
	case Type::FLOAT:
		type_name = L"FLOAT";
		break;
	case Type::DOUBLE:
		type_name = L"DOUBLE";
		break;
	case Type::LONG_DOUBLE:
		type_name = L"LONG DOUBLE";
		break;
	case Type::WCHAR:
		type_name = L"WCHAR";
		break;
	case Type::STRING:
		type_name = L"STRING CLASS";
		break;
	case Type::PDATA:
		type_name = L"USER_DEFINED CLASS";
		break;
	default:
		type_name = L"WSTRING(UNKNOWN)";
	}
	return type_name;
}


vector<wstring> Array::copy_value() {
	vector<wstring> value;
	if (refer_dim == 0) {
		for (int i = 0; i < size; i++) {
			value.push_back(this->array[i]);
		}
	}
	else {
		for (int i = refer_pos_start; i <= refer_pos_end; i++) {
			value.push_back(this->array[i]);
		}
	}
	return value;
}
vector<int> Array::copy_size() {
	vector<int> result;
	for (int i = refer_dim; i < dimension; i++) {
		result.push_back(array_size[i]);
	}
	return result;
}
vector<Array::Type> Array::copy_decoder() {

	vector<Type> value;

	if (refer_dim == 0) {
		for (int i = 0; i < size; i++) {
			value.push_back(this->decoder[i]);
		}
	}
	else {
		for (int i = refer_pos_start; i <= refer_pos_end; i++) {
			value.push_back(this->decoder[i]);
		}
	}
	return value;
}

void Array::Alloc_Array(vector<unsigned int> info) {

	Dealloc_Array();

	size = 1;
	dimension = info.size();
	array_size = new int[dimension];
	for (int i = 0; i < dimension; i++) {
		array_size[i] = info[i];
		size *= info[i];
	}
	array = new wstring[size];
	decoder = new Type[size];
	memset(decoder, static_cast<unsigned int>(Type::DEFAULT_TYPE), sizeof(unsigned int)*this->size);
}
void Array::Dealloc_Array() {
	if (this->array != nullptr) {
		delete[] decoder;
		delete[] array;
		delete[] array_size;
		dimension = 0;
		size = 0;
		init_param();
		this->array = nullptr;
		this->decoder = nullptr;
		this->array_size = nullptr;
	}
}

const int Array::dim() {
	init_param();
	return this->dimension;
}
void Array::init_param() {
	refer_pos_start = 0;
	refer_pos_end = 0;
	refer_dim = 0;
}
const int Array::get_size(const int idx) {
	init_param();
	if (idx >= dimension || idx < 0)
		return -1;
	else
		return this->array_size[idx];
}

const int Array::get_size()
{
	init_param();
	return this->size;
}

wstring Array::get_name()
{
	init_param();
	return this->name;
}

void Array::info()
{
	vector<unsigned int> tmp_shape;
	vector<Type> type;

	wcout << endl << L"Info of " << this->name;
	unsigned int end_pt = this->refer_pos_end;
	unsigned int pos = 1;
	vector<unsigned int> idxs;
	pos = size / this->array_size[this->dimension - 1];

	for (int i = this->refer_dim - 1; i >= 0; i--) {
		for (unsigned int j = 0; j < this->array_size[i]; j++) {
			if (pos * (j + 1) > end_pt) {
				idxs.push_back(j);
				break;
			}
		}
		end_pt -= pos * idxs.back();
		if (i != 0)
			pos /= this->array_size[i - 1];
	}
	unsigned int idx_size = idxs.size();
	for (int i = 0; i < idx_size; i++) {
		wcout << L"[" << idxs[i] << L"]";
	}
	idxs.clear();
	wcout << L":" << endl << endl;

	/* Shape Calculate */
	if (!dimension) {
		wcout << L"\tNot Initialized" << endl << endl;
		return;
	}
	else {
		if (this->dimension - this->refer_dim <= 1)
			tmp_shape.push_back(1);
		if (this->dimension == this->refer_dim)
			tmp_shape.push_back(1);

		for (int i = refer_dim; i < dimension; i++) {
			tmp_shape.push_back(get_size(i));
		}
	}
	/* Shape print */
	wcout << L"\tShape = [ ";
	unsigned int tmp_shape_size = tmp_shape.size();
	for (int i = 0; i < tmp_shape_size; i++) {
		wcout << tmp_shape[i];
		if (i != tmp_shape_size - 1)
			wcout << L", ";
	}
	wcout << L" ]" << endl;
	tmp_shape.clear();

	/* Type print */
	unsigned int end = (this->refer_dim > 0) ? this->refer_pos_end : this->size - 1;
	wcout << L"\tType = {  ";
	for (int i = refer_pos_start; i <= end; i++) {
		type.push_back(this->decoder[i]);
	}
	/* remove the same */
	sort(type.begin(), type.end());
	type.erase(unique(type.begin(), type.end()), type.end());

	unsigned int type_size = type.size();

	for (int i = 0; i < type_size; i++) {
		wcout << Type2str(type[i]) << L"  ";
	}
	wcout << L"}";
	if (type_size > 1)
		wcout << L"[ Mix of " << type_size << L" types]";
	if (this->dimension == this->refer_dim)
		wcout << endl << L"\tValue(string) = " << this->array[refer_pos_start];
	init_param();
	wcout << endl << endl;
	return;
}

int Array::type()
{
	unsigned int type;

	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0016");
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return -1;
	}

	type = (unsigned int)this->decoder[refer_pos_start];

	init_param();
	return type;
}

vector<unsigned int> Array::shape()
{
	vector<unsigned int> shapes;

	if (!dimension)
		Print_Error(L"Multi_Array", this->name + L":MA0000");

	if (this->dimension - this->refer_dim <= 1)
		shapes.push_back(1);
	if (this->dimension == this->refer_dim)
		shapes.push_back(1);

	for (int i = refer_dim; i < dimension; i++) {
		shapes.push_back(get_size(i));
	}

	init_param();
	return shapes;
}

void Array::set_precision(unsigned int num) {
	this->precision = num;
	init_param();
}

wstring Array::get_raw()
{
	wstring raw_value;
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0017");
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
	}
	raw_value = this->array[refer_pos_start];
	init_param();
	return raw_value;
}

wstring Array::Array2str()
{
	wstring str;
	wstring ch = L"\t";
	wstring tmp;

	if (!dimension)
		Print_Error(L"Multi_Array", this->name + L":MA0000");

	unsigned int end = (this->refer_dim > 0) ? this->refer_pos_end : this->size - 1;

	for (int i = this->refer_pos_start; i <= end; i++) {

		tmp = this->array[i];

		unsigned int tmp_len = tmp.length();

		for (int i = 0; i < tmp_len; i++) {
			if (tmp[i] == L'\t')
				tmp[i] = L' ';
		}
		str = str + tmp;
		str.append(ch);
	}

	init_param();

	return str;
}

void Array::change_name(wstring new_name)
{
	this->name = new_name;
	init_param();
	return;
}

void Array::copy(vector<wstring> data, int dim, vector<int> array_size, vector<Type> decoder)
{
	vector<unsigned int> array_info;
	unsigned int data_size = data.size();
	if (data_size == 0)
		return; // Error
	if (dim == 0) {
		dim = 1;
		array_size.push_back(1);
	}
	for (unsigned int i = 0; i < dim; i++) {
		array_info.push_back(array_size[i]);
	}
	/* Allocator */
	if (this->refer_dim == 0)
		Alloc_Array(array_info);

	for (unsigned int i = 0; i < data_size; i++) {
		array[i + refer_pos_start] = data[i];
		this->decoder[i + refer_pos_start] = decoder[i];
	}

	array_info.clear();
}

/* constructor */
Array::Array(wstring name) : array(nullptr), decoder(nullptr), dimension(0),
array_size(nullptr), refer_pos_start(0), refer_pos_end(0), refer_dim(0),
size(0), gen_1(time(NULL)), name(name), precision(6) {
	srand(time(NULL));
}

Array::Array(const Array & in) : array(nullptr), decoder(nullptr), dimension(0),
array_size(nullptr), refer_pos_start(0), refer_pos_end(0), refer_dim(0),
size(0), gen_1(time(NULL)), precision(6)
{
	srand(time(NULL));
	try {
		if (!in.dimension)
			throw runtime_error(":MA0000");
		if (in.refer_dim != 0)
			throw runtime_error(":MA0031");
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
	}

	this->name = in.name;
	vector<unsigned int> new_info;
	for (int i = 0; i < in.dimension; i++) {
		new_info.push_back(in.array_size[i]);
	}
	Alloc_Array(new_info);

	for (int i = 0; i < in.size; i++) {
		this->array[i] = in.array[i];
		this->decoder[i] = in.decoder[i];
	}

	set_precision(in.precision);
	init_param();
}

Array::~Array() {
	Dealloc_Array();
}

/* Initializer */
void Array::zeros(unsigned int args, ...) {
	vector<unsigned int> array_info;
	va_list ap;
	va_start(ap, args);
	unsigned int dim = args;

	if (dim == 0)
		Print_Error(L"Multi_Array", this->name + L":MA0000");

	for (unsigned int i = 0; i < dim; i++) {
		array_info.push_back(va_arg(ap, unsigned int));
	}

	/* Allocator */
	Alloc_Array(array_info);

	/* Initialize */
	for (unsigned int i = 0; i < this->size; i++) {
		array[i] = to_wstring(0);
		decoder[i] = Type::INT;
	}
	init_param();
	array_info.clear();
	va_end(ap);
}

void Array::ones(unsigned int args, ...) {
	vector<unsigned int> array_info;
	va_list ap;
	va_start(ap, args);
	unsigned int dim = args;

	if (dim == 0)
		Print_Error(L"Multi_Array", this->name + L":MA0000");

	for (unsigned int i = 0; i < dim; i++) {
		array_info.push_back(va_arg(ap, unsigned int));
	}

	/* Allocator */
	Alloc_Array(array_info);

	/* Initialize */
	for (unsigned int i = 0; i < this->size; i++) {
		array[i] = to_wstring(1);
		decoder[i] = Type::INT;
	}
	init_param();
	array_info.clear();
	va_end(ap);
}

void Array::randn(unsigned int args, ...) {
	vector<unsigned int> array_info;
	va_list ap;
	va_start(ap, args);
	unsigned int dim = args;

	if (dim == 0)
		Print_Error(L"Multi_Array", this->name + L":MA0000");

	for (unsigned int i = 0; i < dim; i++) {
		array_info.push_back(va_arg(ap, unsigned int));
	}

	/* Allocator */
	Alloc_Array(array_info);

	/* Initialize */
	for (unsigned int i = 0; i < this->size; i++) {
		array[i] = to_string_with_precision(gen_randn(0.0, 1.0));
		decoder[i] = Type::DOUBLE;
	}
	init_param();
	array_info.clear();
	va_end(ap);
}

void Array::init(vector<unsigned int> size)
{
	unsigned int dim = size.size();

	if (dim == 0)
		Print_Error(L"Multi_Array", this->name + L":MA0000");

	/* Allocator */
	Alloc_Array(size);

	/* Initialize */
	for (unsigned int i = 0; i < this->size; i++) {
		array[i] = to_string_with_precision(gen_randn(0.0, 1.0));
		decoder[i] = Type::DOUBLE;
	}
	init_param();
	size.clear();
}

/* Get Value */
Array& Array::operator[](int idx)
{
	try {
		if (array == nullptr)
			throw runtime_error(":MA0001"); // Not valid approach
		if (refer_dim == dimension)
			throw runtime_error(":MA0002");
		if (idx >= array_size[refer_dim] || idx < 0)
			throw runtime_error(":MA0003"); // idx out of range
		int digit = 1;
		for (int i = 0; i < dimension - (refer_dim + 1); i++) {
			digit *= array_size[dimension - i - 1];
		}
		refer_pos_end = refer_pos_start;
		refer_pos_start += digit * idx;
		refer_pos_end += digit * (idx + 1) - 1;
		refer_dim++;
	}
	catch (exception & e) {
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return (*this);
	}

	return (*this);
}

Array& Array::operator=(Array & in) {
	int new_dim = in.dimension - in.refer_dim;
	int new_end_pos = 0;
	try {
		if (!in.dimension)
			throw runtime_error(":MA0000");
		if (refer_dim != 0) {
			if (in.refer_dim == 0)
				new_end_pos = in.size - 1;
			else
				new_end_pos = in.refer_pos_end;
			if (refer_pos_end - refer_pos_start != new_end_pos - in.refer_pos_start)
				throw runtime_error(":MA0004");
		}
		copy(in.copy_value(), new_dim, in.copy_size(), in.copy_decoder());
		set_precision(in.precision);
	}
	catch (exception& e) {
		in.init_param();
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return (*this); // Error
	}
	in.init_param();
	init_param();

	return (*this);
}

Array& Array::operator<<(Array & in) {
	int new_dim = in.dimension - in.refer_dim;
	int new_end_pos = 0;
	try {
		if (!in.dimension)
			throw runtime_error(":MA0000");
		if (refer_dim != 0) {
			if (in.refer_dim == 0)
				new_end_pos = in.size - 1;
			else
				new_end_pos = in.refer_pos_end;
			if (refer_pos_end - refer_pos_start != new_end_pos - in.refer_pos_start)
				throw runtime_error(":MA0005");
		}
		copy(in.copy_value(), new_dim, in.copy_size(), in.copy_decoder());
		set_precision(in.precision);
	}
	catch (exception& e) {
		in.init_param();
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return (*this); // Error
	}
	in.init_param();
	init_param();

	return (*this);
}

Array& Array::operator>> (Array & in) {
	int new_dim = this->dimension - this->refer_dim;
	int new_end_pos = 0;
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (in.refer_dim != 0) {
			if (refer_dim == 0)
				new_end_pos = size - 1;
			else
				new_end_pos = refer_pos_end;

			if (new_end_pos - refer_pos_start != in.refer_pos_end - in.refer_pos_start)
				throw runtime_error(":MA0006");
		}
		in.copy(copy_value(), new_dim, copy_size(), copy_decoder());
		in.set_precision(this->precision);
	}
	catch (exception& e) {
		in.init_param();
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return (*this); // Error
	}
	in.init_param();
	init_param();

	return (*this);
}

bool Array::operator==(string & in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0007"); //
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (this->decoder[refer_pos_start] == declare_type(in) && this->array[refer_pos_start] == To_wstr(in)) {
		init_param();
		return true;
	}
	else {
		init_param();
		return false;
	}
}

bool Array::operator==(wstring & in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0024"); //
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (this->decoder[refer_pos_start] == declare_type(in) && this->array[refer_pos_start] == in) {
		init_param();
		return true;
	}
	else {
		init_param();
		return false;
	}
}

bool Array::operator==(const char * in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0025"); //
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (this->decoder[refer_pos_start] == declare_type(in) && this->array[refer_pos_start] == To_wstr(in)) {
		init_param();
		return true;
	}
	else {
		init_param();
		return false;
	}
}

bool Array::operator==(const wchar_t * in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0026"); //
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (this->decoder[refer_pos_start] == declare_type(in) && this->array[refer_pos_start] == in) {
		init_param();
		return true;
	}
	else {
		init_param();
		return false;
	}
}

bool Array::operator==(Array & in)
{
	int new_dim = this->dimension - this->refer_dim;
	int new_end_pos = 0;
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (in.refer_dim != 0) {
			if (refer_dim == 0)
				new_end_pos = size - 1;
			else
				new_end_pos = refer_pos_end;

			if (new_end_pos - refer_pos_start != in.refer_pos_end - in.refer_pos_start)
				return false;
		}
	}
	catch (exception& e) {
		in.init_param();
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	unsigned int end = (in.refer_dim > 0) ? in.refer_pos_end : in.size - 1;

	for (unsigned int i = in.refer_pos_start; i <= end; i++) {
		if ((this->array[i - in.refer_pos_start + refer_pos_start] != in.array[i]) || (this->decoder[i - in.refer_pos_start + refer_pos_start] != in.decoder[i])) {
			in.init_param();
			init_param();
			return false;
		}
	}

	in.init_param();
	init_param();

	return true;
}

bool Array::operator!=(string & in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0027"); //
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (this->decoder[refer_pos_start] == declare_type(in) && this->array[refer_pos_start] == To_wstr(in)) {
		init_param();
		return false;
	}
	else {
		init_param();
		return true;
	}
}

bool Array::operator!=(wstring & in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0028"); //
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (this->decoder[refer_pos_start] == declare_type(in) && this->array[refer_pos_start] == in) {
		init_param();
		return false;
	}
	else {
		init_param();
		return true;
	}
}

bool Array::operator!=(const char * in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0029"); //
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (this->decoder[refer_pos_start] == declare_type(in) && this->array[refer_pos_start] == To_wstr(in)) {
		init_param();
		return false;
	}
	else {
		init_param();
		return true;
	}
}

bool Array::operator!=(const wchar_t * in)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0030"); //
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	if (this->decoder[refer_pos_start] == declare_type(in) && this->array[refer_pos_start] == in) {
		init_param();
		return false;
	}
	else {
		init_param();
		return true;
	}
}

bool Array::operator!=(Array & in)
{
	int new_dim = this->dimension - this->refer_dim;
	int new_end_pos = 0;
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (in.refer_dim != 0) {
			if (refer_dim == 0)
				new_end_pos = size - 1;
			else
				new_end_pos = refer_pos_end;

			if (new_end_pos - refer_pos_start != in.refer_pos_end - in.refer_pos_start)
				return true;
		}
	}
	catch (exception& e) {
		in.init_param();
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
		return false; // Error
	}

	unsigned int end = (in.refer_dim > 0) ? in.refer_pos_end : in.size - 1;

	for (unsigned int i = in.refer_pos_start; i <= end; i++) {
		if ((this->array[i - in.refer_pos_start] == in.array[i]) && (this->decoder[i - in.refer_pos_start] == in.decoder[i])) {
			in.init_param();
			init_param();
			return false;
		}
	}

	in.init_param();
	init_param();

	return true;

}

Array& Array::operator=(string & in) {
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0007");
		this->decoder[refer_pos_start] = declare_type(in);
		this->array[refer_pos_start] = To_wstr(in);
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
Array& Array::operator=(wstring & in) {
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0008");
		this->decoder[refer_pos_start] = declare_type(in);
		this->array[refer_pos_start] = in;
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
Array & Array::operator=(const char * in)
{
	string new_element = in;
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0007");
		this->decoder[refer_pos_start] = declare_type(new_element);
		this->array[refer_pos_start] = To_wstr(new_element);
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
Array & Array::operator=(const wchar_t * in)
{
	wstring new_element = in;
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0008");
		this->decoder[refer_pos_start] = declare_type(new_element);
		this->array[refer_pos_start] = new_element;
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
Array& Array::operator<<(string & in) {
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0009");
		this->decoder[refer_pos_start] = declare_type(in);
		this->array[refer_pos_start] = To_wstr(in);
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
Array& Array::operator<<(wstring & in) {
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0010");
		this->decoder[refer_pos_start] = declare_type(in);
		this->array[refer_pos_start] = in;
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

Array & Array::operator<<(const char * in)
{
	string new_element = in;
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0009");
		this->decoder[refer_pos_start] = declare_type(new_element);
		this->array[refer_pos_start] = To_wstr(new_element);
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

Array & Array::operator<<(const wchar_t * in)
{
	wstring new_element = in;
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (this->refer_dim != this->dimension)
			throw runtime_error(":MA0010");
		this->decoder[refer_pos_start] = declare_type(new_element);
		this->array[refer_pos_start] = new_element;
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

void Array::put_value(string & in, unsigned int pos)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (pos > this->size)
			throw runtime_error(":MA0022");
		this->decoder[pos] = declare_type(in);
		this->array[pos] = To_wstr(in);
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
	}
	init_param();
}

void Array::put_value(wstring & in, unsigned int pos)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (pos > this->size)
			throw runtime_error(":MA0021");
		this->decoder[pos] = declare_type(in);
		this->array[pos] = in;
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
	}
	init_param();
}

void Array::put_value(const char * in, unsigned int pos)
{
	string new_element = in;
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (pos > this->size)
			throw runtime_error(":MA0020");
		this->decoder[pos] = declare_type(new_element);
		this->array[pos] = To_wstr(new_element);
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
	}
	init_param();
}

void Array::put_value(const wchar_t * in, unsigned int pos)
{
	wstring new_element = in;
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (pos > this->size)
			throw runtime_error(":MA0019");
		this->decoder[pos] = declare_type(new_element);
		this->array[pos] = new_element;
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
	}
	init_param();
}

void Array::put_value(wstring & in, unsigned int pos, unsigned int type)
{
	try {
		if (!dimension)
			throw runtime_error(":MA0000");
		if (pos > this->size)
			throw runtime_error(":MA0021");
		if (type > (unsigned int)Type::PDATA)
			throw runtime_error(":MA0024");
		this->decoder[pos] = (Type)type;
		this->array[pos] = in;
	}
	catch (exception& e) {
		init_param();
		wstring error = name + To_wstr(e.what());
		Print_Error(L"Multi_Array", error);
	}
	init_param();
}

void Array::print(int enter_posx) {

	/* Print Object's Name */
	wcout << this->name;
	unsigned int end_pt = this->refer_pos_end;
	unsigned int pos_pt = 1;
	vector<unsigned int> idxs;
	pos_pt = size / this->array_size[this->dimension - 1];

	for (int i = this->refer_dim - 1; i >= 0; i--) {
		for (unsigned int j = 0; j < this->array_size[i]; j++) {
			if (pos_pt * (j + 1) > end_pt) {
				idxs.push_back(j);
				break;
			}
		}
		end_pt -= pos_pt * idxs.back();
		if (i != 0)
			pos_pt /= this->array_size[i - 1];
	}
	unsigned int idx_size = idxs.size();
	for (int i = 0; i < idx_size; i++) {
		wcout << L"[" << idxs[i] << L"]";
	}
	idxs.clear();
	wcout << L":";

	/* Print the value */
	unsigned int row_count = 0;
	short pos = 0;

	unsigned int start = this->refer_pos_start;
	unsigned int end = this->refer_pos_end;
	unsigned int stop_pt = 1;

	if (this->array != nullptr) {
		if (!this->refer_dim) {
			end = this->size - 1;
			stop_pt = this->size / this->array_size[0];
		}
		else {
			for (int i = 0; i < dimension - (refer_dim + 1); i++) {
				stop_pt *= array_size[dimension - i - 1];
			}
		}
		if (stop_pt == 1) // vector
			stop_pt = end - start + 1;

		for (unsigned int i = start; i <= end; i++) {
			if (i % stop_pt == 0) {
				cout << endl << "     " << ++row_count << "th row :";
			}
			pos = wherex();
			wcout << L" " << array[i];
			if (pos + 11 > enter_posx)
				gotoxy(15, wherey() + 2);
			else
				gotoxy(pos + 11, wherey() + 1);
		}
		cout << endl;
	}
	init_param();
	return;
}

/* Convert value from wstring to its original type */
void Array::Convert_Value(int & in, int idx) {
	in = stoi(array[idx]);
}
void Array::Convert_Value(char & in, int idx) {
	in = (char)stoi(array[idx]);
}
void Array::Convert_Value(wchar_t & in, int idx) {
	in = (wchar_t)stoi(array[idx]);
}
void Array::Convert_Value(short & in, int idx) {
	in = (short)stoi(array[idx]);
}
void Array::Convert_Value(unsigned short & in, int idx) {
	in = (unsigned short)stoi(array[idx]);
}
void Array::Convert_Value(unsigned char & in, int idx) {
	in = (unsigned char)stoi(array[idx]);
}
void Array::Convert_Value(unsigned int & in, int idx) {
	in = (unsigned int)stoi(array[idx]);
}
void Array::Convert_Value(long & in, int idx) {
	in = stol(array[idx]);
}
void Array::Convert_Value(unsigned long & in, int idx) {
	in = stoul(array[idx]);
}
void Array::Convert_Value(long long & in, int idx) {
	in = stoll(array[idx]);
}
void Array::Convert_Value(unsigned long long & in, int idx) {
	in = stoull(array[idx]);
}
void Array::Convert_Value(float & in, int idx) {
	in = stof(array[idx]);
}
void Array::Convert_Value(double & in, int idx) {
	in = stod(array[idx]);
}
void Array::Convert_Value(long double & in, int idx) {
	in = stold(array[idx]);
}
void Array::Convert_Value(string & in, int idx) {
	in = To_str(array[idx]);
}
void Array::Convert_Value(wstring & in, int idx) {
	in = array[idx];
}