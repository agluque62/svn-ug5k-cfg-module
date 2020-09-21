#include "../../include/tools/parse.h"

/** 
*/
Parse::Parse()
{
}

/**
*/
Parse::Parse (string query)
{ 
  this->split (this->_query,query,'\n');
}

/**
*/
void Parse::parse ()
{
  try 
  {
    char symbol = whatIsIt();
    if (symbol == POST)
    {
      this->parsePost();
    }
    else if (symbol == GET)
    {
      this->parseGet(); 
    }
  }
  catch (const char* a)
  {
    cerr << a << endl;
  }
}

/**
*/
void Parse::parse(string request)
{
	clear();
	split(_query,request,'\n');
	parse();
}

/**
*/
string Parse::ValueByKey (string key) const
{
  map<string,string> all;
  map<string,string>::const_iterator it;

  all.insert (this->_attributes.begin(),this->_attributes.end());
  all.insert (this->_head.begin(),this->_head.end());
  all.insert (this->_variables.begin(),this->_variables.end());

  it = all.find(key);
  if (it != all.end())
  {
    return it->second;
  }
  
  else return FAIL_MESSAGE;
}

/**
*/
void Parse::split (vector<string>& tmp,string query,char delimiter)
{
  std::stringstream ss(query);
  std::string item;

  tmp.clear();

  while (std::getline(ss, item, delimiter)) {
        if (query[0] != '\n') {
          tmp.push_back(item);
        }
  }
}

/**
*/
char Parse::whatIsIt () const
{
  if (this->_query[0][0] == GET)
  {
    return GET; 
  }
  else if (this->_query[0][0] == POST)
  {
    return POST;
  }
  else
  {
    throw "ABORT : NO POST NO GET.";
  }
}

/**
*/
void Parse::parsePost ()
{
  for (unsigned int i = 0; i < this->_query.size(); ++i)
  {
    if (i == 0)
    {
      //parse head post's query
    }
    else if (i > 0 && i < this->_query.size()-1)
    {
      //parse attributes post's query
      /*this->_query[i].erase(std::remove(this->_query[i].begin(), this->_query[i].end(), ' '), this->_query[i].end());*/
    }
    else
    {
      vector<string> step;
      this->split (step,this->_query[i],'&');
      for (unsigned int i = 0; i < step.size(); ++i)
      {
        vector<string> data;
        this->split (data,step[i],'=');
	if (data.size() > 2)
	{
	  data.erase(data.begin()+2,data.end());
	}
	if (data.size() == 2)
	{
          this->setMap (this->_variables,data);
	}
      }
    }
  }
}

/**
*/
void Parse::parseGet ()
{
  for (unsigned int i = 0; i < this->_query.size(); ++i)
  {
    if (i == 0)
    {
      vector<string> step1;
      vector<string> step2;
      vector<string> step3;

	  this->split(step1,this->_query[i],' ');
	  if (step1.size()>1)
		  this->split (step2,step1[1],'?');
	  if (step2.size() > 1)
		  this->split (step3,step2[1],'&');

	  for (unsigned int i = 0; i < step3.size(); i++)
	  {
        vector<string> data;
        this->split (data,step3[i],'=');
	
		if (data.size() > 2)
		{
			data.erase(data.begin()+2,data.end());
		}	

		if (data.size() == 1)
			data.push_back("");
		if (data.size() == 2)	
		{        
			this->setMap (this->_variables,data);        
		}
      }
    }
    else
    {
      //parse attribute get's query
      /*this->_query[i].erase(std::remove(this->_query[i].begin(), this->_query[i].end(), ' '), this->_query[i].end());*/
    }
  }      
}

/**
*/
void Parse::setMap (map<string,string>& sMap,vector<string> a)
{
  for (unsigned int i = 0; i < a.size(); i+=2)
  {
    sMap[a[i]] = a[i+1];
  }
}

/**
*/
void Parse::clear()
{
	_query.clear();
	_head.clear();
	_variables.clear();
	_attributes.clear();

}

/** */
ParseResponse::ParseResponse()
{
	_error = false;
}

/** */
ParseResponse::ParseResponse(string response)
{
	_error = !Parse(response);
}

/** */
ParseResponse::ParseResponse(CTCPSocket &sck,int timeout)
{
	_error = !Parse(sck, timeout);
}


/** */
string ParseResponse::Header(string _header) const
{
  map<string,string>::const_iterator it;

  it = _headers.find(_header);
  if (it != _headers.end())
  {
    return it->second;
  }
  
  else return FAIL_MESSAGE;
}

/** */
bool ParseResponse::Parse(string response)
{
	vector<string> tokens = Split(response, "\r\n");
	if (tokens.size() < 1)
		return !(_error=true);

	/** Procesar la Primera Linea */
	vector<string>::iterator it = tokens.begin();
	if (ParseStatus(*it)==false)
		return !(_error=true);	
	it++;

	/** Procesar los HEADERS */
	while (it != tokens.end() && ParseHeader(*it)==true)
	{
		it++;
	}

	/** Procesar el contenido. El primero debe ser una linea en blanco */
	if (*it != "")
		return false;
	it++;

	if (Header("Transfer-Encoding")=="chunked")
	{
		_body = ParseCkunked(tokens, it);
	}
	else
	{
		while (it != tokens.end())
		{
			_body += *it;
			it++;
		}
	}
	return !(_error=false);
}


/** */
vector<string> ParseResponse::Split(string entrada, string sep)
{
	size_t start = 0, end = 0;
	vector<string> tokens; 

	while ((end = entrada.find(sep, start)) != std::string::npos) 
	{
		tokens.push_back(entrada.substr(start, end - start));
		start = end + sep.length();
	}
  
	tokens.push_back(entrada.substr(start));
	return tokens;
}

/** */
bool ParseResponse::ParseStatus(string line)
{
	vector<string> partes = Split(line," ");
	if (partes.size() < 3)
		return false;

	_httpv = partes.at(0);;
	_status = partes.at(1);

	for (int i=2; i<(int )partes.size(); i++)
	{
		_status_text += (partes.at(i)+" ");
	}

	return true;
}

/** */
bool ParseResponse::ParseHeader(string line)
{
	vector<string> partes = Split(line,": ");
	if (partes.size() != 2)
		return false;

	_headers[partes.at(0)]=partes.at(1);
	return true;
}

/** */
string ParseResponse::ParseCkunked(vector<string> &data, vector<string>::iterator &it)
{
	string retorno;

	while (it != data.end())
	{
		int len = atoi((*it).c_str());
		if (len == 0)
			break;
		it++;

		if (it == data.end())
			break;
		
		retorno += *it;
		it++;

	}
	return retorno;
}

/** */
bool ParseResponse::Parse(CTCPSocket &sck,int timeout)
{		
	string line;
	int len=0;

	Tools::Trace("Parseando Respuesta HTTP...");

	/** Procesar la Primera Linea */
	if (sck.ReadLine(line, timeout)<=0)
		return false;
	Tools::Trace("Parseando Respuesta HTTP. Primera Linea Leida =  %s", line.c_str());

	if (ParseStatus(line) == false)
		return false;

	Tools::Trace("Parseando Respuesta HTTP. Primera Linea Procesada.");

	/** Procesar los HEADERS */
	do {
		len = sck.ReadLine(line, timeout);

		if (len < 0 )
			return false;
		
		Tools::Trace("Parseando Respuesta HTTP. Leido HEADER = %s.", line.c_str());
		ParseHeader(line);

	} while (len > 0);

	/** Leer el Body */
	if (Header("Transfer-Encoding")=="chunked")
	{
		Tools::Trace("Parseando Respuesta HTTP. Procesando CHUNK.");
		return ParseCkunked(sck, timeout);
	}
	else 
	{
		string strLen = Header("Content-Length");
		Tools::Trace("Parseando Respuesta HTTP. Procesando Content-Length %s.", strLen.c_str());

		len = Tools::atoi(strLen);
		if (len <= 0)
			return false;

		len = sck.Recv_text(_body, len, timeout);		
		if (len < 0)
			return false;
		Tools::Trace("Parseando Respuesta HTTP. Contenido Leido.");
	}
	return true;
}

/** */
bool ParseResponse::ParseCkunked(CTCPSocket &sck, int timeout)
{
	string data;
	string line;
	int lenline=0;
	bool retorno = true;
	do
	{
		Tools::Trace("Parseando Respuesta HTTP. Leyendo Chunk.");
		/** La primera Linea del chunk es la longitud en hexadecimal */
		lenline = sck.ReadLine(line, timeout);
		
		Tools::Trace("Parseando Respuesta HTTP. Chunk Length %s.", line.c_str());

		if (lenline > 0)
		{
			long chunklen = strtol(line.c_str(), NULL, 16);
			if (chunklen > 0) 
			{
				/** La siguiente linea es el chunk */
				lenline = sck.ReadLine(line, timeout);
				if (lenline > 0)
				{
					_body += line;
					Tools::Trace("Parseando Respuesta HTTP. Chunk leido.");
				}
				else if (lenline < 0)
					retorno = false;
			}
		}
		else if (lenline < 0)
			retorno = false;
		
	} while(lenline > 0);

	return retorno;
}
