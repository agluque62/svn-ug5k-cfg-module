/* */
angular
    .module('Ug5kweb')
    .controller('ug5kRecrCtrl', ug5kRecrCtrl);

ug5kRecrCtrl.$inject = ['$scope', '$routeParams', '$route', 'authservice', 'CfgService', 'ValidateService', 'transerv', 'MantService'];

function ug5kRecrCtrl($scope, $routeParams, $route, authservice, CfgService, ValidateService, transerv, MantService) {
    var vm = this;
    var simul = true;

    /* Modelo */
    vm.radiosel = "0";
    vm.current = -1;
    vm.lradios = [];
    vm.rdata = {};
    vm.pagina = 0;
    vm.vdata = [];
    vm.autosave = v2jdata;
    vm.dval = function () { return ""; };
    vm.hide = function () { return false; };
    vm.lcol = 0;

    /* */
    var ar_on_ulises = true;                // Habilita o no el Agente Radio en ULISES

    CfgService.opcion(2);

    /* Validador de IdRecurso. Actualiza a la ver la URI Local */
    vm.id_val = function (value) {
        var error = ValidateService.max_long_val(value);
        if ( error == "") {
            vm.rdata.IdRecurso = value;
            vm.vdata[4].Value = quitar_sip(CfgService.rec_uri_get(vm.rdata));   // jamp_no_sip == 1 ? CfgService.quitar_sip(CfgService.rec_uri_get(vm.rdata)) : CfgService.rec_uri_get(vm.rdata);
            return "";
        }
        return error;
    };

    /**
     * 
     * @param {any} uri
     */
    vm.uri_val = function (uri) {
        if (uri == "")
            return "";
        return ValidateService.uri_val(uri);
    };

    /* Validador cbm en A/D */
    vm.cbmad_val = function (value) {
        if (value.toString() == "")
            return transerv.translate("El valor debe estar entre: ") + rr_ad_rng.toString();
        return value >= rr_ad_rng.min && value <= rr_ad_rng.max ? "" : transerv.translate("El valor debe estar entre: ") + rr_ad_rng.toString();
    };

    /* Validador cmd en D/A */
    vm.cbmda_val = function (value) {
        if (value.toString() == "")
            return transerv.translate("El valor debe estar entre: ") + rr_da_rng.toString();
        return value >= rr_da_rng.min && value <= rr_da_rng.max ? "" : transerv.translate("El valor debe estar entre: ") + rr_da_rng.toString();
    };

    /* Validador nivel VAD */
    vm.vad_val = function (value) {
        return value >= rr_vad_rng.min && value <= rr_vad_rng.max ? "" : transerv.translate("El valor debe estar entre: ") + rr_vad_rng.toString();
    };

    /* Validador ventana BSS */
    vm.bssv_val = function (value) {
        return value >= rr_bssw_rng.min && value <= rr_bssw_rng.max ? "" : transerv.translate("El valor debe estar entre: ") + rr_bssw_rng.toString();
    };

    /* Validador tiempo fijo CLIMAX */
    vm.climaxt_val = function (value) {
        return value >= rr_clxt_rng.min && value <= rr_clxt_rng.max ? "" : transerv.translate("El valor debe estar entre: ") + rr_clxt_rng.toString();
    };

    /* */
    vm.fid_val = function (value) {
        if (MantService.modo() == "ul")
            return "";
        return value == "" ? "" : ValidateService.fid_val(value);
        //if (value != "" && value.match(regx_fid) == null)
        //    return transerv.translate("Formato de Frecuencia incorrecto");
        //return "";
    };

    /* */
    vm.update_radio = function () {
        if (vm.current != -1)
            set_radio_data(vm.current);
        get_radio_data(parseInt(vm.radiosel));

        vm.set_pagina(0);
    };

    /*  ******************** */
    vm.radgain_show = function (ind) {
        ////if (vm.pagina == 1)
        //    return true;
        switch (ind) {
            case 2:
                return (parseInt(vm.vdata[1].Value) == 0);
            case 4:
                return (parseInt(vm.vdata[3].Value) == 0);
        }
        return true;
    };

    /* */
    vm.set_pagina = function (pagina) {
        if (!authservice.global_validate(validate_page())) {
            /*alert*/alertify.error("01 " + transerv.translate('ICTRL_MSG_01'));   // "Existen Errores de Formato o Rango. No puede cambiar de vista...");
        }
        else if (authservice.check_session() == true) {
            v2jdata();
            vm.pagina = pagina;
            j2vdata();
        }
    };

    /* */
    vm.rad_show = function (ind) {
        switch (ind) {
            case 2:	// Umbral VAD. Solo si SQH==1.
                return (parseInt(vm.vdata[1].Value) == 1);
            case 4: // Tiempo PTT
                return false;
            case 6:	// Mostrar Control BSS. Solo Radio Locales en FD.				
                return (parseInt(vm.vdata[0].Value) == 2 || parseInt(vm.vdata[0].Value) == 3);
            case 7:
            case 8: // Parametros BSS. Solo Radio Locales en FD y BSS Activo.
            case 9:
                return ((parseInt(vm.vdata[0].Value) == 2 || parseInt(vm.vdata[0].Value) == 3) && parseInt(vm.vdata[6].Value) == 1);
            case 10: // Tiempo de Compensacion CLIMAX. Solo Radio Locales en FD, BSS Activo y Metodo Tiempo FIJO.
                return (((parseInt(vm.vdata[0].Value) == 2 || parseInt(vm.vdata[0].Value) == 3) && parseInt(vm.vdata[6].Value) == 1) && parseInt(vm.vdata[9].Value) == 2);
            case 11: // Solo Radio Remoto.
                return (parseInt(vm.vdata[0].Value) > 3);

            case 12: // Prioridad PTT / Sesion SIP. Solo Radios Locales.
            case 13:
                return (parseInt(vm.vdata[0].Value) <= 3);
        }
        return true;
    };

    /* */
    vm.p0_rad_show = function (ind) {
        switch (ind) {
            case 0:     // ID Recurso
                return true;
            case 1:     // SLOT / Posicion
                return true;
            case 2:     // Frecuencia
                return MantService.hide_on_ulises();
            case 3:     // Tipo de Agente
                if (ar_on_ulises == false)
                    return MantService.hide_on_ulises();
                return true;
            case 4:     // URI.
                return true;
            case 5:     // Enable Registro.
                return MantService.hide_on_ulises();
            case 6:     // Clave
                return vm.show_clave();
            case 7:     // Eventos PTT/SQH
                return true;
            case 8:     // Grabacion.
                return vm.grab_show();
            case 9:     // Forzar PTT
                return false;
        }
        return false;
    };

    /* */
    vm.p1_rad_show = function (ind) {
        switch (ind) {
            case 0:         // CODEC
                return true;
            case 1:         // AGC A/D
                return true;
            case 2:         // Val G-A/D
                return vm.radgain_show();
            case 3:         // AGC D/A
                return true;
            case 4:         // Val G-D/A
                return vm.radgain_show();
            case 5:         // Jitter Buffer
                return false;
            case 6:         // Precision Audio.
                return !force_rdaudio_normal;           // true; 20200508. Opcion de 16 canales radio sin audio stricto
        }
        return false;
    };

    /* */
    vm.p2_rad_show = function (ind) {
        switch (ind) {
            case 0:         // Tipo Agente
                if (ar_on_ulises == false)
                    return MantService.hide_on_ulises();
                return true;

            case 1:         // Opciones de SQH. En todos menos en los TX-Remotos (vm.vdata[0].Value == 5)
                return parseInt(vm.vdata[0].Value) != 5;

            case 2:         // Umbral VAD
                return (parseInt(vm.vdata[1].Value) == 1 && parseInt(vm.vdata[0].Value) != 5);

            case 3:         // Indicacion Salida Audio. En todos menos en los Rx-Remotos (vm.vdata[0].Value == 6)
                return parseInt(vm.vdata[0].Value) != 6;

            case 4:         // Tiempo PTT Maximo
                return false;

            case 5:         // Metodo BSS. En todos menos en los TX-Remotos (vm.vdata[0].Value == 5)
                return parseInt(vm.vdata[0].Value) != 5;

            case 6:         // BSS / CLIMAX ?
                return (parseInt(vm.vdata[0].Value) == 2 || parseInt(vm.vdata[0].Value) == 3);

            case 8:         // BSS. Cola SQUELCH
                return false;
            case 7:         // Ventana BSS
            case 9:         // CLIMAX-DELAY
                return ((parseInt(vm.vdata[0].Value) == 2 || parseInt(vm.vdata[0].Value) == 3) && parseInt(vm.vdata[6].Value) == 1);

            case 10:        // Tiempo...
                return (((parseInt(vm.vdata[0].Value) == 2 || parseInt(vm.vdata[0].Value) == 3) && parseInt(vm.vdata[6].Value) == 1) && parseInt(vm.vdata[9].Value) == 2);

            case 11:        // Modo Calculo CLIMAX
                return ((parseInt(vm.vdata[0].Value) == 2 || parseInt(vm.vdata[0].Value) == 3) && parseInt(vm.vdata[6].Value) == 1 && parseInt(vm.vdata[9].Value) != 0);

            case 12:        // GRS Internal DELAY
                return (parseInt(vm.vdata[0].Value) == 4 || parseInt(vm.vdata[0].Value) == 5);

            case 13:        // Prioridad PTT.
            case 14:        // Prioridad Session.
                return (parseInt(vm.vdata[0].Value) <= 3);

            case 15:        // Tabla de Calificacion Radio. Solo en Radios Remotos.
                //if (MantService.hide_on_ulises() == false)
                //    return false;
                return (parseInt(vm.vdata[0].Value) == 4 || parseInt(vm.vdata[0].Value) == 6);
        }
        return true;
    };

    /* */
    vm.p3_rad_show = function (ind) {
        switch (ind) {
            case 0:         // Tipo Agente Radio.
                if (ar_on_ulises == false)
                    return MantService.hide_on_ulises();
                break;
            case 1:         // Frecuencia
                return MantService.hide_on_ulises();
        }
        return vm.col_show(ind);
    };

    /* */
    vm.p4_rad_show = function (ind) {
        switch (ind) {
            default:
                break;
        }
        return true;
    };

    /* */
    vm.grab_show = function (ind) {
        if (parseInt(vm.vdata[3].Value) == 4 || parseInt(vm.vdata[3].Value) == 6)
            return true;
        return false;
    };

    /* */
    vm.col_show = function (ind) {
        switch (ind) {
            case 0:     // Tipo.
            case 1:     // Frecuencia.
                return true;

            case 2:     // Literal Emplazamiento 1
                return (vm.vdata[0].Value == 2 || vm.vdata[0].Value == 3);

            case 3:     // Emp 1. TXA
            case 5:     // Emp 1. RXA
                return (vm.vdata[0].Value == 0 || vm.vdata[0].Value == 1 || vm.vdata[0].Value == 2 || vm.vdata[0].Value == 3);

            case 4:	    // Emp 1. TxB   // Solo en Configuracion P/R
            case 6:     // Emp 1. RXB
                return (vm.vdata[0].Value == 1 || vm.vdata[0].Value == 3);

            case 7:     // Literal EMP-2
            case 8:     // Emp 2. TXA.
            case 10:    // Emp 2. RXA.
                return (vm.vdata[0].Value == 2 || vm.vdata[0].Value == 3);

            case 9:     // Emp 2. TXB. 
            case 11:    // Emp 2. RXB
                return (vm.vdata[0].Value == 3);

            case 12:    // Literal EMP-3
            case 13:    // Emp 3. TXA
            case 15:    // Emp 3. RXA
                return (vm.vdata[0].Value == 2 || vm.vdata[0].Value == 3);
            case 14:    // Emp 3. TXB
            case 16:    // Emp 3. RXB
                return (vm.vdata[0].Value == 3);
            default:
                return true;
        }
    };

    /* */
    vm.lbn_show = function (ind) {
        switch (ind) {
            case 1:
                return (vm.vdata[0].Value == 2);
            case 2:
                return (vm.vdata[0].Value == 1);
            default:
                return true;
        }
    };

    /* */
    vm.show_forceptt = function () {
        var radiotipo = vm.pagina == 0 ? vm.vdata[3].Value : vm.rdata.radio.tipo;
        if (radiotipo == 0 || radiotipo == 4 || radiotipo == 6)
            return true;
        return false;
    };

    /* */
    vm.show_clave = function () {
        return vm.vdata[5].Value == 1 ? true : false;
    };

    /* */
    vm.restore = function () {
        CfgService.restore().then(function () {
            get_radios();
            get_radio_data(parseInt(vm.radiosel));
        });
    };

    /* Mostrar Pestaña Colaterales */
    vm.pcol_show = function () {
        if (vm.rdata != undefined && vm.vdata.length > 0) {
            switch (vm.pagina) {
                case 0:
                    return vm.vdata[3].Value < 4 ? true : false;
                case 1:
                case 2:
                case 3:
                case 4:
                    return vm.rdata.radio.tipo < 4 ? true : false;
            }
        }
        return true;
    };

    /* Mostrar Pestañas Listas Blancas y Negras */
    vm.pbn_show = function () {
        if (MantService.hide_on_ulises() == false)
            return false;
        if (vm.rdata != undefined && vm.vdata.length > 0) {
            switch (vm.pagina) {
                case 0:
                    return vm.vdata[3].Value >= 4 ? true : false;
                case 1:
                case 2:
                case 3:
                    return vm.rdata.radio.tipo >= 4 ? true : false;
            }
        }
        return true;
    };

    /* */
    vm.sqh_options = function () {
        if (vm.show_forceptt() == false) {
            return {
                Name: /*'Indicacion Entrada Audio:'*/transerv.translate('RCTRL_P02_IENA'),
                Value: vm.rdata.radio.sq < 2 ? vm.rdata.radio.sq.toString() : "0",
                Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                Input: 1,
                Inputs: [/*"Hardware"*/transerv.translate('RCTRL_P02_HARD'), /*"VAD"*/transerv.translate('RCTRL_P02_VAD')],
                Show: vm.p2_rad_show,
                Val: vm.dval
            };
        }
        else {
            return {
                Name: /*'Indicacion Entrada Audio:'*/transerv.translate('RCTRL_P02_IENA'),
                Value: vm.rdata.radio.sq.toString(),
                Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                Input: 1,
                Inputs: [/*"Hardware"*/transerv.translate('RCTRL_P02_HARD'), /*"VAD"*/transerv.translate('RCTRL_P02_VAD'), /*"Forzado"*/transerv.translate('RCTRL_P02_FOR')],
                Show: vm.p2_rad_show,
                Val: vm.dval
            };
        }
    };

    /* */
    vm.tbQidx = function (radio) {
        var str_qidx = "";

        //var index = 0;
        //radio.tabla_indices_calidad.forEach(function (val) {
        //    if (index < 6)
        //        str_qidx += ((index * 10).toString() + ": " + val.toString() + ", ");
        //    index++;
        //});

        /** 20210609 RM4849. Cuando no hay tabla de calidad, los valores deben estar a '0' en vez de a 15, y se debe presentar exista o no tabla */
        for (index = 0; index < 6; index++) {
            var val = 0;
            if (index < radio.tabla_indices_calidad.length) {
                val = radio.tabla_indices_calidad[index];
            } 
           str_qidx += ((index * 10).toString() + ": " + val.toString() + ", ");
        }
        return str_qidx;
    };


    /* */
    function quitar_sip(uri) {
        if (jamp_no_sip == 1)
            return CfgService.quitar_sip(uri);
        return uri;
    }

    /* */
    function poner_sip(uri) {
        if (jamp_no_sip == 1)
            return CfgService.poner_sip(uri);
        return uri;
    }

    /* */
    function get_radios() {
        vm.lradios = CfgService.lradios();
        //vm.lradios.sort(function (a, b) {
        //    var x = a.hw.toLowerCase();
        //    var y = b.hw.toLowerCase();
        //    if (x < y) { return -1; }
        //    if (x > y) { return 1; }
        //    return 0;
        //});
    }

    /* */
    function get_radio_data(radio) {
        vm.radiosel = radio.toString();
        vm.current = radio;
        vm.rdata = CfgService.radio_data_get(radio);
        if (vm.rdata != undefined) {
            vm.lcol = vm.rdata.radio.colateral.emplazamientos.length;
            if (vm.lcol != 3)
                /*alert*/alertify.error(transerv.translate('RCTRL_MSG_01')/*"El recurso " */ + vm.rdata.IdRecurso + transerv.translate('RCTRL_MSG_02')/*" viene con "*/ +
                    vm.lcol + transerv.translate('RCTRL_MSG_03')/*" emplazamientos!.\nDebería venir con 3..."*/);
            j2vdata();
            /* Campos nuevos */
            /* 20160830 */
            //if (typeof vm.rdata.radio.iPrecisionAudio == 'undefined' ||
            //    vm.rdata.radio.iPrecisionAudio == undefined) {
            //        vm.rdata.radio.iPrecisionAudio = 1;
            //}
        }
    }

    /* */
    function set_radio_data(radio) {
        if (vm.rdata != undefined) {
            v2jdata();
            CfgService.radio_data_set(radio, vm.rdata);
        }
    }


    /* */
    function j2vdata() {
        if (vm.rdata == undefined)
            return;
        switch (vm.pagina) {
            case 0:
                vm.vdata = [
                    {
                        Name: /*'Recurso:'*/transerv.translate('RCTRL_P00_REC'),
                        Value: vm.rdata.IdRecurso,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.p0_rad_show,
                        Val: vm.id_val
                    },
                    {
                        Name: /*'SLOT/Posicion:'*/transerv.translate('RCTRL_P00_ST'),
                        Value: vm.rdata.SlotPasarela + " / " + vm.rdata.NumDispositivoSlot,
                        Enable: false,
                        Input: 0,
                        Inputs: [],
                        Show: vm.p0_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Frecuencia:'*/transerv.translate('RCTRL_P00_FRC'),
                        Value: vm.rdata.radio.colateral.name,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.p0_rad_show,
                        Val: vm.fid_val
                    },
                    {
                        Name:/*'Tipo de Agente Radio:'*/transerv.translate('RCTRL_P00_TPA'),
                        Value: vm.rdata.radio.tipo.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [
                             /*"Local-Simple"*/transerv.translate('RCTRL_P00_ALS'),
                             /*"Local-P/R"*/transerv.translate('RCTRL_P00_ALP'),
                             /*"Local FD-Simple"*/transerv.translate('RCTRL_P00_ALD'),
                             /*"Local FD-P/R"*/transerv.translate('RCTRL_P00_ALF'),
                             /*"Remoto RxTx"*/transerv.translate('RCTRL_P00_RTR'),
                             /*"Remoto Tx"*/transerv.translate('RCTRL_P00_RTX'),
                             /*"Remoto Rx"]*/transerv.translate('RCTRL_P00_RRX')],
                        Show: vm.p0_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'URI:'*/transerv.translate('RCTRL_P00_URI'),
                        Value: quitar_sip(vm.rdata.Uri_Local),
                        Enable: false,
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p0_rad_show,
                        Val: ValidateService.uri_val
                    },
                    {
                        Name: /*'Enable Registro ?:'*/transerv.translate('RCTRL_P00_ERG'),
                        Value: vm.rdata.enableRegistro.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('RCTRL_P00_NO'), /*"Si"*/transerv.translate('RCTRL_P00_SI')],
                        Show: vm.p0_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Clave:'*/transerv.translate('RCTRL_P00_CLV'),
                        Value: vm.rdata.szClave,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.p0_rad_show,
                        Val: ValidateService.max_long_val
                    },
                    {
                        Name: /*'Eventos PTT-SQH ?:'*/transerv.translate('RCTRL_P00_ETQ'),
                        Value: vm.rdata.radio.evtPTT.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('RCTRL_P00_NO'), /*"Si"*/transerv.translate('RCTRL_P00_SI')],
                        Show: vm.p0_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Grabacion ?:'*/transerv.translate('RCTRL_P00_GRB'),
                        Value: vm.rdata.radio.iEnableGI.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('RCTRL_P00_NO'), /*"Si"*/transerv.translate('RCTRL_P00_SI')],
                        Show: vm.p0_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Forzar PTT:'*/transerv.translate('TCTRL_P00_FPS'),
                        Value: 1/* TODO. vm.tdata.szClave*/,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: vm.p0_rad_show,
                        Val: vm.dval
                    }

                ];
                break;
            case 1:
                vm.vdata = [
                    {
                        Name: /*'CODEC :'*/transerv.translate('RCTRL_P01_COD'),
                        Value: vm.rdata.Codec.toString(),
                        Enable: false,
                        Input: 1,
                        Inputs: ["G711-A", "G711-U", "G729"],
                        Show: vm.p1_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'AGC en A/D ?:'*/transerv.translate('RCTRL_P01_AGCAD'),
                        Value: vm.rdata.hardware.AD_AGC.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('RCTRL_P00_NO'), /*"Si"*/transerv.translate('RCTRL_P00_SI')],
                        Show: vm.p1_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Ajuste Cero Digital en A/D (dBm):'*/transerv.translate('RCTRL_P01_AGCAD_AJUSTE'),
                        Value: vm.rdata.hardware.AD_Gain / 10,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.radgain_show,
                        Val: vm.cbmad_val
                    },
                    {
                        Name: /*'AGC en D/A ?:'*/transerv.translate('RCTRL_P01_AGCDA'),
                        Value: vm.rdata.hardware.DA_AGC.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('RCTRL_P00_NO'), /*"Si"*/transerv.translate('RCTRL_P00_SI')],
                        Show: vm.p1_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Ajuste Cero Digital en D/A (dBm):'*/transerv.translate('RCTRL_P01_AGCDA_AJUSTE'),
                        Value: vm.rdata.hardware.DA_Gain / 10,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.radgain_show,
                        Val: vm.cbmda_val
                    },
                    {
                        Name: /*'Jitter Buffer Delay (ms):'*/transerv.translate('RCTRL_P01_JIT'),
                        Value: vm.rdata.radio.tjbd,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.p1_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Precision Audio:'*/transerv.translate('RCTRL_P01_AUDP'),
                        Value: vm.rdata.radio.iPrecisionAudio.toString(),
                        Enable: false /*authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE])*/,
                        Input: 1,
                        Inputs: [/*"Estricto"*/transerv.translate('RCTRL_P01_AUDPS'), /*"Normal"*/transerv.translate('RCTRL_P01_AUDPN')],
                        Show: vm.p1_rad_show,
                        Val: vm.dval
                    }
                ];
                break;
            case 2:
                vm.vdata = [
                    { // 0
                        Name:/*'Tipo de Agente Radio:'*/transerv.translate('RCTRL_P00_TPA'),
                        Value: vm.rdata.radio.tipo.toString(),
                        Enable: false,
                        Input: 1,
                        Inputs: [
                             /*"Local-Simple"*/transerv.translate('RCTRL_P00_ALS'),
                             /*"Local-P/R"*/transerv.translate('RCTRL_P00_ALP'),
                             /*"Local FD-Simple"*/transerv.translate('RCTRL_P00_ALD'),
                             /*"Local FD-P/R"*/transerv.translate('RCTRL_P00_ALF'),
                             /*"Remoto RxTx"*/transerv.translate('RCTRL_P00_RTR'),
                             /*"Remoto Tx"*/transerv.translate('RCTRL_P00_RTX'),
                             /*"Remoto Rx"]*/transerv.translate('RCTRL_P00_RRX')],
                        Show: vm.p2_rad_show,
                        Val: vm.dval
                    },
                    //{
                    //    Name: /*'Indicacion Entrada Audio:'*/transerv.translate('RCTRL_P02_IENA'),
                    //    Value: vm.rdata.radio.sq,
                    //    Enable: true,
                    //    Input: 1,
                    //    Inputs: [/*"Hardware"*/transerv.translate('RCTRL_P02_HARD'), /*"VAD"*/transerv.translate('RCTRL_P02_VAD')],
                    //    Show: vm.rad_show,
                    //    Val: vm.dval
                    //},
                    vm.sqh_options(),   // 1
                    {   // 2
                        Name: /*'Umbral VAD (dbm):'*/transerv.translate('RCTRL_P02_UVAD'),
                        Value: vm.rdata.radio.umbralVad,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_rad_show,
                        Val: vm.vad_val
                    },
                    {   // 3
                        Name: /*'Indicacion Salida Audio:'*/transerv.translate('RCTRL_P02_ISDA'),
                        Value: vm.rdata.radio.ptt.toString(),
                        Enable: false,
                        Input: 1,
                        Inputs: [/*"Hardware"*/transerv.translate('RCTRL_P02_HARD'), /*"Tono"*/transerv.translate('RCTRL_P02_TONO')],
                        Show: vm.p2_rad_show,
                        Val: vm.dval
                    },
                    {   // 4
                        Name: /*'Tiempo PTT Maximo:'*/transerv.translate('RCTRL_P02_TPTT'),
                        Value: vm.rdata.radio.tiempoPtt,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_rad_show,
                        Val: vm.dval
                    },
                    {   // 5
                        Name: isLocal() ? /*'Metodos BSS Disponibles:'*/transerv.translate('RCTRL_P02_BSSD') : /*'Método BSS Preferido:'*/transerv.translate('RCTRL_P02_BSSP'),
                        Value: vm.rdata.radio.metodoBss.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: isLocal() ? [
                            /*"Ninguno"*/transerv.translate('RCTRL_P02_BSSN'),
                            /*"RSSI"*/transerv.translate('RCTRL_P02_BSSR'),
                            /*"RSSI y NUCLEO"*/transerv.translate('RCTRL_P02_BSSC'),
                            /*"NUCLEO"*/transerv.translate('RCTRL_P02_BSSC1')
                        ] : [
                            /*"RSSI"*/transerv.translate('RCTRL_P02_BSSR'),
                            /*"NUCLEO"*/transerv.translate('RCTRL_P02_BSSC1'),
                            /*"Ninguno"*/transerv.translate('RCTRL_P02_BSSN'),
                            ],
                        Show: vm.p2_rad_show,
                        Val: vm.dval
                    },
                    { // 6
                        Name: /*'BSS/CLIMAX?:'*/transerv.translate('RCTRL_P02_BSCX'),
                        Value: vm.rdata.radio.bss.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('RCTRL_P00_NO'), /*"Si"*/transerv.translate('RCTRL_P00_SI')],
                        Show: vm.p2_rad_show,
                        Val: vm.dval
                    },
                    { // 7
                        Name: /*'Ventana BSS (ms):'*/transerv.translate('RCTRL_P02_BSCXW'),
                        Value: vm.rdata.radio.tmVentanaRx,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_rad_show,
                        Val: vm.bssv_val
                    },
                    { // 8
                        Name: /*'BSS COLA SQHELCH (ms):'*/transerv.translate('RCTRL_P02_BSCXQ'),
                        Value: vm.rdata.radio.retrasoSqOff,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_rad_show,
                        Val: vm.bssv_val
                    },
                    { // 9
                        Name: /*'CLIMAX DELAY:'*/transerv.translate('RCTRL_P02_BSCXD'),
                        Value: vm.rdata.radio.climaxDelay.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('RCTRL_P00_NO'), /*"ASAP"*/transerv.translate('RCTRL_P02_BSCXA'), /*"Tiempo Fijo"*/transerv.translate('RCTRL_P02_BSCXF')],
                        Show: vm.p2_rad_show,
                        Val: vm.dval
                    },
                    { // 10
                        Name: /*'TIEMPO (ms):'*/transerv.translate('RCTRL_P02_BSCXT'),
                        Value: vm.rdata.radio.tmRetardoFijo,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_rad_show,
                        Val: vm.climaxt_val
                    },
                    { // 11
                        Name: transerv.translate('Metodo de Calculo CLIMAX'),
                        Value: vm.rdata.radio.iModoCalculoClimax.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [transerv.translate('Relativo'), transerv.translate('Absoluto')],
                        Show: vm.p2_rad_show,
                        Val: vm.dval
                    },
                    { // 12 <-- 11
                        Name: /*'GRS INTERNAL DELAY (ms):'*/transerv.translate('RCTRL_P02_BSCXG'),
                        Value: vm.rdata.radio.tGRSid,
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_rad_show,
                        Val: vm.climaxt_val
                    },
                    { // 13 <-- 12
                        Name:/*'Prioridad PTT'*/transerv.translate('RCTRL_P02_PTTP'),
                        Value: vm.rdata.radio.iPttPrio.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [
                             ///*"PTT-OFF"*/transerv.translate('RCTRL_P02_PTTP0'),
                             /*"PTT-Normal"*/transerv.translate('RCTRL_P02_PTTP1'),
                             ///*"PTT-Coupling"*/transerv.translate('RCTRL_P02_PTTP2'),
                             /*"PTT-Prioritario"*/transerv.translate('RCTRL_P02_PTTP3'),
                             /*"PTT-Emergencia"*/transerv.translate('RCTRL_P02_PTTP4')
                        ],
                        Show: vm.p2_rad_show,
                        Val: vm.dval
                    },
                    { // 14 <-- 13
                        Name:/*'Prioridad Session'*/transerv.translate('RCTRL_P02_SESP'),
                        Value: vm.rdata.radio.iSesionPrio.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [
                             /*"Normal"*/transerv.translate('RCTRL_P02_SESP0'),
                             /*"Emergencia"*/transerv.translate('RCTRL_P02_SESP1')
                        ],
                        Show: vm.p2_rad_show,
                        Val: vm.dval
                    },
                    { // 15 <-- 14
                        Name:/*'Tabla QIDX'*/transerv.translate('Calificacion BSS'),
                        Value: vm.tbQidx(vm.rdata.radio),                           // "00-01-02-03-04-05",
                        Enable: false,
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_rad_show,
                        Val: vm.dval
                    }
                ];
                break;
            case 3:
                vm.vdata = [                                                       // Enable: false,
                    {
                        Name:/*'Tipo de Agente Radio:'*/transerv.translate('RCTRL_P00_TPA'),
                        Value: vm.rdata.radio.tipo.toString(),
                        Enable: false,
                        Input: 1,
                        Inputs: [
                             /*"Local-Simple"*/transerv.translate('RCTRL_P00_ALS'),
                             /*"Local-P/R"*/transerv.translate('RCTRL_P00_ALP'),
                             /*"Local FD-Simple"*/transerv.translate('RCTRL_P00_ALD'),
                             /*"Local FD-P/R"*/transerv.translate('RCTRL_P00_ALF'),
                             /*"Remoto RxTx"*/transerv.translate('RCTRL_P00_RTR'),
                             /*"Remoto Tx"*/transerv.translate('RCTRL_P00_RTX'),
                             /*"Remoto Rx"]*/transerv.translate('RCTRL_P00_RRX')],
                        Show: vm.p3_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Frecuencia:'*/transerv.translate('RCTRL_P00_FRC'),
                        Value: vm.rdata.radio.colateral.name,
                        Enable: false,
                        Input: 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.fid_val
                    },
                    {
                        Name: /*'Emplazamiento-1:'*/transerv.translate('RCTRL_P03_SITE1'),
                        Value: [],
                        Enable: false,
                        Input: -1,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'-- URI-TX-A:'*/transerv.translate('RCTRL_P03_TXA'),
                        Value: vm.lcol < 1 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[0].uriTxA),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'-- URI-TX-B:'*/transerv.translate('RCTRL_P03_TXB'),
                        Value: vm.lcol < 1 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[0].uriTxB),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'-- URI-RX-A:'*/transerv.translate('RCTRL_P03_RXA'),
                        Value: vm.lcol < 1 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[0].uriRxA),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'-- URI-RX-B:'*/transerv.translate('RCTRL_P03_RXB'),
                        Value: vm.lcol < 1 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[0].uriRxB),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'Emplazamiento-2:'*/transerv.translate('RCTRL_P03_SITE2'),
                        Value: [],
                        Enable: false,
                        Input: -1,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'-- URI-TX-A:'*/transerv.translate('RCTRL_P03_TXA'),
                        Value: vm.lcol < 2 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[1].uriTxA),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'-- URI-TX-B:'*/transerv.translate('RCTRL_P03_TXB'),
                        Value: vm.lcol < 2 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[1].uriTxB),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'-- URI-RX-A:'*/transerv.translate('RCTRL_P03_RXA'),
                        Value: vm.lcol < 2 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[1].uriRxA),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'-- URI-RX-B:'*/transerv.translate('RCTRL_P03_RXB'),
                        Value: vm.lcol < 2 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[1].uriRxB),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    },

                    {
                        Name: /*'Emplazamiento-3:'*/transerv.translate('RCTRL_P03_SITE3'),
                        Value: [],
                        Enable: false,
                        Input: -1,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'-- URI-TX-A:'*/transerv.translate('RCTRL_P03_TXA'),
                        Value: vm.lcol < 3 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[2].uriTxA),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'-- URI-TX-B:'*/transerv.translate('RCTRL_P03_TXB'),
                        Value: vm.lcol < 3 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[2].uriTxB),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'-- URI-RX-A:'*/transerv.translate('RCTRL_P03_RXA'),
                        Value: vm.lcol < 3 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[2].uriRxA),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'-- URI-RX-B:'*/transerv.translate('RCTRL_P03_RXB'),
                        Value: vm.lcol < 3 ? "" : quitar_sip(vm.rdata.radio.colateral.emplazamientos[2].uriRxB),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p3_rad_show,
                        Val: vm.uri_val
                    }
                ];
                break;
            case 4:
                vm.vdata = [
                    {
                        Name: /*'Tipo de Restriccion:'*/transerv.translate('RCTRL_P04_RES'),
                        Value: vm.rdata.restriccion.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: 1,
                        Inputs: [/*"Ninguna"*/transerv.translate('RCTRL_P04_RESN'),
                            /*"Lista Negra"*/transerv.translate('RCTRL_P04_RESB'),
                            /*"Lista Blanca"]*/transerv.translate('RCTRL_P04_RESW')],
                        Show: vm.lbn_show,
                        Val: function () { return ""; }
                    },
                    {
                        Name: /*'Lista Blanca:'*/transerv.translate('RCTRL_P04_RESW'),
                        Value: quitar_sip(vm.rdata.blanca),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 4 : 2,
                        Inputs: [],
                        Show: vm.lbn_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'Lista Negra:'*/transerv.translate('RCTRL_P04_RESB'),
                        Value: quitar_sip(vm.rdata.negra),
                        Enable: authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        Input: jamp_no_sip == 1 ? 4 : 2,
                        Inputs: [],
                        Show: vm.lbn_show,
                        Val: vm.uri_val
                    }
                ];
                break;
            default:
                vm.vdata = [];
                break;
        }
    }

    /* */
    function v2jdata() {
        if (vm.rdata == undefined)
            return;
        lcol = vm.rdata.radio.colateral.emplazamientos.length;
        switch (vm.pagina) {
            case 0:
                vm.rdata.IdRecurso = vm.vdata[0].Value;
                vm.rdata.radio.colateral.name = vm.vdata[2].Value;
                vm.rdata.radio.tipo = parseInt(vm.vdata[3].Value);
                vm.rdata.Uri_Local = poner_sip(vm.vdata[4].Value);
                vm.rdata.enableRegistro = parseInt(vm.vdata[5].Value);
                vm.rdata.szClave = vm.vdata[6].Value;
                vm.rdata.radio.evtPTT = parseInt(vm.vdata[7].Value);
                vm.rdata.radio.iEnableGI = parseInt(vm.vdata[8].Value);
                /* TODO. Forzar PTTSQH vm.tdata.szClave = vm.vdata[9].Value;*/
                break;
            case 1:
                vm.rdata.Codec = parseInt(vm.vdata[0].Value);
                vm.rdata.hardware.AD_AGC = parseInt(vm.vdata[1].Value);
                vm.rdata.hardware.AD_Gain = vm.vdata[2].Value * 10;
                vm.rdata.hardware.DA_AGC = parseInt(vm.vdata[3].Value);
                vm.rdata.hardware.DA_Gain = vm.vdata[4].Value * 10;
                vm.rdata.radio.tjbd = vm.vdata[5].Value;
                vm.rdata.radio.iPrecisionAudio = parseInt(vm.vdata[6].Value);
                break;
            case 2:
                vm.rdata.radio.tipo = parseInt(vm.vdata[0].Value);
                vm.rdata.radio.sq = vm.vdata[1].Value;
                vm.rdata.radio.umbralVad = vm.vdata[2].Value;
                vm.rdata.radio.ptt = parseInt(vm.vdata[3].Value);
                vm.rdata.radio.tiempoPtt = vm.vdata[4].Value;
                vm.rdata.radio.metodoBss = parseInt(vm.vdata[5].Value);
                vm.rdata.radio.bss = parseInt(vm.vdata[6].Value);
                vm.rdata.radio.tmVentanaRx = vm.vdata[7].Value;
                vm.rdata.radio.retrasoSqOff = vm.vdata[8].Value;
                vm.rdata.radio.climaxDelay = parseInt(vm.vdata[9].Value);
                vm.rdata.radio.tmRetardoFijo = vm.vdata[10].Value;
                vm.rdata.radio.iModoCalculoClimax = vm.vdata[11].Value;
                vm.rdata.radio.tGRSid = vm.vdata[12].Value;
                vm.rdata.radio.iPttPrio = parseInt(vm.vdata[13].Value);
                vm.rdata.radio.iSesionPrio = parseInt(vm.vdata[14].Value);
                break;
            case 3:
                vm.rdata.radio.tipo = parseInt(vm.vdata[0].Value);
                vm.rdata.radio.colateral.name = vm.vdata[1].Value;
                if (vm.lcol >= 1) {
                    vm.rdata.radio.colateral.emplazamientos[0].uriTxA = poner_sip(vm.vdata[3].Value);
                    vm.rdata.radio.colateral.emplazamientos[0].uriTxB = poner_sip(vm.vdata[4].Value);
                    vm.rdata.radio.colateral.emplazamientos[0].uriRxA = poner_sip(vm.vdata[5].Value);
                    vm.rdata.radio.colateral.emplazamientos[0].uriRxB = poner_sip(vm.vdata[6].Value);
                }
                if (vm.lcol >= 2) {
                    vm.rdata.radio.colateral.emplazamientos[1].uriTxA = poner_sip(vm.vdata[8].Value);
                    vm.rdata.radio.colateral.emplazamientos[1].uriTxB = poner_sip(vm.vdata[9].Value);
                    vm.rdata.radio.colateral.emplazamientos[1].uriRxA = poner_sip(vm.vdata[10].Value);
                    vm.rdata.radio.colateral.emplazamientos[1].uriRxB = poner_sip(vm.vdata[11].Value);
                }
                if (vm.lcol >= 3) {
                    vm.rdata.radio.colateral.emplazamientos[2].uriTxA = poner_sip(vm.vdata[13].Value);
                    vm.rdata.radio.colateral.emplazamientos[2].uriTxB = poner_sip(vm.vdata[14].Value);
                    vm.rdata.radio.colateral.emplazamientos[2].uriRxA = poner_sip(vm.vdata[15].Value);
                    vm.rdata.radio.colateral.emplazamientos[2].uriRxB = poner_sip(vm.vdata[16].Value);
                }
                break;
            case 4:
                vm.rdata.restriccion = parseInt(vm.vdata[0].Value);
                vm.rdata.blanca = poner_sip(vm.vdata[1].Value);
                vm.rdata.negra = poner_sip(vm.vdata[2].Value);
                break;
        }
    }

    /* */
    function isLocal() {
        return (vm.rdata != undefined && vm.rdata.radio.tipo) <= 3 ? true : false;
    }

    /* */
    function validate_empl() {
        var validate = true;
        angular.forEach(vm.vdata, function (value, index) {
            switch (index) {
                case 0:
                case 1:
                case 2:
                case 7:
                case 12:
                    break;
                default:
                    if (value.Show(index) && vm.uri_val(value.Value)!="") validate = false;
            }
        });
        return validate;
    }

    /* */
    function validate_lbn() {
        var validate = true;
        if (vm.vdata[0].Value == 1) {       // lista negra
            angular.forEach(vm.vdata[2].Value, function (value, index) {
                if (vm.uri_val(value)!="") validate = false;
            });
        }
        else if (vm.vdata[0].Value == 2) { // lista blanca.
            angular.forEach(vm.vdata[1].Value, function (value, index) {
                if (vm.uri_val(value)!="") validate = false;
            });
        }
        return validate;
    }


    /* */
    function validate_page() {

        if (vm.rdata == undefined)
            return true;

        switch (vm.pagina) {
            case 0:
                return vm.uri_val(vm.vdata[4].Value)=="" && vm.fid_val(vm.vdata[2].Value)=="";
            case 1:
                if (vm.vdata[2].Show(2) && vm.cbmad_val(vm.vdata[2].Value)!="") return false;
                if (vm.vdata[4].Show(4) && vm.cbmda_val(vm.vdata[4].Value)!="") return false;
                break;
            case 2:
                if (vm.vdata[2].Show(2) && vm.vad_val(vm.vdata[2].Value)!="") return false;
                if (vm.vdata[7].Show(7) && vm.bssv_val(vm.vdata[7].Value) != "") return false;
                if (vm.vdata[10].Show(10) && vm.climaxt_val(vm.vdata[10].Value) != "") return false;
                break;
            case 3:
                return validate_empl();
            case 4:
                return validate_lbn();
        }
        return true;
    }

    /* */
    function Init() {
        vm.radiosel = "0";
        vm.current = -1;
        vm.pagina = 0;
    }

    /* */
    CfgService.init().then(function () {
        get_radios();
        if ($routeParams.recId == -1)
            vm.radiosel = CfgService.radio_last_get().toString();
        else
            vm.radiosel = CfgService.radio_index_get(Math.floor($routeParams.recId / 4), $routeParams.recId % 4).toString();
        vm.update_radio();
    });

    /* Cambio de Vista... */
    $scope.$on("$locationChangeStart", function (event) {
        if (!authservice.global_validate(validate_page())) {
            /*alert*/alertify.error("02 " + transerv.translate('ICTRL_MSG_01'));  // "Existen Errores de Formato o Rango. No puede cambiar de vista...");
            event.preventDefault();
        }
        else
            set_radio_data(parseInt(vm.radiosel));
    });

    /* Se ha pulsado el boton -aplicar- */
    $scope.$on('savecfg', function (data) {
        //console.log("savecfg");
        if (!authservice.global_validate(validate_page())) {
            /*alert*/alertify.error("03 " + transerv.translate('ICTRL_MSG_01'));  // "Existen Errores de Formato o Rango. No puede salvar lo cambios...");
        }
        else {
            //if (CfgService.test_ip_virtual() == true ||
            //    confirm(transerv.translate('GCTRL_IPV_WARNING')) == true) {
            //    CfgService.aplicar_cambios();
            //}
            if (CfgService.test_ip_virtual() == true) {
                CfgService.aplicar_cambios();
            }
            else {
                AltfyConfirm(authservice, transerv.translate('GCTRL_IPV_WARNING'), function () {
					setTimeout(function(){
						CfgService.aplicar_cambios();
					},500);
                });
            }
        }
    });

    /* */
    $scope.$on('loadcfg', function (data) {
        Init();
        get_radios();
        vm.radiosel = CfgService.radio_last_get().toString();
        vm.update_radio();
    });

    /* */
    $scope.$on('std_change', function (data) {
        $route.reload();
    });
}
