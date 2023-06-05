/** */
angular
    .module('Ug5kweb')
    .factory('MantService', MantService);

MantService.$inject = ['dataservice', 'transerv'];

/** */
function MantService(dataservice, transerv) {

    /** */
    var txtStdGeneral = [
        transerv.translate('MANS_MSG_NINI'),        // STDG: No Inicializado
        transerv.translate('MANS_MSG_OK'),          // STDG: Ok
        transerv.translate('MANS_MSG_FAIL'),        // STDG: Fallo
        transerv.translate('MANS_MSG_WARN'),        // STD: Aviso
        transerv.translate('MANS_MSG_UKNW')         // Desconocido.
    ];
    var txtStdCpuActiva = [
        transerv.translate('MANS_MSG_NINFO'),       // CPU ACTIVA: Sin Informacion
        transerv.translate('MANS_MSG_CPU0'),        // CPU ACTIVA: CPU-0
        transerv.translate('MANS_MSG_CPU1'),        // CPU ACTIVA: CPU-1
        transerv.translate('MANS_MSG_UKNW'),        // Desconocido.
        transerv.translate('MANS_MSG_UKNW')         // Desconocido.
    ];
    var txtStdFA = [
        transerv.translate('MANS_MSG_NPRE'),        // FA: No Presente
        transerv.translate('MANS_MSG_OK'),          // FA: Ok
        transerv.translate('MANS_MSG_ERROR'),       // FA: Error
        transerv.translate('MANS_MSG_UKNW'),        // Desconocido.
        transerv.translate('MANS_MSG_UKNW')         // Desconocido.
    ];
    var txtStdNtp = [
        transerv.translate('MANS_MSG_NCFG'),        // NTP: No Configurado
        transerv.translate('MANS_MSG_CONE'),        // NTP: Conectado
        transerv.translate('MANS_MSG_DCON'),        // NTP: Desconectado
        transerv.translate('MANS_MSG_UKNW'),        // Desconocido.
        transerv.translate('MANS_MSG_UKNW')         // Desconocido.
    ];
    var txtStdPR = [
        transerv.translate('MANS_MSG_NPRE'),        // CPU P/R: No Presente
        transerv.translate('MANS_MSG_MAIN'),        // CPU P/R: Principal
        transerv.translate('MANS_MSG_RSVA'),        // CPU P/R: Reserva
        transerv.translate('MANS_MSG_UKNW'),        // Desconocido.
        transerv.translate('MANS_MSG_UKNW')         // Desconocido.
    ];
    var txtStdLAN = [
        transerv.translate('MANS_MSG_NPRE'),        // LAN: No Presente
        transerv.translate('MANS_MSG_OK'),          // LAN: Ok
        transerv.translate('MANS_MSG_ERROR'),       // LAN: Error
        transerv.translate('MANS_MSG_UKNW'),        // Desconocido.
        transerv.translate('MANS_MSG_UKNW')         // Desconocido.
    ];
    var txtStdItf = [
        transerv.translate('MANS_MSG_NPRE'),        // STD-ITF: No Presente
        transerv.translate('MANS_MSG_OK'),          // STD-ITF: Ok
        transerv.translate('MANS_MSG_FAIL'),       	// STD-ITF: Fallo
        transerv.translate('MANS_MSG_DGRA'),        // STD-ITF: Degradado
        transerv.translate('MANS_MSG_UKNW')         // Desconocido.
    ];
    var txtStdBdt = [
        transerv.translate('MANS_MSG_UKNW'),        // BDT: Desconocido.
        transerv.translate('MANS_MSG_OK'),          // BDT: Ok
        transerv.translate('MANS_MSG_CONF'),        // BDT: Conflicto
        transerv.translate('MANS_MSG_UKNW'),        // Desconocido.
        transerv.translate('MANS_MSG_UKNW')         // Desconocido.
    ];
    /** */
    var txtEstados = [txtStdGeneral, txtStdCpuActiva, txtStdFA, txtStdNtp, txtStdPR, txtStdLAN, txtStdItf, txtStdBdt];

    var styEstados = [
        { 'color': 'black', 'background-color': 'lightgray' },
        { 'color': 'blue', 'background-color': 'white' },
        { 'color': 'red', 'background-color': 'white' },
        { 'color': 'black', 'background-color': 'tomato' },
        { 'color': 'white', 'background-color': 'tomato' },
        { 'color': 'black', 'background-color': 'white' }
    ];
    //var txtItfTipos = ["Radio", "LCE", "BC", "BL", "AB", "R2", "N5", "QSIG","Tunneling","No Configurado","Sin Informacion"];
//    var txtItfTipos = [
//        transerv.translate('MANS_ITF_RD'),
//        transerv.translate('MANS_ITF_LC'),
//        transerv.translate('MANS_ITF_BC'),
//        transerv.translate('MANS_ITF_BL'),
//        transerv.translate('MANS_ITF_AB'),
//        transerv.translate('MANS_ITF_R2'),
//        transerv.translate('MANS_ITF_N5'),
//        transerv.translate('MANS_ITF_QS'),
//        transerv.translate('MANS_ITF_TN'),
//        transerv.translate('MANS_ITF_NC'),
//        transerv.translate('MANS_ITF_NI')];

    //var txtItfTipos = ["Radio", "LCE", "BC", "BL", "AB", "R2", "N5", "QSIG", "RDSI_2BD", "RDSI_30BD", 
    //						"IO", "DATOS", "RRC", "EYM_PP", "EYM_MARC", "Tunnel-loc", "Tunnel-rem", "TUNN-2H","No Configurado","Sin Informacion"];
    var txtItfTipos = [
        transerv.translate('MANS_ITF_RD'),
        transerv.translate('MANS_ITF_LC'),
        transerv.translate('MANS_ITF_BC'),
        transerv.translate('MANS_ITF_BL'),
        transerv.translate('MANS_ITF_AB'),
        transerv.translate('MANS_ITF_R2'),
        transerv.translate('MANS_ITF_N5'),
        transerv.translate('MANS_ITF_QS'),
        transerv.translate('MANS_ITF_RDSI_2BD'),
        transerv.translate('MANS_ITF_NC'), 		/*transerv.translate('MANS_ITF_RDSI_30BD'), en ulises el recurso rdsi-30bd coincide con el campo no configurado (9)*/
        transerv.translate('MANS_ITF_NI'), 		/*transerv.translate('MANS_ITF_IO'), en ulises el recurso io coincide con el campo no informacion (10) */
        transerv.translate('MANS_ITF_DATOS'),
        transerv.translate('MANS_ITF_RRC'),
        transerv.translate('MANS_ITF_EYMPP'),
        transerv.translate('MANS_ITF_EYMMARC'),
        transerv.translate('MANS_ITF_TNL'),
        transerv.translate('MANS_ITF_TNR'),
        transerv.translate('MANS_ITF_TN2H'),
        transerv.translate('MANS_ITF_NC'),
        transerv.translate('MANS_ITF_NI')];

    //var txtModos = ["Normal", "Mantenimiento"];
    var txtModos = [
         transerv.translate('MANS_MOD_NORMAL'),
         transerv.translate('MANS_MOD_MANT')];

    /** */
    var std = defMantStd();
    var global_modo = "rd";
    var global_std = -1;
    var ntpsync = 0;
    var modo_redan = "0";

    /** */
    function defMantStd() {
        return {
            stdg: 0,                // Estado Global.               0: No Inicializado, 1: OK,              2: Fallo,          3: Aviso.    4: Incongruente.
            cpua: 0,                // CPU Activa.                  0: No Info,         1: CPU1,            2: CPU2,           3:           4: Incongruente.
            stdf: 0,                // Estado Fuente Alimentacion.  0: No Presente,     1: Ok,              2: Fallo.          3:           4: Incongruente.
            stdn: 0,                // Estado NTP.                  0: No Configurado.  1: OK.              2: No Conectado.              
            cpu0: {
                stdg: 0,            // Estado General CPU.          0: No Presente,     1: Principal,       2: Reserva
                stl1: 0,            // Estado LAN-1.                0: No Presente,     1: OK,              2:Error
                stl2: 0            // Estado LAN-2.                 0: No Presente,     1: OK,              2:Error
            },
            cpu1: {
                stdg: 0,            // Estado General CPU.          0: No Presente,     1: Principal,       2: Reserva
                stl1: 7,            // Estado LAN-1.                0: No Presente,     1: OK,              2:Error
                stl2: 7            // Estado LAN-2.                 0: No Presente,     1: OK,              2:Error
            },
            itf: [
                { id: "IFT-00", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-01", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-02", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-03", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-10", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-11", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-12", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-13", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-20", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-21", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-22", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-23", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-30", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-31", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-32", tp: 10, st: 0, md: 0, bc: 0},
                { id: "IFT-33", tp: 10, st: 0, md: 0, bc: 0}
            ]
        };
    }

    /** */
    function getEstado() {
        return dataservice.mnt_std().then(
            function (resp) {
                if (resp.hasOwnProperty('error')) {
                    std = defMantStd();
                }
                else {
                    std = resp.data;
                    normalizeEstadoItf();
                }
            });
    }

    /** */
    function normalizeEstadoItf() {
        std.itf.forEach(function (itf) {
            if (itf.tp < 0 || itf.tp >= 19) {
                itf.id = "";
                itf.st = 0;
                itf.md = -1;
                itf.bc = -1;
            }
        });
    }

    /** */
    return {
        init: getEstado
        , dispose: function () { }
        , get_estado: getEstado
        , estado: function () {
            return std;
        }
        ,
        textoEstado: function (std, indice) {
            if (std >= 0 && std < 5 && indice >= 0 && indice < 8)
                return transerv.translate(txtEstados[indice][std]);
            return transerv.translate('MANS_MSG_UKNW');     // "Desconocido"
        }
        , styleEstado: function (std) {
            return (std >= 0 && std < 5) ? styEstados[std] : { 'color': 'red', ' background-color': 'gray' };

        }
        , styleOther: function (cpu) {
            return cpu == 0 ? { 'color': 'black', 'background-color': 'lightgray' } :
                cpu == 1 ? { 'color': 'blue', 'background-color': 'white' } :
                cpu == 2 ? { 'color': 'blue', 'background-color': 'white' } : { 'color': 'white', 'background-color': 'tomato' };
        }
        , textoTipoItf: function (tp) {
            return (tp >= 0 && tp < 20) ? transerv.translate(txtItfTipos[tp]) : /*"Desconocido"*/transerv.translate('MANS_MSG_UKNW');
        }
        , textoModoItf: function (tp) {
            return (tp >= 0 && tp < 2) ? transerv.translate(txtModos[tp]) : /*"Sin Informacion"*/transerv.translate('MANS_MSG_NINFO');
        }
        , textoBucleItf: function (tp) {
            if (tp == 0)
                return "Off";
            else if (tp == 1)
                return "RFS";
            else if (tp >= 20 && tp <= 23) {
                var emp = tp - 20;
                return "RFE" + (emp+1);
            }
            return /*"Sin Informacion"*/"--";
        }
        , get_slot_itf: function (index) {
            var slot = Math.floor(index/4).toString();
            var itf = (index % 4).toString();
            return slot + ":" + itf;
        }
        /** */
        , modo: function (md) {
            if (md != undefined)
                global_modo = md;
            return global_modo;
        }
        /** */
        , modo_redan: function (md) {
            if (md != undefined)
                modo_redan = md;
            return modo_redan;
        }
        /** */
        , hide_on_ulises: function () {
            return !(global_modo == "ul");
        }
        , global_estado: function (std) {
            if (std != undefined) {
                global_std = std;
            }
            return global_std;
        }
        , ntpsync: function (ntp) {
            if (ntp != undefined)
                ntpsync = ntp;
            return ntpsync;
        }
        , is_redan: function () {
            return global_modo == "rd";
        }
        , is_ulises: function () {
            return global_modo == "ul";
        }
        
    };
}