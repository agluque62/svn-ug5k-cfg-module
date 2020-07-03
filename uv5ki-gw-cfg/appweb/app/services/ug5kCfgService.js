/** */
angular
    .module('Ug5kweb')
    .factory('CfgService', CfgService);

CfgService.$inject = ['dataservice', '$q', '$rootScope', 'transerv', 'authservice'];

/** */
function CfgService(dataservice, $q, $rootScope, transerv, authservice) {

    /** */
    var cfg = null;
    var last_ip_virtual = null;
    var lradios = [];
    var radios = [];
    var ltelef = [];
    var telefs = [];
    /** */
    var last_hw_slv = 0;
    var last_rd_rec = 0;
    var last_tf_rec = 0;

    /** */
    var global_opt = 0;

    /** */
    function Cfg2RadioTelef() {

        lradios = [];
        radios = [];
        ltelef = [];
        telefs = [];

        for (irec = 0; irec < cfg.recursos.length; irec++) {
            if (cfg.recursos[irec].Radio_o_Telefonia == 1) {
                lradios.push({
                    index: irec,
                    id: cfg.recursos[irec].IdRecurso,
                    hw: cfg.recursos[irec].SlotPasarela.toString() + '-' + cfg.recursos[irec].NumDispositivoSlot.toString()
                });
                radios.push(cfg.recursos[irec]);
            }
            else {
                ltelef.push({
                    index: irec, id: cfg.recursos[irec].IdRecurso,
                    hw: cfg.recursos[irec].SlotPasarela.toString() + '-' + cfg.recursos[irec].NumDispositivoSlot.toString()
                });
                telefs.push(cfg.recursos[irec]);
            }
        }
    }

    /** */
    function HwSlvSet(slv, data) {
        var modificado = false;
        for (ican = 0; ican < 4; ican++) {
            if (cfg.hardware.slv[slv].pos[ican].cfg != data.pos[ican].cfg) {
                RecSet(slv, ican, parseInt(data.pos[ican].cfg));
                modificado = true;
            }
        }
        cfg.hardware.slv[slv] = jQuery.extend(true, {}, data);
        // cfg.hardware.slv[slv] = data;

        if (modificado)
            Cfg2RadioTelef();
    }

    /** Añade, Modifica o Borra un Recurso */
    function RecSet(slv, can, tp) {
        for (irec = 0; irec < cfg.recursos.length; irec++) {
            if (cfg.recursos[irec].SlotPasarela == slv && cfg.recursos[irec].NumDispositivoSlot == can) {
                switch (tp) {
                    case 0: // Borra el Recurso...
                        console.log("Borrado Recurso: ", slv, ":", can);
                        cfg.recursos.splice(irec, 1);
                        break;
                    case 1:
                        console.log("Modificado Recurso: ", slv, ":", can, " a Radio");
                        cfg.recursos[irec].Radio_o_Telefonia = 1;
                        break;
                    case 2:
                        console.log("Modificado Recurso: ", slv, ":", can, " a Telefonia");
                        cfg.recursos[irec].Radio_o_Telefonia = 2;
                        break;
                }
                return;
            }
        }
        /** Añadir el Recurso */
        if (tp != 0) {
            console.log("Añadido Recurso: ", slv, ":", can, " Tipo: ", tp);
            cfg.recursos.push(defResource(slv, can, tp));
        }
    }

    /**
     * Compone la URI de un Recurso en funcion de la ipv de de la Pasarela
     * @param {any} rec
     * 20190207. Los Recursos LCEN(5), BL (0), BC(1) y AB(2) Son de Formato Libre
     */
    function RecUriGet(rec) {
        if (cfg != null && rec != undefined) {

            if (rec.Radio_o_Telefonia == 2 &&
                (rec.telefonia.tipo == 0 || rec.telefonia.tipo == 1 || rec.telefonia.tipo == 2 || rec.telefonia.tipo == 5)) {
                return rec.Uri_Local;
            }
            return "sip:" + rec.IdRecurso + "@" + cfg.general.ipv;
        }
		return "";
    }

    /**
     * Normaliza las URI de todos los recursos
     *
     * */
    function RecUriNorm() {
        for (i = 0; i < cfg.recursos.length; i++) {
            var rec = cfg.recursos[i];
            cfg.recursos[i].Uri_Local = RecUriGet(cfg.recursos[i]);
        }
    }

    /** */
    function defResource(slv, can, tp) {
        var idRec = tp == 1 ? "RD-" + slv + "-" + can : "TF-" + slv + "-" + can;
        var PrecisionAudio = tp == 1 ? 0 : 1;
        return {
            IdRecurso: idRec,
            Radio_o_Telefonia: tp == 1 ? 1 : 2,
            SlotPasarela: slv,
            NumDispositivoSlot: can,
            TamRTP: 20,
            Codec: 0,
            Uri_Local: "sip:"+idRec+"@"+cfg.general.ipv,                               // "sip:local@127.0.0.1",
            enableRegistro: 0,
            szClave: "",
            Buffer_jitter: {
                min: 0,
                max: 0
            },
            hardware: {
                AD_AGC: 0,
                AD_Gain: 10,
                DA_AGC: 0,
                DA_Gain: -10
            },
            radio: {
                tipo: 0,
                sq: 0,
                ptt: 0,
                bss: 0,
                modoConfPtt: 0,
                repSqBss: 1,
                desactivacionSq: 1,
                timeoutPtt: 200,
                metodoBss: 0,
                umbralVad: -33,
                numFlujosAudio: 1,
                tiempoPtt: 0,
                tmVentanaRx: 100,
                climaxDelay: 1,
                tmRetardoFijo: 100,
                bssRtp: 0,
                retrasoSqOff: 50,
                evtPTT: 0,
                tjbd: 20,
                tGRSid: 10,
                iEnableGI: 0,
                tabla_indices_calidad:
                    [2,3,7,10,12,15],
                iSesionPrio: 0,
                iPttPrio: 0,
                iPrecisionAudio: PrecisionAudio,
                colateral: {
                    name: "118.000",
                    tipoConmutacion: 0,
                    emplazamientos: [{
                        uriTxA: "",
                        uriTxB: "",
                        activoTx: 0,
                        uriRxA: "",
                        uriRxB: "",
                        activoRx: 0
                    }, {
                        uriTxA: "",
                        uriTxB: "",
                        activoTx: 0,
                        uriRxA: "",
                        uriRxB: "",
                        activoRx: 0
                    }, {
                        uriTxA: "",
                        uriTxB: "",
                        activoTx: 0,
                        uriRxA: "",
                        uriRxB: "",
                        activoRx: 0
                    }
                    ]
                },
                FrqTonoSQ: -1,
                UmbralTonoSQ: -1,
                FrqTonoPTT: -1,
                UmbralTonoPTT: -1,
                SupervPortadoraTx: -1,
                SupervModuladoraTx: -1,
                iModoCalculoClimax: 0
            },
            telefonia: {
                tipo: 0,
                lado: 1,
                t_eym: 0,
                h2h4: 0,
                ladoeym: 0,
                modo: 0,
                no_test_local: "",
                no_test_remoto: "",
                it_release: 5,
                detect_vox: 1,
                umbral_vox: -20,
                tm_inactividad: 2,
                colateral_scv: 0,
                iT_Int_Warning: 5,
                ats_rangos_dst: [
					{ inicial: "", final: "" },
					{ inicial: "", final: "" },
					{ inicial: "", final: "" },
					{ inicial: "", final: "" }
                ],
                ats_rangos_org: [
					{ inicial: "", final: "" },
					{ inicial: "", final: "" },
					{ inicial: "", final: "" },
					{ inicial: "", final: "" }
                ]
                , idRed: ""
                , idTroncal: ""
				, iTmLlamEntrante: 30
				, iTmDetFinLlamada: 6
				, TReleaseBL: 3
				, iDetCallerId: 1
				, iTmCallerId: 2500
				, iDetInversionPol: 1
				, iPeriodoSpvRing: 200
				, iFiltroSpvRing: 2
                , iDetDtmf: 0
            /** 20200703. Nuevos parámetros de líneas Telefónicas */
                , iDetLineaAB: 0
                , iEnableNoED137: 0
            /** 20200703. Nueva Estructura de Colateral Remoto */
                , r_automatica: 1
                , tm_superv_options: 5
                , uri_remota: ""
                , superv_options: 1
                , itiporespuesta: 0
                , additional_uri_remota: ""
                , additional_superv_options: 0
                , additional_itiporespuesta: 0
            },
            LlamadaAutomatica: 0,
            restriccion: 0,
            blanca: ["", "", "", "", "", "", "", ""],
            negra: ["", "", "", "", "", "", "", ""],
            iFlgUsarDiffServ: 0,
            szDestino: ""
        };
    }

    /** */
    function Init() {
        var promise = $q.defer();
        if (cfg == null) {
            dataservice.get_config().then(
                function (respuesta) {
                    console.log("res:", respuesta.data);
                    cfg = respuesta.data;
                    Cfg2RadioTelef();
                    RecUriNorm();
                    last_ip_virtual = cfg.general.ipv;
                    $rootScope.$broadcast('loadcfg', [1, 2, 3]);
                    promise.resolve();
                },
                function (error) {
                    console.log("Error:", error);
                }
                );
        }
        else {
            promise.resolve();
        }
        return promise.promise;
    }

    /** */
    function local_quitar_sip(uri) {
        if (uri.indexOf("sip:") === 0) {
            var newuri = uri.substring(4, uri.length);
            return newuri;
        }
        return uri;
    }
    /** */
    function local_poner_sip(uri) {
        if (uri.length === 0)
            return uri;
        if (uri.indexOf("sip:") != 0) {
            var newuri = "sip:" + uri;
            return newuri;
        }
        return uri;
    }

    /** */
    return {
        init: Init
        , restore: function () {
            cfg = null;
            return Init();
        }
        , test_ip_virtual: function () {
            return cfg == null ? true : cfg.general.ipv == last_ip_virtual ? true : false;
        }
        , save: function () {
            dataservice.set_config(cfg).then(
                function (respuesta) {
                    console.log("POST: ", respuesta.data);
                    cfg = null;
                    Init();
                },
                function (error) {
                    console.log("POST-ERROR: ", error);
                }
            );
        }
        ,cfg_get: function () {
            return cfg;
        }
        ,inicio_data_get: function () {
            return cfg==null ? null : cfg.general;
        }
        ,inicio_data_set: function (data) {
            cfg.general = data;
        }
        ,hw_data_get: function (slv) {
            last_hw_slv = slv;
            return jQuery.extend(true, {}, cfg.hardware.slv[slv]);
            // return cfg.hardware.slv[slv];
        }
        ,hw_data_set: function (slv, data) {
            last_hw_slv = slv;
            HwSlvSet(slv, data);
        }
        ,hw_last_get: function () {
            return last_hw_slv;
        }
        ,ser_data_get: function () {
            return cfg.servicios;
        }
        ,ser_data_set: function (data) {
            cfg.servicios = data;
        }
        ,lradios: function () {
            return lradios;
        }
        ,radio_data_get: function (index) {
            last_rd_rec = index;
            return radios[index];
        }
        ,radio_data_set: function (index, data) {
            last_rd_rec = index;
            radios[index] = data;
            lradios[index].id = data.IdRecurso;
            cfg.recursos[lradios[index].index] = data;
        }
        ,radio_last_get: function() {
            return last_rd_rec < radios.length ? last_rd_rec : 0;
        }
        ,ltelef: function () {
            return ltelef;
        }
        ,telef_data_get: function (index) {
            last_tf_rec = index;
            return telefs[index];
        }
        ,telef_data_set: function (index, data) {
            last_tf_rec = index;
            telefs[index] = data;
            ltelef[index].id = data.IdRecurso;
            cfg.recursos[ltelef[index].index] = data;
        }
        ,telef_last_get: function () {
            return last_tf_rec < telefs.length ? last_tf_rec : 0;
        }
        ,radio_index_get: function (tar, pos) {
            for (i = 0; i < radios.length; i++) {
                if (radios[i].SlotPasarela == tar && radios[i].NumDispositivoSlot == pos)
                    return i;
            }
            return 0;
        }
        ,telef_index_get: function (tar, pos) {
            for (i = 0; i < telefs.length; i++) {
                if (telefs[i].SlotPasarela == tar && telefs[i].NumDispositivoSlot == pos)
                    return i;
            }
            return 0;
        }
        ,rec_uri_get: function (rec) {
            return RecUriGet(rec);
        }
        , rec_uri_nor: RecUriNorm
        , aplicar_cambios: function () {
            //if (confirm(/*"¿Desea salvar los cambios efectuados?"*/transerv.translate('CFGS_MSG_00'))) {
            //    $("body").css("cursor", "progress");
            //    dataservice.set_config(cfg).then(
            //        function (respuesta) {
            //            cfg = null;
            //            Init();
            //            $("body").css("cursor", "default");
            //            if (confirm(/*"¿Desea Subir los cambios?"*/transerv.translate('CFGS_MSG_01'))) {
            //                dataservice.upload_config();
            //            }
            //        },
            //        function (error) {
            //            $("body").css("cursor", "default");
            //            console.log("POST-ERROR: ", error);
            //        }
            //    );
            //}
            AltfyConfirm(authservice, transerv.translate('CFGS_MSG_00'), function () {                
                $("body").css("cursor", "progress");
                dataservice.set_config(cfg).then(
                    function (respuesta) {
                        cfg = null;
                        Init();
                        $("body").css("cursor", "default");
                        AltfyConfirm(authservice, transerv.translate('CFGS_MSG_01'), function () {
                            dataservice.upload_config();
                        });
                    },
                    function (error) {
                        $("body").css("cursor", "default");
                        console.log("POST-ERROR: ", error);
                    }
                );
            });
        }
        , test_json: function (data) {
            return true;
        }
        , aplicar_cambios_carga: function () {
            $("body").css("cursor", "progress");
            cfg.idConf = "carga-local";
            dataservice.set_config(cfg).then(
                function (respuesta) {
                    cfg = null;
                    Init();
                    $("body").css("cursor", "default");
                    console.log("aplicar_cambios_carga: Ok");
                },
                function (error) {
                    $("body").css("cursor", "default");
                    console.log("aplicar_cambios_carga: Error: " + error);
                }
            );
        }
        , quitar_sip: function (uri) {            
            if (typeof uri === "string") {
                return local_quitar_sip(uri);
            }
            else {
                var ret = [];
                for (i = 0; i < uri.length; i++) {
                    ret[i] = local_quitar_sip(uri[i]);
                }
                return ret;
            }
        }
        , poner_sip: function (uri) {
            if (typeof uri === "string") {
                return local_poner_sip(uri);
            }
            else {
                var ret = [];
                for (i = 0; i < uri.length; i++) {
                    ret[i] = local_poner_sip(uri[i]);
                }
                return ret;
            }
        }
        /** */
        , opcion: function (opt) {
            if (opt != undefined)
                global_opt = opt;
            return global_opt;                
        }
        , ulises: function () {
            return cfg.ulises;
        }
        , dualidad: function () {
            return cfg.general.dualidad == 0 ? false : true;
        }
        , indice_carga: function (modo) {   // 0: Redan, 1: Ulises
            var ic = 0;
            var nr = 0;
            if (cfg != null) {
                $.each(cfg.recursos, function (index) {
                    var rec = cfg.recursos[index];
                    nr += 1;
                    if (rec.Radio_o_Telefonia == 1) {
                        switch (rec.radio.tipo) {
                            case 0: // Radio Local Simple o P/R
                            case 1:
                                //ic += (rec.radio.iPrecisionAudio == 0 ? 2 : 1);
                                ic += (force_rdaudio_normal == true ? 1 : 2);
                                break;;
                            case 2: // Radio Local FD Simple o P/R
                            case 3:
                                ic += 8;
                                break;
                            case 4: // Radio Remota Receptores
                            case 6:
                                ic += (force_rdaudio_normal == true ? 1 : 4);
                                break;
                            case 5: // Radio Remota Transmisor.
                                ic += (force_rdaudio_normal == true ? 1 : 2);
                                break;
                        }
                    }
                    else if (rec.Radio_o_Telefonia == 2) {
                        switch (rec.telefonia.tipo) {
                            case 0: // BL
                            case 1: // BC
                                ic += 1;
                                break;

                            case 2: // AB en ulises 4 en REDAN 1.
                                ic += (modo == 0 ? 1 : 4);
                                break;

                            case 3: // R2
                            case 4: // N5
                            case 5: // LCEN
                                ic += 2;
                                break;

                            default:
                                ic += 1;
                                break;
                        }
                    }
                });
            }
                
            return nr.toString() + "/" + ic.toString();
        }
    };
}