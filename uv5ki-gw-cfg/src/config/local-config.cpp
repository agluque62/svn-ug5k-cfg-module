#include "../../include/config/local-config.h"

/** */
LocalConfig *LocalConfig::p_cfg;
	/** 20171016. Mutex Global para controlar los accesos a ficheros INI */
pthread_mutex_t LocalConfig::global_mutex = PTHREAD_MUTEX_INITIALIZER;

///**
// */
//LocalConfig::LocalConfig(void) {
//	
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	namefile = strFile;
//	ini = LoadIni(namefile.c_str());
//    LoadStrings();
//}
//
///**
//*/
//LocalConfig::LocalConfig(string strfile)
//{
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	namefile = strfile;
//	ini = LoadIni(namefile.c_str());
//}
//
///**
// */
//LocalConfig::~LocalConfig(void) {
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//}
//
///** PRIVATE */
///**
// */
//void LocalConfig::LoadStrings() {
//
//    INISection strings = ini[strSectionStrings];
//    for (INISection::iterator item = strings.begin(); item != strings.end(); item++) {
//        _strings.ToIdent[item->first] = item->second;
//        _strings.ToKey[item->second] = item->first;
//    }
//}
//
///**
// */
//string LocalConfig::KeyToIdent(string key) {
//    MapString::iterator itKey = _strings.ToIdent.find(key);
//    return itKey != _strings.ToIdent.end() ? itKey->second : key;
//}
//
///**
// */
//string:IdentToKey(string ident) {
//    MapString::it LocalConfig:erator itIden = _strings.ToKey.find(ident);
//    return itIden != _strings.ToKey.end() ? itIden->second : ident;
//
//}
//
///**
// */
//void LocalConfig::GetKeyConfigLocal(INISection &config) {
//    config = ini[strSection];
//}
//
///**
// */
//void LocalConfig::SetKeyConfigLocal(string key, string data) {
//    PutIniSetting(ini, strSection, key.c_str(), data.c_str());
//}
//
///** PUBLIC */
///**
// */
//void LocalConfig::save(void) {
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	SaveIni(ini, namefile.c_str());
//}
//
///**
//*/
//void LocalConfig::sync()
//{
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//	ini = LoadIni(namefile.c_str());
//}
//
///**
// */
//string LocalConfig::PuertoEscucha(string puerto) {
//	
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	if (puerto != "NoPuerto")
//        PutIniSetting(ini, strSection, strItemWebPort, puerto.c_str());
//
//    return GetIniSetting(ini, strSection, strItemWebPort, "8080");
//}
//
///**
// */
//string LocalConfig::TiempoSesion(string segundos) {
//
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//    if (segundos != "NoSegundos")
//        PutIniSetting(ini, strSection, strItemSessionTime, segundos.c_str());
//
//    return GetIniSetting(ini, strSection, strItemSessionTime, "1200");
//}
//
///**
// */
////string LocalConfig::PathToIniRam(string path) {
////
////#ifdef _LOCAL_LOCK_
////	CCSLock _lock(_lock);
////#endif
////    if (path != "NoPath")
////        PutIniSetting(ini, strSection, strItemPath2Ini, path.c_str());
////
////    return GetIniSetting(ini, strSection, strItemPath2Ini, "./");
////}
//
///**
// */
////string LocalConfig::PathToIniFlash(string path) {
////
////#ifdef _LOCAL_LOCK_
////	CCSLock _lock(_lock);
////#endif
////    if (path != "NoPath")
////        PutIniSetting(ini, strSection, strItemPath2IniFlash, path.c_str());
////
////    return GetIniSetting(ini, strSection, strItemPath2IniFlash, "./");
////}
//
///**
//*/
//string LocalConfig::ConfigFile()
//{
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	return GetIniSetting(ini, strSection, ConfigTsup, "gw_config.json");
//}
//
///**
//*/
//int LocalConfig::ConfigTsup()
//{
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	string strtsup =  GetIniSetting(ini, strSection, strItemConfigTSUP, "2");
//	return atoi(strtsup.c_str());
//}
//
///**
// */
////string LocalConfig::PathToRCSH(string path) {
////#ifdef _LOCAL_LOCK_
////	CCSLock _lock(_lock);
////#endif
////
////	if (path != "NoPath")
////        PutIniSetting(ini, strSection, strItemPath2RCSH, path.c_str());
////
////    return GetIniSetting(ini, strSection, strItemPath2RCSH, "./");
////}
//
///** */
//string LocalConfig::PathToVersiones()
//{
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	return onfs(GetIniSetting(ini, strSection, strItemPath2Versiones, "./versiones.txt"));
//}
//
///** */
//string LocalConfig::Log() {
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	return GetIniSetting(ini, strSection, strItemLog, "0");
//}
//
///** */
//string LocalConfig::Login() {
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	return GetIniSetting(ini, strSection, strItemLogin, "0");
//}
//
///** */
//string LocalConfig::ServerURL() {
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//    return GetIniSetting(ini, strSection, strItemServidor, "127.0.0.1:8080/config");
//}
//
///** */
//string LocalConfig::NucleoProcName() {
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	return GetIniSetting(ini, strSection, strItemNucleoProcName, "testmemcompartida");
//}
//
///** */
//string LocalConfig::SnmpProcName() {
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	return GetIniSetting(ini, strSection, strItemSnmpProcName, "testmemcompartida");
//}
//
///** */
//string LocalConfig::RecordProcName() {
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	return GetIniSetting(ini, strSection, strItemRecordProcName, "testmemcompartida");
//}
//
///** */
//string LocalConfig::NetworkInterfaceActiva()
//{
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//	return GetIniSetting(ini, strSection, strItemNetworkInterfaceActiva, "eth0");
//}
//
///**
// *
// */
//string LocalConfig::LastRouteDest(string ip) {
//
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	if (ip != "GetIp")
//        PutIniSetting(ini, strSectionLast, strLastIpDest, ip.c_str());
//
//    return GetIniSetting(ini, strSectionLast, strLastIpDest, "127.0.0.1");
//}
//
///**
// *
// */
//string LocalConfig::LastRouteVia(string ip) {
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	if (ip != "GetIp")
//        PutIniSetting(ini, strSectionLast, strLastIpVia, ip.c_str());
//
//    return GetIniSetting(ini, strSectionLast, strLastIpVia, "127.0.0.1");
//}
//
///**
// *
// */
//string LocalConfig::LastRouteSource(string ip) {
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	if (ip != "GetIp")
//        PutIniSetting(ini, strSectionLast, strLastIpSource, ip.c_str());
//
//    return GetIniSetting(ini, strSectionLast, strLastIpSource, "127.0.0.1");
//}
//
///** */
////string LocalConfig::onram(string filename) {
////#ifdef _WIN32
////	return "./wram/" + filename;
////#else
////
////	return LocalConfig::cfg.PathToIniRam() + filename;
////#endif
////}
//
///** */
////string LocalConfig::onflash(string filename) {
//////    const char* env_p = std::getenv("PATH");
////#ifdef _WIN32
////	return /*"./wflash/"*/"./" + filename;
////#else
////    return LocalConfig::cfg.PathToIniFlash() + filename;
////#endif
////}
//
///** */
//string LocalConfig::onswrep(string filename) {
////#ifdef _WIN32
////	return "./wflash/swrep/" + filename;
////#else
////    return onflash("swrep/" + filename);
////#endif
//    return onflash("swrep/" + filename);
//}
//
///**
//*/
//string LocalConfig::getString(string section, string item, string def)
//{
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	return GetIniSetting(ini, section.c_str(), item.c_str(), def.c_str());
//}
//
///**
//*/
//void LocalConfig::setString(string section, string item, string valor)
//{
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	PutIniSetting(ini, section.c_str(), item.c_str(), valor.c_str());
//}
//
///**
//*/
//int LocalConfig::getInt(string section, string item, string def)
//{
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	return atoi(GetIniSetting(ini, section.c_str(), item.c_str(), def.c_str()).c_str());
//}
//
///**
//*/
//void LocalConfig::setInt(string section, string item, int valor)
//{
//	char buffer[32];
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//#ifdef _WIN32
//	sprintf_s (buffer,"%d",valor);
//#else
//	sprintf (buffer,"%d",valor);
//#endif
//	PutIniSetting(ini, section.c_str(), item.c_str(), buffer);
//}
//
///**
//*/
//INISection LocalConfig::getSection(string section)
//{
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	return ini[section.c_str()];
//}
//
///**
//*/
//void LocalConfig::deleteSection(string section)
//{
//	INISection vacia;
//#ifdef _LOCAL_LOCK_
//	CCSLock _lock(_lock);
//#endif
//
//	ini[section.c_str()] = vacia;
//}
//
