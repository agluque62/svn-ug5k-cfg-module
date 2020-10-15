#include "../../include/tools/tools.h"
#include "../../include/base/socket.h"

/** */
void url::parse(const string& url_s)
{
	size_t pos1 = url_s.find("//") + 2;
	if (pos1 < url_s.length()) 
	{
		protocol_ = url_s.substr(0, pos1);
		size_t pos2 = url_s.find("/", pos1);
		if (pos2 < url_s.length())
		{
			host_ = url_s.substr(pos1, (pos2-pos1));
			path_ = url_s.substr(pos2, url_s.length()-pos2);
		}
	}
}

/**
*/
bool Tools::EsIgual(const char * str1, const char * str2)
{
	return strcmp(str1,str2)==0 ? true : false;
}

/**
*/
bool Tools::FileExist(const char *filename)
{
	// ifstream file(string(filename), ifstream::in);
	ifstream file(filename, ios_base::in);
	bool ret = file.good();
	file.close();
	return ret;
}

/**
*/
static char _fecha[40]={0};
char * Tools::Hoy(void)
{
	
	time_t now;	
	struct tm * timeinfo;	

	time(&now);
	timeinfo = localtime (&now);
	strftime (_fecha, sizeof(_fecha), " %d/%m/%Y", timeinfo);
	return _fecha;
}

/**
*/
string Tools::HttpDecodeSpecialChars(string &stry)
{
	stry = Tools::replace(stry, "%2F", "/");
	stry = Tools::replace(stry, "%3A", ":");
	stry = Tools::replace(stry, "+", " ");
	stry = Tools::replace(stry, "%20", " ");
	stry = Tools::replace(stry, "%40", "@");

	return stry;
}

/**
*/
bool Tools::HttpQuerySplit(const char *qry, string &prop, string &value)
{
	/** Analizo los caracteres especiales. */
	string stry = qry;
	stry = Tools::HttpDecodeSpecialChars(stry);

	/** Obtengo los campos.. */
	size_t pos = stry.find("=");
	if (pos != string::npos)
	{
		prop = stry.substr(0,pos);
		value = stry.substr(pos+1,stry.length()-1);
		return true;
	}

	return false;
}

/**
*/
string & Tools::replace(string & subj, string old, string neu)
{
	size_t uiui = subj.find(old);
	while (uiui != string::npos)    
	{    
		subj.erase(uiui, old.size());        
		subj.insert(uiui, neu);        
		uiui = subj.find(old,uiui);
	}    
	return subj;    
}

/**
 * Wed, 09 Jun 2021 10:18:14 GMT
 * Wed Feb 13 15:46:11 2013
 * */
string Tools::DateCookie(int seconds)
{
	time_t now;
	struct tm * timeinfo;

	time(&now);
	now += seconds;
	timeinfo = gmtime (&now);
	strftime (_fecha, sizeof(_fecha), "%a,  %d %b %Y %H:%M:%S GMT", timeinfo);
	return _fecha;

}

/**
*/
string Tools::itoa(int i)
{
	std::stringstream out;
	out << i;
	return out.str();
}

/**
*/
int Tools::atoi(string str)
{
	int ret;
	int ok = sscanf(str.c_str(), "%i", &ret);
	return ok==1 ? ret : -1;
}

/**
*/
float Tools::atof(string num_str)
{
	//float ret;
	//int ok = sscanf(str.c_str(), "%f", &ret);
	//return ok==1 ? ret : -1;
	// return (float)(::atof(str.c_str()));
	std::istringstream iss(num_str);
	float f_val = 0;
	iss >> f_val;
	return f_val;
}

/** */
bool Tools::ValidateIpAddress(const string &ipAddress)
{
	int p1,p2,p3,p4;
	int conv = sscanf(ipAddress.c_str(),"%u.%u.%u.%u",&p1,&p2,&p3,&p4);
	if (conv != 4) return false;
	if (p1<0 || p1>255) return false;
	if (p2<0 || p2>255) return false;
	if (p3<0 || p3>255) return false;
	if (p4<0 || p4>255) return false;
	return true;
}


/**
*/
void Tools::DelFile(string path, string file)
{
	string name = path + file;
	if( remove( (name).c_str() ) != 0 )  
	{  
        std::cout << "No Puedo Eliminar el fichero: " << (name).c_str() << endl;
	}
}

////////////////////////////////////////////
/** */
template< typename T > std::string Tools::hexify(T i, bool prefijo)
{
    std::stringbuf buf;
    std::ostream os(&buf);

    os << (prefijo ? "0x" : "") << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << i;

    return buf.str().c_str();
}

/** */
void Tools::split(std::vector<std::string> &tokens, const std::string &text, char sep) {
    /*unsigned int*/size_t start = 0, end = 0;

    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }

    tokens.push_back(text.substr(start));
}

/** */
void Tools::stringofint_to_vectorint(string inputstring, char sep, vector<int> &output)
{
	/* El string debe venir sin caracteres adicionales entre los enteros */
	vector<string> str_ints;
	Tools::split(str_ints, inputstring, sep);
	for (vector<string>::iterator i=str_ints.begin();
		i!=str_ints.end(); i++) {
			output.push_back(Tools::atoi(*i));
	}
}


/** */
bool Tools::StrContains(std::string texto, std::string valor) {
    if (texto.find(valor) != std::string::npos)
        return true;
    return false;
}

/** */
std::string Tools::StrFormat(const char *fmt, ...) {
    char textString[1024] = {'\0'}; // TODO: Revisar este limite...

    // -- empty the buffer properly to ensure no leaks.
    memset(textString, '\0', sizeof (textString));

    va_list args;
    va_start(args, fmt);
#ifdef _WIN32
    vsnprintf_s(textString, 1024, fmt, args); // TODO: Tambien esta aqui el limite...
#else
    vsnprintf(textString, 1024, fmt, args); // TODO: Tambien esta aqui el limite...
#endif
    va_end(args);
    std::string retStr = textString;
    return retStr;
}

/** */
std::string Tools::Ahora() {
    struct tm * timeinfo;
    char buffer [80];
	char buffer2[16];
	timeb extTime;

	ftime(&extTime);
	
    //std::time_t result = std::time(NULL);
    //timeinfo = localtime(&result);
	//strftime(buffer, 80, "%Y-%m-%dT%H:%M:%S.000Z", timeinfo);
	timeinfo = localtime(&extTime.time);
	strftime(buffer, 80, "%Y-%m-%dT%H:%M:%S", timeinfo);
	sprintf(buffer2, ".%03dZ", extTime.millitm);

    return std::string(buffer) + std::string(buffer2);
}

/** */
std::string Tools::Ahora_Servidor() {
    struct tm * timeinfo;
    char buffer [80];

    std::time_t result = std::time(NULL);
//    timeinfo = localtime(&result);
    timeinfo = gmtime(&result);
    strftime(buffer, 80, "%d/%m/%Y %H:%M:%S UTC", timeinfo);

    return std::string(buffer);
}

/** */
std::string Tools::Buff2String(int len, const char *data) {
    std::string str(data, len);
    return str;
}

/** */
std::string Tools::Int2String(int val)
{
	char buf[8];
	sprintf(buf,"%d",val);
	return std::string(buf);
}

/** */
bool Tools::DateString2time(std::string str, struct tm &stime)
{
// #ifdef _WIN32
	sscanf(str.c_str(),"%d/%d/%d %d:%d:%d", &stime.tm_mday, &stime.tm_mon, &stime.tm_year, &stime.tm_hour, &stime.tm_min, &stime.tm_sec);
	stime.tm_year -= 1900;
	stime.tm_mon -= 1;
	stime.tm_isdst = -1;
// #else
// 	strptime(str.c_str(), "%d/%m/%Y %H:%M:%s", &stime);
// #endif

	return true;
}

/** Numero de Segundos desde 1-1-2010 */
std::string Tools::ProgramUniqueId()
{
	struct tm stref;
	Tools::DateString2time("1/1/2010 00:00:00", stref);

	time_t tref = mktime(&stref);
	time_t tnow = time(NULL);
	
	long dif = (long )difftime(tnow,tref);
//	std::string id = Tools::hexify< long >(dif, false);
	std::string id = Tools::hexify(dif, false);

	return id;
}

/** */
std::string Tools::FileUniqueName(std::string fname)
{
	std::string ufname;
	std::string uid = Tools::ProgramUniqueId();

	size_t i = fname.rfind('.', fname.length());
	if (i != std::string::npos) 
	{
		ufname = fname.substr(0,i) + "_" + uid + fname.substr(i,fname.length()-i);
	}
	else
	{
		ufname = fname + "_" + uid;
	}

	return ufname;
}

/** */
std::string Tools::CookieTime(char *buffer, struct tm stime)
{
	const std::string dayName[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	const std::string monName[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	sprintf(buffer,"%s, %d %s %d %02d:%02d:%02d GMT", dayName[stime.tm_wday].c_str(), stime.tm_mday, monName[stime.tm_mon].c_str(),
		1900 + stime.tm_year, stime.tm_hour, stime.tm_min, stime.tm_sec);

	return std::string(buffer);
}

/** */
bool Tools::IsNoAscii(char c)
{
	return ((int)c >= 32 && (int)c<128) ? false : true;
}

/** */
string Tools::read_txt_file(string name)
{
	ifstream infile(name.c_str());
	vector<char> buffer((istreambuf_iterator<char>(infile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	return string(&buffer[0]);
}

/** */
bool Tools::ip_format_test(string ip)
{
	vector<string> cuartetos;
	split(cuartetos, ip, '.');

	if (cuartetos.size() != 4)
		return false;

	for (int cuarteto = 0; cuarteto < 4; cuarteto++)
	{
		int valor = atoi(cuartetos[cuarteto]);
		if (valor < 0 || valor > 255)
			return false;
	}
	return true;
}

/** */
void Tools::Dir(vector<string> &lista, string path, string msc)
{
	DIR *dir;
	struct dirent *ent;
	lista.clear();
	if ((dir = opendir (path.c_str())) != NULL)  {
	  /* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			string name = string(ent->d_name);
			if (name != "." && name != "..")
				lista.push_back(name);
		}
		closedir (dir);
	} 
	else {
	  /* could not open directory */
	}
}

/** */
void Tools::tm2String(struct tm* timeptr, string format, string &date)
{
	char buffer [80];
	strftime (buffer,80,format.c_str(),timeptr);
	date = string(buffer);
}

/** */
void Tools::append2file(string name, string msg) 
{
	try {
		ofstream outfile(name.c_str(), ios_base::app);
		outfile << msg << endl;
	}
	catch(...) {
	}
}

/** */
static int current_file_error = 4, max_files_error=4;
static int current_file_entry = 250, max_file_entries=250;
void Tools::fatalerror(string msg)
{
	if (++current_file_entry >= max_file_entries) {
		current_file_entry = 0;
		if (++current_file_error >= max_files_error) {
			current_file_error = 0;
		}
		/** Inicializar el fichero para la proxima escritura */
		string name_prox = "fatalerror_" + Tools::Int2String(current_file_error) + ".log";
		remove( onflash(name_prox).c_str() );
	}

	string name = "fatalerror_" + Tools::Int2String(current_file_error) + ".log";
	Tools::append2file(onflash(name), Tools::Int2String(current_file_entry) + ", " + Tools::Ahora() + ": " + msg);
}

/** */
void Tools::Trace(const char* fmt, ...) {
	static int current_file_trace = 4, max_files_trace = 4;
	static int current_file_trace_entry = 1024, max_file_trace_entries = 1024;
	static util::Mutex mtx;

	va_list args;
	va_start(args, fmt);
	char textString[256] = { '\0' };
#ifdef _WIN32
	vsnprintf_s(textString, sizeof(textString), fmt, args);
#else
	vsnprintf(textString, sizeof(textString), fmt, args);
#endif
	va_end(args);

	util::MutexLock lock(mtx);
	if (++current_file_trace_entry >= max_file_trace_entries) {
		current_file_trace_entry = 0;
		if (++current_file_trace >= max_files_trace) {
			current_file_trace = 0;
		}
		/** Inicializar el fichero para la proxima escritura */
		string name_prox = "trace_" + Tools::Int2String(current_file_trace) + ".log";
		remove(onram(name_prox).c_str());
	}

	string name = "trace_" + Tools::Int2String(current_file_trace) + ".log";
	Tools::append2file(onram(name), /*Tools::Int2String(current_file_trace_entry) + ", " + */Tools::Ahora() + ": " + textString);
}

bool Tools::SafeItoA(int val, char* strval) {
	if (val < 0 || val > 65535)
		return false;

	strval[0] = ' ';
	strval[1] = val / (10000) + '0';
	strval[2] = ((val % 10000) / 1000) + '0';
	strval[3] = (((val % 10000) % 1000) / 100) + '0';
	strval[4] = ((((val % 10000) % 1000) % 100) / 10) + '0';
	strval[5] = (val % 10) + '0';
	strval[6] = 0;

	return true;
}

void Tools::SafeLogFromSignal(const char* msg, int p1, int p2, int p3) {
	static int current_file_sig = 4, max_files_sig = 4;
	static int current_file_sig_entry = 1024, max_file_sig_entries = 1024;

	char filename[40];
	char fileorder[4];
	bool remove = false;
	char tmpData[8];
	char formatted[128];

	filename[0] = fileorder[0] = tmpData[0] = formatted[0] = 0;

	if (++current_file_sig_entry >= max_file_sig_entries) {
		current_file_sig_entry = 0;
		if (++current_file_sig >= max_files_sig) {
			current_file_sig = 0;
		}
		remove = true;
	}
	fileorder[0] = (char)('0' + current_file_sig);
	fileorder[1] = 0;
#if defined(_WIN32)
	strcat(filename, "./fs-win/home/serv/signals_");
#else
	strcat(filename, "/home/serv/signals_");
#endif
	strcat(filename, fileorder);
	strcat(filename, ".log");
	if (remove == true) {
		/** Inicializar el fichero para la proxima escritura */
		unlink(filename);
	}
	if (Tools::SafeItoA(current_file_sig_entry, tmpData)) {
		strcat(formatted, tmpData);
		strcat(formatted, ": ");
	}
	strcat(formatted, msg);
	if (Tools::SafeItoA(p1, tmpData)) {
		strcat(formatted, tmpData);
	}
	if (Tools::SafeItoA(p2, tmpData)) {
		strcat(formatted, tmpData);
	}
	if (Tools::SafeItoA(p3, tmpData)) {
		strcat(formatted, tmpData);
	}
	strcat(formatted, "\n");
	int filehandle = open(filename, O_CREAT | O_APPEND | O_RDWR);
	if (filehandle >= 0) {
		write(filehandle, formatted, strlen(formatted));
		close(filehandle);
	}
}
