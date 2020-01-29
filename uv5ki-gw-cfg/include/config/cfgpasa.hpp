/******************************************************************************

   Nombre: CFGPASA.HPP
   Descripcion: Datos de configuracion de la pasarela del CD-40 y sus recursos.

   $Revision: 1.9 $
   $Date: 2011/03/08 09:22:05 $
   
   Fecha ultima modificacion: 10/01/2011                                 

   Notas: 
   
   Los #defines de tamanos minimos deberian ser compartidos con otros fuentes.
   
                   
******************************************************************************/

#ifndef INCLUIDO_CFGPASA_PUNTOHACHE
#define INCLUIDO_CFGPASA_PUNTOHACHE

//#include <pthread.h>

//------------------------------------------------------------
//  Definiciones
//------------------------------------------------------------
#ifndef LongWord
	#define LongWord unsigned int
#endif

#define MODO_ULISES		0
#define MODO_REDAN		1


#define PRECISION_ESTRICTA 0
#define PRECISION_NORMAL   1

//Identificacion de configuracion vacia

#define ID_CFG_VACIA	"ESTA-VACIA"

/*
 * Dimensionamiento
 */
 
#define MAX_SLOTS_PASARELA  4

#define CFG_MAX_LONG_NOMBRE_RECURSO         64			// 32
#define CFG_MAX_LONG_NOMBRE_PASARELA        64			// 32

#define NUM_SIP_REGISTRAR   3
#define NUM_SIP_PROXY       3
#define NUM_SRV_NTP         3

#define MAX_LONG_DIRIP      64
#define CFG_MAX_LONG_URL 	MAX_LONG_DIRIP

#define MAX_LONG_DIR_AMPLIADA 	80
#define MAX_LONG_DIR_URI    	MAX_LONG_DIR_AMPLIADA

#define MAX_NUM_LISTA_BLANCA  8
#define MAX_NUM_LISTA_NEGRA   8
#define MAX_LONG_CLAVE        8

#define MAX_NUM_EMPL          4

#define MAX_NUM_COLATERALES     MAX_NUM_EMPL*4

#define LONG_AB_ATS   6
#define PREF_ATS		3

#define CFG_COORDS_LOCALIZADOR          2  // Coordenadas para lozalizar un rec. ( slot+dispositivo ).

#define CFG_LONG_ALIAS                  32
#define CFG_MAX_LONG_ID                 50
#define CFG_MAX_LONG_ID_CONFIG   ((2*CFG_MAX_LONG_ID)+(CFG_MAX_LONG_NOMBRE_PASARELA)+2)


#define MAX_RECURSOS_TIFX               16
#define MAX_ENLACES_EXTERNOS_POR_REC    32

#define N_MAX_RANGOS_ATS    4

#define CFG_MAX_LONG_NOMBRE_RED       32
#define CFG_MAX_LONG_NOMBRE_TRONCAL    32

#define	MAX_TM_LLAMADA_ENTRANTE		60			//maximo tiempo de llamada entrante para lineas de 2 hilos
#define MAX_TM_DET_FINLLAMADA		8			//maximo tiempo para detectar que ha finalizado la llamada entrante rtb (ringon-ringoff), es decir, reposo desde el ultimo ringoff

#define	MIN_PERIODO_SPV_RING		50
#define	MAX_PERIODO_SPV_RING		400
#define	DEF_PERIODO_SPV_RING		200
#define	MIN_FILTRO_SPV_RING			1
#define	MAX_FILTRO_SPV_RING			6
#define	DEF_FILTRO_SPV_RING			2

/*
 * Tipos de sincronizacion.
 */

#define CFG_MODO_SINC_NINGUNO       0  // Sin sincronziacion.
#define CFG_MODO_SINC_PTP           1  // Protocolo IEEE 1588 "PTP".
#define CFG_MODO_SINC_NTP           2  // Protocolo NTP.
#define CFG_MODO_SINC_PROPIETARIO   3  // Protocolo propietario de Page.

#define CFG_TOTAL_MODOS_SINC        4

#define NO_CLIMAXDELAY     0
#define CLD_ASAP		   1
#define CLD_TM_FIJO        2


#define MODO_CALC_CLD_RELATIVO	0
#define MODO_CALC_CLD_ABSOLUTO	1

/*
 * Tipos de recurso.
 */

#define CFG_REC_TIPO_DESCONOCIDO   -1

#define CFG_REC_TIPO_AUDIO_RX       0
#define CFG_REC_TIPO_AUDIO_TX       1
#define CFG_REC_TIPO_AUDIO_TXRX     2
#define CFG_REC_TIPO_AUDIO_HFTX     3
#define CFG_REC_TIPO_DATOS_TX       4
#define CFG_REC_TIPO_DATOS_RX       5
#define CFG_REC_TIPO_DATOS_TXRX     6

#define CFG_REC_TOTAL_TIPOS         7


/*
 * Tipos de interfaz de recurso.
 */

#define CFG_IFREC_TIPO_DESCONOCIDO -1

#define CFG_IFREC_TIPO_RADIO        0
#define CFG_IFREC_TIPO_LCEN         1
#define CFG_IFREC_TIPO_BC           2
#define CFG_IFREC_TIPO_BL           3
#define CFG_IFREC_TIPO_AB           4
#define CFG_IFREC_TIPO_R2           5
#define CFG_IFREC_TIPO_N5           6
#define CFG_IFREC_TIPO_QSIG         7
#define CFG_IFREC_TIPO_RDSI_2BD     8
#define CFG_IFREC_TIPO_RDSI_30BD    9
#define CFG_IFREC_TIPO_IO          10
#define CFG_IFREC_TIPO_DATOS       11
#define CFG_IFREC_TIPO_RRC         12
#define CFG_IFREC_TIPO_EYM_PP      13
#define CFG_IFREC_TIPO_EYM_MARC    14
#define CFG_IFREC_TIPO_TUN_LOCAL   15
#define CFG_IFREC_TIPO_TUN_REMOTO  16

#define CFG_IFREC_TOTAL_TIPOS      17
//tipo de proxy. el tipo viene en la informacion de tipo de recurso. para que no se solapen tipos, deben valer por encima de los tipos de recursos que hay configurados

#define TIPO_PROXY_PRINCIPAL		CFG_IFREC_TOTAL_TIPOS
#define TIPO_PROXY_ALTERNATIVO		CFG_IFREC_TOTAL_TIPOS+1


/*
 * Tipos de Radio REDAN
 */

#define TIPO_RD_LOCAL_SIMPLE       0
#define TIPO_RD_LOCAL_PR           1
#define TIPO_RD_LOCAL_FD_SIMPLE    2
#define TIPO_RD_LOCAL_FD_PR        3
#define TIPO_RD_REMOTO_RXTX        4
#define TIPO_RD_REMOTO_TX          5
#define TIPO_RD_REMOTO_RX          6



/* 
 * Ganancia / AGC.
 */
 
#define CFG_GANANCIA    0 
#define CFG_AGC         1 


/* 
 * Codecs preferidos.
 */
 
#define CFG_CODEC_G711_LEY_A    0
#define CFG_CODEC_G711_LEY_MU   1


/*
 * Tipos de SQ.
 */

// Segun documento de refencia Configuracion del sistema CD40.doc
// y conversacion  11-12-08 estos defines quedan derogados asi como el tipo int.
/*
#define CFG_SQ_HW             0
#define CFG_SQ_TONO           1
#define CFG_SQ_VAD            2

#define CFG_TOTAL_TIPOS_SQ    3
*/

#define sCFG_SQ_HW             'h'
#define sCFG_SQ_TONO           't'
#define sCFG_SQ_VAD            'v'



/*
 * Tipos de SQ.
 */

// Segun documento de refencia Configuracion del sistema CD40.doc
// y conversacion  11-12-08 estos defines quedan derogados asi como el tipo int.
/*
#define CFG_PTT_HW      0
#define CFG_PTT_TONO    1
#define CFG_PTT_AMBOS   2

#define CFG_TIPOS_PTT   3
*/

#define sCFG_PTT_HW      'h'
#define sCFG_PTT_TONO    's'
#define sCFG_PTT_AMBOS   'm'



/*
 * Tipos de BSS que puede tener un canal local
 * (coincide con los defines de para rellenar el sdp en una llamada radio)
 */

#define CFG_METBSS_NO             0
#define CFG_METBSS_RSSI       	  1
#define CFG_METBSS_RSSI_NUCLEO    2

/*
 * BSS preferido para un canal remoto
 */
#define CFG_PREF_BSS_RSSI    0
#define CFG_PREF_BSS_NUCLEO    1

/*
 * Tipos de NTZ.
 */

#define CFG_NTZ_NINGUNO        -1
#define CFG_NTZ_CONTACTO_SECO   0
#define CFG_NTZ_MASA            1
#define CFG_NTZ_TENSION         2

#define CFG_TOTAL_TIPOS_NTZ     3


/*
 * Modos de confirmacion de PTT en cabeceras RTP.
 */

#define CFG_CONFIRM_PTT_NINGUNA         0
#define CFG_CONFIRM_PTT_A_TODOS         1
#define CFG_CONFIRM_PTT_AL_INTERESADO   2
#define CFG_TOTAL_TIPOS_CONFIRM_PTT     3


/*
 * Lados del ATS-R2/ATS-N5/ATS-QSIG.
 */

#define CFG_LADO_A          0
#define CFG_LADO_B          1
#define CFG_TOTAL_LADOS_ATS 2


/*
 * Modos del ATS-QSIG.
 */

#define CFG_MODO_USER         0
#define CFG_MODO_NET          1
#define CFG_TOTAL_MODOS_RDSI  2

/*
* Tipos EyM
*/

enum eType
    {
        CFG_EyM_TYPE_I,
        CFG_EyM_TYPE_II,
        CFG_EyM_TYPE_III,
        CFG_EyM_TYPE_IV,
        CFG_EyM_TYPE_V,
    };
#define CFG_TOTAL_TYPES_EYM   5

/*
* Modo EyM
*/
#define CFG_EyM_2W            0
#define CFG_EyM_4W            1
#define CFG_TOTAL_MODOS_EYM   2

/*
* Lado EyM
*/
#define CFG_EyM_LADO_EyM        0
#define CFG_EyM_LADO_PLR        1
#define CFG_TOTAL_LADOS_EYM     2
 


/*
 * Rangos validos para algunos otros numeritos.
 */
 
#define CFG_SLOT_NINGUNO    -1
#define CFG_MIN_SLOT         1   // Numero minimo de slot para un recurso. 
#define CFG_MAX_SLOT         4   // Numero minimo de slot para un recurso.

#define CFG_DISPOSITIVO_NINGUNO    -1
#define CFG_MIN_DISPOSITIVO         1   // Numero minimo de de dispositivo (dentro de slot). 
#define CFG_MAX_DISPOSITIVO         4   // Numero minimo de de dispositivo (dentro de slot).
 
#define CFG_MIN_FLUJOS_PTT_MEZCLA   2
#define CFG_MAX_FLUJOS_PTT_MEZCLA   7
 

/* Restriccion de llamadas */

#define SIN_RESTRICCION            0
#define RESTRICCION_LISTA_BLANCA   2
#define RESTRICCION_LISTA_NEGRA    1


#define TM_MAX_SUPERV_OPTIONS		20

/*
 * Otros numeros.
 */

#define CFG_SIP_SIN_SUPERV             0
#define CFG_MAX_TIEMPO_PTT_INFINITO    0
#define CFG_PTT_LOCKOUT                1
 
/* strings
 *
 */
#define ID_DESTINO_DESCONOCIDO      "id.desconocido.destino.cd40"
#define STR_NULO                    "---"


#define N_MAX_RANGOS        16
#define N_MAX_PRIV          8

#define BSSC_QIDXVAL		16

//------------------------------------------------------------------------------------------
//  datos y estructuras del sistema Ulises
//------------------------------------------------------------------------------------------


#define LONG_AB             	19
#define N_ABONADOS_BY_TV        32
#define N_MAX_TRONCALES         128
#define N_MAX_CENTRALES         8
#define N_RUTAS_BY_CENTRAL      4
#define N_MAX_RUTAS         	(N_MAX_CENTRALES*N_RUTAS_BY_CENTRAL)
#define N_TRONCALES_RUTA        8
#define N_MAX_REC_BY_TRONCAL    8
#define N_MAX_REDES         	16
#define N_MAX_REC_BY_NET        128
#define N_MAX_TV            	256
#define N_MAX_TIFX          	64
#define N_MAX_REC_BY_TIFX       16
#define N_MAX_REC_SISTEMA       N_MAX_TIFX*N_MAX_REC_BY_TIFX


#define CFG_MAX_LONG_NOMBRE     CFG_MAX_LONG_NOMBRE_RECURSO    //igualo los nombres al mismo tamano

/*#define LONG_NAME           32
#define LONG_NAME_TV            LONG_NAME
#define LONG_NAME_REC           LONG_NAME
#define LONG_NAME_RUTA          LONG_NAME
#define LONG_NAME_CENTRAL       LONG_NAME
#define LONG_NAME_TRONCAL       LONG_NAME
*/

#define RUTA_DIRECTA        0
#define RUTA_ALTERNATIVA    1

//tipo de interface para cada recurso en la structura struct st_plan_recursos

#define TI_Radio	0
#define TI_LCEN		1
#define TI_BC		2
#define TI_BL		3
#define TI_AB		4
#define TI_ATS_R2	5
#define TI_ATS_N5	6
#define TI_ATS_QSIG	7
#define TI_ISDN_2BD	8
#define TI_ISDN_30BD	9
#define TI_I_O		10
#define TI_DATOS	11

#define MAX_PREFIJOS	99

//--------------------- Finalizacion de tablas ---------------------------------/
#define NO_NAME                 0
#define NO_PREFIJO              255
#define CENTRAL_NO_DEF          2
#define NO_INICIAL              0
#define NO_TRONCAL              0
#define NO_TIPO_RUTA            255
#define NO_REC                  0
#define NO_HOST                 0
#define NO_USUARIO              0
//--------------------- Finalizacion de tablas ---------------------------------/


struct st_direccionamientoproxy {
    char idhost[CFG_MAX_LONG_NOMBRE];/*NO_NAME fin de tabla*/
    char IpRed1[MAX_LONG_DIRIP];
    char IpRed2[MAX_LONG_DIRIP];
    char IpRed3[MAX_LONG_DIRIP];
    char SrvPresenciaIpRed1[MAX_LONG_DIRIP];
    char SrvPresenciaIpRed2[MAX_LONG_DIRIP];
    char SrvPresenciaIpRed3[MAX_LONG_DIRIP];
    bool operator ==( const st_direccionamientoproxy& );
    bool operator !=( const st_direccionamientoproxy& );
};


struct st_direccionamientoip {
    char idhost[CFG_MAX_LONG_NOMBRE];/*NO_NAME fin de tabla*/
    char ipred1[MAX_LONG_DIRIP];
    char ipred2[MAX_LONG_DIRIP];
    unsigned char tipohost;
    bool operator ==( const st_direccionamientoip& );
    bool operator !=( const st_direccionamientoip& );
};


struct st_asignacionusuario_tv {
    char idusuario [CFG_MAX_LONG_NOMBRE];
    char idhost[CFG_MAX_LONG_NOMBRE];/*NO_NAME fin de tabla NO_HOST*/
    bool operator ==( const st_asignacionusuario_tv& );
    bool operator !=( const st_asignacionusuario_tv& );
};


struct st_asignacionrecursos_gw {
    char idrecurso [CFG_MAX_LONG_NOMBRE];
    char idhost[CFG_MAX_LONG_NOMBRE];/*NO_NAME fin de tabla NO_HOST*/
    bool operator ==( const st_asignacionrecursos_gw& );
    bool operator !=( const st_asignacionrecursos_gw& );
};


struct st_no_abonado {
    unsigned int prefijo;/*NO_PREFIJO fin de tabla*/
    char abonado [LONG_AB];
    bool operator ==( const st_no_abonado& );
    bool operator !=( const st_no_abonado& );
};


struct st_direccionamiento_sip  {
    char idusuario [CFG_MAX_LONG_NOMBRE];/*NO_NAME fin de tabla NO_USUARIO*/
    struct st_no_abonado array_abonados[N_ABONADOS_BY_TV];
    bool operator ==( const st_direccionamiento_sip& );
    bool operator !=( const st_direccionamiento_sip& );
};


struct st_plan_recursos {
    unsigned char tipo;
    unsigned char idrec [CFG_MAX_LONG_NOMBRE];/*0 para indicar fin de tabla NO_REC*/
    bool operator ==( const st_plan_recursos& );
    bool operator !=( const st_plan_recursos& );
};


struct st_listaredes {
    char idred [CFG_MAX_LONG_NOMBRE];
    unsigned int prefijo;/*prefijo 255 para indicar fin tabla*/
    struct st_plan_recursos listarecursos [N_MAX_REC_BY_NET];
    bool operator ==( const st_listaredes& );
    bool operator !=( const st_listaredes& );
};


struct st_listatroncales {
    char idtroncal [CFG_MAX_LONG_NOMBRE];/*0 para indicar fin de tabla NO_TRONCAL*/
    char no_test [LONG_AB_ATS];
    struct st_plan_recursos listarecursos [N_MAX_REC_BY_TRONCAL];
    bool operator ==( const st_listatroncales& );
    bool operator !=( const st_listatroncales& );
    bool ConfirmaRecurso( char* );
};


struct st_planrutas {
    unsigned char tiporuta; /* directa o alternativa,255 fin tabla(NO_TIPO_RUTA)*/
    char listatroncales [N_TRONCALES_RUTA][CFG_MAX_LONG_NOMBRE];
    bool operator ==( const st_planrutas& );
    bool operator !=( const st_planrutas& );
    bool ConfirmaTroncal( char* );
};

/*
 * Rango con los numeros ATS en binario.
 */


struct st_rango_binario
{
    LongWord lwInicial;
    LongWord lwFinal;
};


struct st_rango {
    char inicial [LONG_AB_ATS];/*0 para indicar fin de tabla*/
    char final [LONG_AB_ATS];
    struct st_no_abonado abonado;
    bool operator ==( const st_rango& );
    bool operator !=( const st_rango& );
    void EnBinario( struct st_rango_binario* );
};


struct st_numeracionats {
    char nombre[CFG_MAX_LONG_NOMBRE];
    unsigned char centralpropia; /*Mi Central,CENTRAL_NO_DEF para indicar fin tabla */;
    unsigned char throwswitching;
    char no_test [LONG_AB_ATS];
    struct st_rango rangosoperador[N_MAX_RANGOS]; /*array de rangos de operdores */
    struct st_rango rangosprivilegiados[N_MAX_PRIV]; /*array de rangos de ope.privilegiados */
    struct st_planrutas listarutas [N_MAX_RUTAS];
    bool operator ==( const st_numeracionats& );
    bool operator !=( const st_numeracionats& );
    bool ConfirmaTroncal( char* );
};


/*
* Estructura: cfgEnlaceExterno
* Introducido 10 de Marzo de 2009
*/

struct cfgEnlaceExterno
{
   char szId[CFG_MAX_LONG_NOMBRE+1];
};


//------------------------------------------------------------------------------------------

//------------------------------------------------------------
//  Tipos de datos
//------------------------------------------------------------

/*
 * Estructura: LocalizadorRecurso.
 * Descripcion: Coordenadas de ubicacion de un recurso en la pasarela.
 * Campos->
 *     iSlot: Ranura del bastidor (0-3)
 *     iDispositivo: Dispositivo dentro de la ranuda (0-3).
 */
 
struct LocalizadorRecurso
{
    int iSlot;
    int iDispositivo;
};

/*
 * Estructura: st_rangoATS.
 * Descripcion: Configuracion del colateral PP para un interfaz Tlf/LC
 *
 * Campos->
    szDestino: id de la frecuencia
    iTipoConmutacion: M manual / A automatica
    asEmpl: array de informacion de cada emplazamiento
 *
 */

struct st_rangoATS {
    char inicial [LONG_AB_ATS];/*0 para indicar fin de tabla*/
    char final [LONG_AB_ATS];
};



/*
 * Estructura: cfgColateralPP.
 * Descripcion: Configuracion del colateral PP para un interfaz Tlf/LC
 *
 * Campos->
    szDestino: id de la frecuencia
    iTipoConmutacion: M manual / A automatica
    asEmpl: array de informacion de cada emplazamiento
 *
 */

struct cfgColateralPP
{
    int iRespuestaAutomatica;
    char szUriRemota[MAX_LONG_DIR_AMPLIADA+1];
    int isuperv_options;                    /* temporizador supervision options */
    int itm_superv_options;                 /* temporizador supervision options */
    int iColateralSCV;                    /* indica si el colateral es un recurso de una pasarela (0) o un SCV (1)*/

};


//esta estructura se deja de momento porque en redan se utiliza, se pasa cfgsistema a los recursos para que tengan la ip para darla a grabación.
struct st_config_sistema
{
    char szDirVirtual[CFG_MAX_LONG_URL+1];
};


/*
 * Estructura: cfgConfigGeneralRecurso.
 * Descripcion: Configuracion general de un recurso de cualquier tipo.
 * Campos->
 *     szNombre: Nombre del narciso.
 *     iTipo: Tipo de recurso. En CD-40 siempre es AUDIO.
 *     iTipoIf: Tipo de interfaz del recurso-
 *     iSlot: N de slot en la pasarela donde vive el recurso (de 0 a 3).
 *     iDispositivo: N de dispositivo en el slot (de 0 a 3).
 *     szDestino: ID del destino asociado al recurso.
 *     szDirMiSipRegistrar: Direccion del registrar server a usar por el recurso.
 *                          Si es NULL o \0 no se registra en ningn sitio.
 *     iFlgUsarDiffServ: 0=No usar protocolo DiffServ. 1=Usarlo.
 *     aListaEnlacesExternos: Lista de enlaces externos autorizados a establecer sesiones
 *                            con el recurso. Si es NULL no existe tal lista.
 *                            La lista de enlaces externos solo deberian ser manejada por el Nucleo.
 *                            Los recursos no la deberian necesitar si el ncleo se encarga del
 *                            pre-filtrado de las sesiones entrantes.
 */
 
struct cfgConfigGeneralRecurso
{
    char szNombre[CFG_MAX_LONG_NOMBRE_RECURSO+1];
    int iTipo;
    int iTipoIf;
    int iSlot;
    int iDispositivo;
    char szDestino[CFG_MAX_LONG_NOMBRE_RECURSO+1];
    int iCfgModoSistema;
    // int iNumSesionesRtpEstaticas;
    int iFlgUsarDiffServ;
    int iSnmpPuertoRemoto;
    int iRecPuertoRemoto;
    int iRecPuertoBase;
    int iEnableGI;

    char szUriLocal[MAX_LONG_DIR_AMPLIADA+1];
    int iLLamadaAutomatica;
    int iRestriccion;
    char szListaBlanca[MAX_NUM_LISTA_BLANCA][MAX_LONG_DIR_AMPLIADA+1];
    char szListaNegra[MAX_NUM_LISTA_NEGRA][MAX_LONG_DIR_AMPLIADA+1];
    int iEnableRegistro;
    char szClave[MAX_LONG_CLAVE+1];
    struct cfgColateralPP  sColateral;

    int size_aListaEnlacesExternos;
    struct cfgEnlaceExterno aListaEnlacesExternos[MAX_ENLACES_EXTERNOS_POR_REC];
    cfgConfigGeneralRecurso();
    void TomaSlot( int );
    void TomaDispositivo( int );
    void PorDefecto();
    bool operator ==( const cfgConfigGeneralRecurso& );
    bool operator !=( const cfgConfigGeneralRecurso& );
    };


/*
 * Estructura: cfgConfigRecAudio.
 * Descripcion: Configuracion de las cosas da audio de un recurso evidentemente de audio.
 * Campos->
 *     iSelGananciaAgcTx: Selecciona CFG_GANANCIA o CFG_AGC en TX.
 *     iGananciaAgcTx: Ganancia o nivel de senal deseado (AGC) en TX.
 *     iSelGananciaAgcRx: Selecciona CFG_GANANCIA o CFG_AGC en RX.
 *     iGananciaAgcRx: Ganancia o nivel de senal deseado (AGC) en RX.
 *     iHaySupresionSilencio: 0=No hay supresion de silencios. !=0 Si la hay.
 *     iTamanoRtp: Tamano de los paquetes RTP (si el codec es de tamano fijo). En ms.
 *     iCodecPreferido: Codec preferido por el recurso.
 */
 
struct cfgConfigRecAudio
{
    int iSelGananciaAgcTx;
    int iGananciaAgcTx;
    int iSelGananciaAgcRx;
    int iGananciaAgcRx;
    int iHaySupresionSilencio;
    int iTamanoRtp;
    int iCodecPreferido;
    int iBufferJitterMin;
    int iBufferJitterMax;
    
    void TomaCodecPreferido( int );
    void PorDefecto();
//    cfgConfigRecAudio();
};


/*
 * Estructura: cfgConfigRecDatos.
 * Descripcion: Configuracion de las cosas da datos de un recurso evidentemente de datos.
 *              Solo para SCIP. No se usa en CD-40.
 * Campos->
 *   
 *   
 *   
 */
 
struct cfgConfigRecDatos
{
    unsigned int uiBitsPorSegundo;

    void PorDefecto();
//    cfgConfigRecDatos();
};


/*
 * Estructura: cfgEmplazamiento.
 * Descripcion: Configuracion de las uris de un emplazamiento remoto desde el punto de vista de de un canal radio local
 *
 * Campos->
 * UriTxA: URI del transmisor principal
   UriTxB: URI del transmisor reserva
   cActivoTx: transmisor activo
   UriRxA: URI del receptor principal
   UriRxB:  URI del receptor reserva
   cActivoRx: receptor activo
 *
 */

struct cfgEmplazamiento
    {
        char UriTxA[MAX_LONG_DIR_AMPLIADA+1];
        char UriTxB[MAX_LONG_DIR_AMPLIADA+1];
        char cActivoTx;
        char UriRxA[MAX_LONG_DIR_AMPLIADA+1];
        char UriRxB[MAX_LONG_DIR_AMPLIADA+1];
        char cActivoRx;
    };


/*
 * Estructura: cfgColateralRadio.
 * Descripcion: Configuracion del colateral de una radio local
 *
 * Campos->
    szDestino: id de la frecuencia
    iTipoConmutacion: M manual / A automatica
    asEmpl: array de informacion de cada emplazamiento
 *
 */

struct cfgColateralRadio
{
//    char szDestino[CFG_MAX_LONG_NOMBRE_RECURSO+1];
    struct cfgEmplazamiento asEmpl[MAX_NUM_EMPL];
};



/*
 * Estructura: cfgConfigIfRadio.
 * Descripcion: Configuracion del interfaz de radio de un recurso.
 * Campos->
 *     iTipoPtt: Tipo de PTT (HW, SW, o ambos).
 *     iTipoSq:  Tipo de SQ (HW, VOX, SW).
 *     iEyM: El caso es que sigo sin saber que significa esto exactamente.
 *     iUmbralDetTonoE: En dBm.
 *     iNivelTonoM: En dBm.
 *     iUmbralDetSq: En dBm.
 *     iNivelTonoPtt: En dBm.
 *     iMetodoBss: Metodo de BSS a utilizar. 
 *     iNtz: Tipo de senal NTZ.
 *     iTiempoMaxPtt: Tiempo minimo de PTT en segundos.
 *     iTiempoPTTBloqueado: Tiempo que el PTT permanece bloqueado despues de rebasarse
 *                          iTiempoMaxPtt. En segundos
 *     iSupervisionPortadoraTx: El caso es que sigo sin saber que significa esto exactamente.
 *     iSupervisionModuladoraTx: El caso es que sigo sin saber que significa esto exactamente.
 *     iModoConfirmacionPtt: Modo de confirmacion del PTT por RTP.
 *     iPeriodoRtpSqBss: Periodo de repeticion de estados SQ y BSS, medido en paquetes RTP.
 *                       En milisegundos. (Vease seccion  3.12.1.2. de [ED137P1]) 
 *     iNumPaquetesSqOff: Numero de paquetes de desactivacion de SQ 
 *                        (Vease seccion  3.12.1.2. de [ED137P1])
 *     iPttTimeOut: Timeout de activacion de PTT.En milisegundos. (Vease seccion  3.12.1.2. de [ED137P1])
 *     iNumFlujosMezcla: Nmero minimo de flujos de audio que pueden transmitir en mezcla. 
 */
 
struct cfgConfigIfRadio
{
	int iTipoRadio;
	char cTipoPtt;
    char cTipoSq;
    bool iEyM;
//    int iUmbralDetTonoE;
//    int iNivelTonoM;
    int iUmbralDetSq;
    int iNivelTonoPtt;
    int iMetodoBss;
    int iNtz;
    int TiempoMaxPtt;
    int iSupervisionPortadoraTx;   
    int iSupervisionModuladoraTx;   
    int iModoConfirmacionPtt;   
    int iPeriodoRtpSqBss;   
    int iNumPaquetesSqOff;   	////iNumPaquetesSqOff en realidad es SQ Forzado o no
    int iPttTimeOut;
    int iNumFlujosMezcla;
    unsigned int KeepAlivePeriod;
    unsigned int KeepAliveMultiplier;
    int iSessionPrio;
    int iPttPrio;
    int iPrecisionAudio;     //precision del recurso radio (5 o 20 grenularidad)

    int iTiempoPttBloqueado;    // 2000 por defecto.
    int iRetrasoPttOff;         // 0 por defecto.
    int iRetrasoSqOff;          //  0 por defecto.

    int iTiempo_VentanaRx;
    int iClimaxDelay;
    int iTmRetardoFijo;
    int iModoCalculoClimax;		//MODO_CALC_CLD_RELATIVO- ABSOLUTO
    int iBssRtp;
    char cTipoConmutacion;
    int iEvtPTT;     //0 no envia 1 envia
    int iTjbd;        //jitter buffer delay
    int iTGRSid;         //TGRS internal delay


    struct cfgColateralRadio  sColateral;

    char qidxNdf2Rssi[BSSC_QIDXVAL];

    void TomaTipoPtt( char );
    void TomaTipoSq( char  );
    void TomaMetodoBss(char activo, int tipo);
    void TomaNtz( int );
    void TomaModoConfirmPtt( int );
    void TomaNumFlujosMezcla( int );
    void PorDefecto();
    bool operator ==( const cfgConfigIfRadio &sLaOtra );
    bool operator !=( const cfgConfigIfRadio &sLaOtra );
//    cfgConfigIfRadio();
};



/*
 * Estructura: cfgConfigIfLcen.
 * Descripcion: Configuracion del interfaz de telefonon de un recurso.
 * Campos->
 *     iUmbralRxTonoLcen: Umbral de deteccion del tono de linea caliente. En dBm.
 *     iNivelTxTonoLcen: Nivel de generacion del tono de linea caliente. En dBm.
 *     iPeriodoRefrescoEstado: Periodo de refresco del estado por RTP. En ms.
 *     iTimeoutEstado: Timeout de recepcion del estado por RTP. En ms.
 *     iLongRafagas:   Longitud de rafagas de cambio de estado por RTP. En paquetes.
 *     iT1...iT12: Tiempos de linea caliente.
 */
 
struct cfgConfigIfLcen
{
    int iUmbralRxTonoLcen;
    int iNivelTxTonoLcen;
    int iPeriodoRefrescoEstado;
    int iTimeoutEstado;
    int iLongRafagas;
    int iT1;
    int iT1Max;
    int iT2;
    int iT2Max;
    int iT3;
    int iT4;
    int iT4Max;
    int iT5;
    int iT5Max;
    int iT6;
    int iT6Max;
    int iT8;
    int iT8Max;
    int iT9;
    int iT9Max;
    int iT10;
    int iT10Max;
    int iT11;
    int iT11Max;
    int iT12;

    int iT_Release;
    void PorDefecto();
};


/*
 * Estructura: cfgConfigIfR2N5.
 * Descripcion: Configuracion del interfaz R2/N5 de un recurso
 * Campos->
 *     iLado: CFG_LADO_A o CFG_LADO_B.
 *     Tiempos:P1: Tiempo de toma de linea.
 *     Tiempos:P2: Tiempo entre la toma de linea y primer digito.
 *     Tiempos:P3: Tiempo entre senal de registro -digito o estado- y en inicio del ACK de la misma.
 *     Tiempos:P4: Tiempo desde el inicio de un ACK hasta el fin de la senal de registro correspondiente.  
 *     Tiempos:P5: Tiempo desde fin de digito hasta fin de ACK digito.
 *     Tiempos:P6: Tiempo de silencio despues del ultimo digito ACK.  
 *     Tiempos:P7: Tiempo entre p5 y nuevo digito  
 *     Tiempos:P8: Tiempo entre la senal de ACK estado y la de Release 
 *     Tiempos:P9: Tiempo de la senal de Release  
 *     Tiempos:P10     
 *     Tiempos:P11: Tiempo antes de generar tono de llamada  
 *     Tiempos:P12  
 *     Tiempos:P13: Interrupt warning  
 *     Tiempos:P14  
 *     Tiempos:P15  
 *     Tiempos:P16: Blocking  
 *     Tiempos:P17: Silencio en rellamada  
 *     Tiempos:P18: Silencio antes de liberar  
 *     Tiempos:P19  
 *     Tiempos:P20: Tiempo entre la senal de ACK estado y la de Release  
 *   
 *   
 */

struct cfgConfigIfR2N5
{
    int iLado;
    struct Tiempos
    {
        int iP1Min;
        int iP1Nom;
        int iP1Max;
        int iP2;
        int iP2Tout;
        int iP3;
        int iP3Tout;
        int iP4;
        int iP4Tout;
        int iP5;
        int iP5Tout;
        int iP6;
        int iP6Tout;
        int iP7;
        int iP7Tout;
        int iP8;
        int iP8Tout;
        int iP9Min;
        int iP9Nom;
        int iP9Max;
        int iP11;
        int iP16Min;
        int iP16Nom;
        int iP16Max;
        int iP18;
    } sTiemA, sTiemB;

    int iT_Release;                              //tiempo generando tonos antes de liberar
    int iT_Int_Warning;   //tiempo de duracion de los tonos de interrupt warning

    char ntest_local[LONG_AB_ATS];
    char ntest_remoto[LONG_AB_ATS];
    struct st_rangoATS rangos_dst[N_MAX_RANGOS_ATS];      /*array de rangos de nuemros destino */
    struct st_rangoATS rangos_org[N_MAX_RANGOS_ATS];      /*array de rangos de numeros origen */

    char szIdTroncal[CFG_MAX_LONG_NOMBRE_TRONCAL+1];
    char szIdRed[CFG_MAX_LONG_NOMBRE_RED+1];
    int iNumRangosOperador;
    int iNumRangosPrivilegiados;
    struct st_rango_binario asMisRangosOperador[N_MAX_RANGOS];
    struct st_rango_binario asMisRangosPrivilegiados[N_MAX_PRIV];
    int iNumRangosDirectosOpe;
    int iNumRangosDirectosPriv;
    struct st_rango_binario asRangosDirectosOpe[N_MAX_RANGOS];
    struct st_rango_binario asRangosDirectosPriv[N_MAX_PRIV];
    void TomaLado( int );
    void PorDefecto();
    bool operator ==( const cfgConfigIfR2N5& );
    bool operator !=( const cfgConfigIfR2N5& );
};


/*
 * Estructura: cfgConfigIfTlf.
 * Descripcion: Configuracion del interfaz de telefonia de un recurso.
 * Campos->
 *
 *     iPatronRing = Patron de la seonl de ring (ver driver MTIFM).
 *     iPeriodoRing = Periodo de la seonl de ring generada en ms.
 *     iUmbralAltoPeriodoRing = Umbral alto de deteccion de periodo de ring. En ms.
 *     iUmbralBajoPeriodoRing = Umbral bajo de deteccion de periodo de ring. En ms.
 *     iVRing = Tension de la seonl de ring generada. En mv.
 *     iVBat = Tension continua de bateron generada. En mv.
 *     iTypeEyM = Typo de if EyM (Type I,II,II,IV y V); (si la linea es eym
 *     iModo = modo de if EyM (2w,4w);
 *     iLadoEyM = lado (EyM o PLR);
 */
 
struct cfgConfigIfTlf
{
    int iPatronRing;
    int iPeriodoRing;
    int iUmbralAltoPeriodoRing;
    int iUmbralBajoPeriodoRing;
    int iVRing;
    int iVBat;
    enum eType eTypeEyM;
    int iModoEyM;
    int iLadoEyM;

    int iDetectVox;
    int iUmbralVox;
    int iTmInactividad;
    int iTmLlamEntrante;			//tmout de llamada entrante en lineas tlf (BL,BC,RTB)
    int iTmDetFinLlamada;			//tmout para detectar fin llamada desde el ultimo ringoff

    int TReleaseBL;
    int iDetDtmf;
    int iDetInversionPol;
    int iDetCallerId;
    int iTmCallerId;

    int iPeriodoSpvRing;
    int iFiltroSpvRing;

    char szIdRed[CFG_MAX_LONG_NOMBRE_RED+1];
    void PorDefecto();
    bool operator ==( const cfgConfigIfTlf& );
    bool operator !=( const cfgConfigIfTlf& );
};



/*
 * Estructura: cfgConfigIfRdsi.
 * Descripcion: Configuracion del interfaz RDSI/ATS-QSIG de un recurso.
 * Campos->
 *    iLado: CFG_LADO_A o CFG_LADO_B.
 *    iModo: CFG_MODO_USER o CFG_MODO_RED.  
 *
 */
 
struct cfgConfigIfRdsi
{
    int iLado;
    int iModo;

//    char szIdTroncal[CFG_MAX_LONG_NOMBRE_TRONCAL+1];
//    char szIdRed[CFG_MAX_LONG_NOMBRE_RED+1];

    void PorDefecto();
    bool operator ==( const cfgConfigIfRdsi& );
    bool operator !=( const cfgConfigIfRdsi& );
};


/*
 * Estructura: cfgConfigRecurso.
 * Descripcion: Configuracion _completita_ de un recurso.
 * Campos->
 *    sGeneral: Configuracion general de recurso.
 *    uRec: Configuracion del recurso. Varia si es de audio o de datos.
 *    uIf: Configuracion del interfaz. Varia si es radio, LCEN, Rs/N5, telefonia, etc...
 */
 
struct cfgConfigRecurso
{
    struct cfgConfigGeneralRecurso sGeneral;
    union Rec
    {
        struct cfgConfigRecAudio sAudio;
        struct cfgConfigRecDatos sDatos;
    } uRec;        
    union uIf
    {
        struct cfgConfigIfRadio sRadio;
        struct cfgConfigIfLcen sLcen;
        struct cfgConfigIfR2N5 sR2N5;
        struct cfgConfigIfTlf sTlf;
        struct cfgConfigIfRdsi sRdsi;
    } uIf;
    
    cfgConfigRecurso();
    void PorDefecto();
    void TomaTipoRec( int );
    void TomaTipoIf( int );
    bool operator ==( const cfgConfigRecurso& );
    bool operator !=( const cfgConfigRecurso& );
    bool CambiaColateral(cfgConfigRecurso *pcfgOtra );
    bool CambioCritico(  const cfgConfigRecurso &sLaOtra );
    void ReajustaAudio( int offsetAtenuacion );
};        

/*
 * Estructura: cfgConfigPasarela
 * Descripcion: Configuracion general de la pasarela.
 * Campos->
 *
 *     tMutex: Mutex para el acceso a esta estructura.
 *
 *     acIdSistema: Nombre del sistema al que pertenece esta pasarela.
 *     szNombre: Nombre de la pasarela.
 *     szDirSrvConfig: Direccion del servidor de configuracion.
 *
 *     iNivelConsola: 
 *     iPuertoConsola: Puerto de escucha para abrir consolas remotas.
 *     iNivelIncidencias: 
 *
 *     acIdConfig: Identificador de configuracion (suministrado por el servidor).
 *     acIdSistemaSoap: Nombre del sistema al que pertenece esta pasarela segun dijo
 *                  el servidor de CFG la ultima vez.
 *     szNombreSoap: Nombre de la pasarela segun dijo el servidor de CFG la ultima vez.
 *     iModoSincronizacion: Ninguno, PTP, NTP, propietario de Page.   
 *     char szDirMasterSincro: Direccion del servidor de sincronizacion.
 *     iSipPuertoLocal: Puerto de escucha para el SIP User Agent.
 *     iSipPeriodoSuperv: Perioo de supervision de las sesiones SIP.
 *
 *     iSnmpPuertoLocal:
 *     iSnmpPuertoRemoto:
 *     iSnmpPuertoRemotoTraps:
 *
 *     iNumRecursos: Total de recursos operativos en la pasarela.
 *
 *     acGrupoMulticast: Direccion multicast del grupo al que se suscribe la pasarela.
 *     uiPuertoMulticast: Puerto del grupo al que se suscribe la pasarela.                     
 *
 *     asLocRec: Tabla de ubicacion de los recursos dados de alta en la pasarela.
 *               A esta tabla se accede con indices que van de 0 iNumRecursos.
 *     sRecurso: Tabla de configuracion de cada recurso.
 *               A esta tabla se accede con indices que van de 0 MAX_RECURSOS_TIFX.
 */
 

struct cfgConfigPasarela
{
    pthread_mutex_t tMutex;

    char acIdSistema[CFG_MAX_LONG_ID+1];   //poner CFG_MAX_LONG_NOMBRE_PASARELA
    char szNombre[CFG_MAX_LONG_NOMBRE_PASARELA+1]; 
    char szDirCPU0[CFG_MAX_LONG_URL+1];
    char szDirCPU1[CFG_MAX_LONG_URL+1];
    char szDirVirtual[CFG_MAX_LONG_URL+1];
    char szDirSrvConfig[CFG_MAX_LONG_URL+1];

    int iCfgModoSistema;

    int iNivelConsola;
    int iPuertoConsola;
    int iNivelIncidencias;
    int iDual;

    char acIdConfig[CFG_MAX_LONG_ID_CONFIG+1];
    int iModoSincronizacion;
//    char szDirMasterSincro[CFG_MAX_LONG_DIRECCION_TRANSPORTE];
    int iSipPuertoLocal;
    int iSipPeriodoSuperv;
//    int iSnmpPuertoLocal;
    int iSnmpPuertoRemoto;
    int iRecPuertoRemoto;
//    int iSnmpPuertoRemotoTraps;
//    char szTraps[CFG_MAX_LONG_URL+1];

    int iSupervLanGW;
	int itmmaxSupervLanGW;

	int idelayIniciaVrrp;

    int iNumRecursos;

    char acTipoSlot[MAX_SLOTS_PASARELA];
    struct LocalizadorRecurso asLocRec[MAX_RECURSOS_TIFX];
    struct cfgConfigRecurso sRecurso[MAX_RECURSOS_TIFX];

    char szDirSipRegistrar[NUM_SIP_REGISTRAR][MAX_LONG_DIRIP];
    char szDirSipProxy[NUM_SIP_PROXY][MAX_LONG_DIRIP];
    char szDirSrvNtp[NUM_SRV_NTP][MAX_LONG_DIRIP];

    char acGrupoMulticast[CFG_MAX_LONG_URL+1];
    unsigned int uiPuertoMulticast;
    struct st_numeracionats plannumeracionats [N_MAX_CENTRALES];
    struct st_direccionamientoip plandireccionamientoip[N_MAX_TV+N_MAX_TIFX];
    struct st_listatroncales plantroncales [N_MAX_TRONCALES];
    struct st_listaredes planredes [N_MAX_REDES];
    struct st_asignacionusuario_tv planasignacionusuarios [N_MAX_TV];
    struct st_asignacionrecursos_gw planasignacionrecursos[N_MAX_REC_SISTEMA];
    struct st_direccionamiento_sip plandireccionamientosip [N_MAX_TV];
//#ifdef CFG_ENC
    struct st_direccionamientoproxy plandireccionamientoproxy[N_MAX_CENTRALES];
//#endif
    /*
     * todo meter estas funciones en cfgConfigPasarela
     *
    st_config_sistema();
    bool operator ==( const st_config_sistema& );
    bool operator !=( const st_config_sistema& );*/
    void DimeNumerosPrueba( char*, int, char*, char* );
    void DimeMisRangos( struct st_rango_binario*, int*, struct st_rango_binario*, int* );
    void DimeRangosDirectos( char *szIdRec,
    										struct st_rango_binario *psRangosDirectosOpe,
    										int *piNumRangosOperador,
    										struct st_rango_binario *psRangosDirectosPriv,
    										int *piNumRangosPrivilegiados );

    void TomaNombre( char* );
    void TomaModoSincro( int );
    void PorDefecto( int );
    void ExtraeCfgSistema( struct st_config_sistema *ptcfgsistema);
    void TomaOffsetAtenuacion( int offset);

    cfgConfigPasarela();
    void  operator= ( cfgConfigPasarela );
    int EsRecursoMio( char* );
};


#endif


//-----------------------------------------------------------------
//  Historico de revisiones
//-----------------------------------------------------------------

/*
    $Log: cfgpasa.hpp,v $
    Revision 1.9  2011/03/08 09:22:05  jmgarcia
    nueva funcion Esrecursomio

    Revision 1.8  2011/01/11 11:19:27  jmgarcia

       (chg) Se modifican los #defines sCFG_SQ_HW, sCFG_SQ_TONO, y sCFG_SQ_VAD.

    Revision 1.7  2010/08/27 12:37:45  jmgarcia

       (bug)  Arregla CFG_MODO_NET
       (new) Anade CFG_TOTAL_LADOS_ATS y CFG_TOTAL_MODOS_RDSI

    Revision 1.6  2010/06/29 08:29:30  jmgarcia

       (new) Parametros adicionales en recursos: destino, troncal, y red.

    Revision 1.5  2010/05/10 13:29:20  jmgarcia
    (new) Elimina parametros SNMP de los recursos.

    Revision 1.4  2010/04/13 16:19:28  jmgarcia
    CFG_PTT_LOCKOUT pasa a valer 1

    Revision 1.3  2010/04/12 07:55:20  jmgarcia
    ID config lleva el ID de sistema y de pasarela.

    Revision 1.2  2010/03/25 15:26:03  jmgarcia
    Incluye mutex y operador de asignacion

    Revision 1.1  2010/03/24 11:53:47  jmgarcia
    Version inicial

*/    
