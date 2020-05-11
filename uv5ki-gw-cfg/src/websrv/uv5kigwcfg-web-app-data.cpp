#include "../../include/websrv/uv5kigwcfg-web-app-data.h"
#include "../../include/cfg-proc.h"


/** */
webData_tses::webData_tses(int parStd, string parIdc, string parTim, string parModo, string version)
{	
	std = parStd;
	idc = parIdc;
	tim = parTim;
	ver = version;			// VersionGlobal();
	modo = parModo;

	ntpsync = IsNtpSync()==true ? 1 : 0;
	localdate = Tools::Ahora_Servidor();
	timr = P_CFG_PROC->LastServCfgTime();

	modo_redan = LocalConfig().get(strRuntime, strRuntimeItemModoRedan, "0");
	modo_16r = LocalConfig::p_cfg->getint(strRuntime, strRuntimeItemForceRdAudioNormal, "0");


#if LOCAL_TEST_1
	val_prueba1.push_back("Valor-1");
	val_prueba1.push_back("Valor-2");

	val_prueba2.push_back(0);
	val_prueba2.push_back(2);
	val_prueba2.push_back(1000);
	val_prueba2.push_back(-23);

	val_prueba3.push_back(new webData_line());
	val_prueba3.push_back(new webData_line("hola"));
#endif
}

/** */
void webData_tses::jwrite(Writer<StringBuffer> &writer)
{
	write_key/*_string*/(writer, "ver", ver);
	write_key/*_string*/(writer, "modo", modo);
	write_key/*_string*/(writer, "modo_redan", modo_redan);
	write_key/*_uint*/(writer, "std", std);
	write_key/*_string*/(writer, "ntpsync", ntpsync);
	write_key/*_string*/(writer, "localdate", localdate);
	write_key/*_string*/(writer, "idc", idc);
	write_key/*_string*/(writer, "tim", tim);
	write_key/*_string*/(writer, "timr", timr);
	write_key/*_string*/(writer, "modo_16r", modo_16r);
#if LOCAL_TEST_1
	write_key/*_object*/(writer, "msg", msg);
	write_key(writer, "ArrayPrueba1", val_prueba1);
	write_key(writer, "ArrayPrueba2", val_prueba2);
	write_key(writer, "ArrayPrueba3", val_prueba3);
#endif
}
