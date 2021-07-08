/** */
angular
    .module('Ug5kweb')
    .controller('ug5kRectCtrl', ug5kRectCtrl);

ug5kRectCtrl.$inject = ['$scope', '$routeParams', '$route', 'authservice', 'CfgService', 'ValidateService', 'transerv', 'MantService'];

function ug5kRectCtrl($scope, $routeParams, $route, authservice, CfgService, ValidateService, transerv, MantService) {
    var vm = this;
    var simul = true;

    /** Modelo */
    vm.selected = "0";
    vm.current = -1;
    vm.ltelef = [];
    vm.tdata = {};
    vm.pagina = 0;
    vm.vdata = [];
    vm.autosave = v2jdata;
    vm.dval = function () { return ""; };

    /** 20190207. Personalizacion de los Errores de Formato. */
    vm.valService = ValidateService;

    CfgService.opcion(3);

    /**
     * Validador de IdRecurso. Actualiza a la ver la URI Local 
     * 20190207. Los Recursos LCEN(5), BL (0), BC(1) y AB(2) Son de Formato Libre
     * @param {any} value
     */
    vm.id_val = function (value) {
        var error = ValidateService.max_long_val(value);
        if ( error == "") {
            vm.tdata.IdRecurso = value;
            var teldata = vm.tdata.telefonia;
            if ((teldata.tipo != 0 && teldata.tipo != 1 && teldata.tipo != 2 && teldata.tipo != 5)) {
                vm.vdata[3].Value = jamp_no_sip == 1 ? CfgService.quitar_sip(CfgService.rec_uri_get(vm.tdata)) : CfgService.rec_uri_get(vm.tdata);
            }
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
    /**
     * 
     * @param {any} uri
     */
    vm.uri_or_ip_val = (uri) => {
        var ipv = ValidateService.ip_val(uri);
        var uriv = ValidateService.uri_val(uri);
        if (ipv == "" || uriv == "")
            return "";
        return uriv == "" ? ipv : uriv;
    };

    /* Validador cbm en A/D */
    vm.cbmad_val = function (value) {
        if (value.toString() == "")
            return transerv.translate("El valor debe estar entre: ") + pr_ad_rng.toString();
        return value >= pr_ad_rng.min && value <= pr_ad_rng.max ? "" : transerv.translate("El valor debe estar entre: ") + pr_ad_rng.toString();
    };

    /* Validador cmd en D/A */
    vm.cbmda_val = function (value) {
        if (value.toString() == "")
            return transerv.translate("El valor debe estar entre: ") + pr_da_rng.toString();
        return value >= pr_da_rng.min && value <= pr_da_rng.max ? "" : transerv.translate("El valor debe estar entre: ") + pr_da_rng.toString();
    };

    /* Validador nivel VOX */
    vm.vox_val = function (value) {
        return value >= pr_vad_rng.min && value <= pr_vad_rng.max ? "" : transerv.translate("El valor debe estar entre: ") + pr_vad_rng.toString();
    };

    /* Validador cola VOX */
    vm.cvox_val = function (value) {
        return value >= pr_vxq_rng.min && value <= pr_vxq_rng.max ? "" : transerv.translate("El valor debe estar entre: ") + pr_vxq_rng.toString();
    };

    /* Validador Periodo tonos respuesta estado (s) */
    vm.ptre_val = function (value) {
        return value >= pr_ptre_rng.min && value <= pr_ptre_rng.max ? "" : transerv.translate("El valor debe estar entre: ") + pr_ptre_rng.toString();
    };

    /* Validador tiempo supervision (s) */
    vm.tsup_val = function (value) {
        return value >= pr_tsup_rng.min && value <= pr_tsup_rng.max ? "" : transerv.translate("El valor debe estar entre: ") + pr_tsup_rng.toString();
    };

    /* */
    vm.validate_ats_range = function (range) {
        var match = range.match(regx_atsrango);
        return (range == "" || match) ? "" : transerv.translate("Los valores deben estar entre: ") + "200000-399999";
    };

    /* */
    vm.validate_ats_number = function (number) {
        var match = number.match(regx_atsnumber);
        return (number == "" || match) ? "" : transerv.translate("El valor debe estar entre: ") + "200000-399999";
    };

    /* */
    vm.validate_iwp = function (valor) {
        return (valor >= pr_iwp_rng.min && valor <= pr_iwp_rng.max) ? "" : transerv.translate("El valor debe estar entre: ") + pr_iwp_rng.toString();
    };

    vm.validate_local_uri = function (valor) {
        /** valida el formato */
        var msg = ValidateService.uri_val(valor);
        if (msg != "")
            return msg;
        /** valida que la IP del campo coincide con la ip virtual de la pasarela */
        var groups = valor.match(regx_urival_nosip);
        if (groups[2] == undefined || CfgService.cfg_get().general.ipv != groups[2]) {
            return transerv.translate("La IP de la URI debe ser: ") + CfgService.cfg_get().general.ipv;
        }
        return "";
    };

    /* */
    vm.show_listas_bn = function () {
        //return !vm.show_rangos_ats();
        return false;
    };

    /* */
    vm.show_plbn = function () {
        var hide = MantService.hide_on_ulises();
        if (hide == false)
            return false;
        return vm.show_rangos_ats();
    };

    /* */
    vm.show_penl = function () {
        var hide = MantService.hide_on_ulises();
        return !hide;
    };

    /* */
    vm.show_clave = function () {
        return vm.vdata[4].Value == 1 ? true : false;
    };

    /* */
    vm.show_rangos_ats = function () {

        if (vm.tdata == undefined)
            return false;
/* 20200706. En todas las líneas telefonicas se muestran al menos los rangos de Origen.
        switch (vm.pagina) {
            case 0:
                return (vm.vdata.length > 0) && (vm.vdata[2].Value == 3 || vm.vdata[2].Value == 4 || vm.vdata[2].Value == 6);
            case 1:
            case 3:
            case 4:
                return (vm.tdata.telefonia.tipo == 3 || vm.tdata.telefonia.tipo == 4 || vm.tdata.telefonia.tipo == 6);
            case 2:
                return (vm.vdata.length > 0) && (vm.vdata[0].Value == 3 || vm.vdata[0].Value == 4 || vm.vdata[2].Value == 6);
        }
        return false;
*/
        return true;
    };

    /* */
    vm.bnorats = function () {
        return vm.show_rangos_ats() ? /*"Rangos ATS"*/transerv.translate('TCTRL_PG03_OPT1') : /*"Listas B/N"*/transerv.translate('TCTRL_PG03_OPT2');
    };

    vm.AtsRangesLabelShow = () => { return { show: true, label: transerv.translate('TCTRL_PG03_OPT1') } };
    vm.AtsRangesShow = (IsOrg, LineType) =>
    {
        return IsOrg == true ? true : (LineType == 3 || LineType == 4 || LineType == 6) ? true : false;
    }

    /* */
    vm.telgain_show = function (ind) {
        ////if (vm.pagina == 1)
        //    return true;
        switch (ind) {
            case 2:
                return (vm.vdata[1].Value == 0);
            case 4:
                return (vm.vdata[3].Value == 0);
        }
        return true;
    };

    /* */
    vm.update_telef = function () {
        if (vm.current != -1)
            set_telef_data(vm.current);
        get_telef_data(parseInt(vm.selected));

        vm.set_pagina(0);
    };

    /* */
    vm.set_pagina = function (pagina) {
        if (!authservice.global_validate(validate_page())) {
            /*alert*/alertify.error(/*"Existen Errores de Formato o Rango. No puede cambiar de vista..."*/transerv.translate('ICTRL_MSG_01'));
        }
        else if (authservice.check_session() == true) {
            v2jdata();
            vm.pagina = pagina;
            j2vdata();
        }
    };

    /* */
    vm.lbn_show = function (ind) {
        if (vm.show_listas_bn()) {
            switch (ind) {
                case 1:
                    return (vm.vdata[0].Value == 2);
                case 2:
                    return (vm.vdata[0].Value == 1);
                default:
                    return true;
            }
        }
        return false;
    };

    /* */
    vm.p0_tel_show = function (ind) {
        switch (ind) {
            case 0:             // ID recurso
                return true;
            case 1:             // Slot / Posicion
                return true;
            case 2:             // Tipo de Interfaz
                return true;
            case 3:             // URI
                return true;
            case 4:             // Enable Registro
                return MantService.hide_on_ulises();
            case 5:             // Clave.
                return vm.show_clave();
        }
        return false;
    };

    /* */
    vm.p1_tel_show = function (ind) {
        switch (ind) {
            case 0:             // CODEC
            case 1:             // AGC A/D
            case 3:             // AGC D/A
                return true;
            case 2:             // A/D Gain
            case 4:             // D/A Gain
                return vm.telgain_show(ind);
        }
        return false;
    };

    /* */
    vm.p2_tel_show = function (ind) {
        switch (ind) {
            case 0: // Tipo
                return true;

            case 1: // uri
                if (MantService.hide_on_ulises() == false)
                    return false;
                return vm.vdata[0].Value != 6;

            case 2: // Respuesta automatica simulada. Para R2/N5 y LCEN
            case 7: // Supervisa Colateral
                if (MantService.hide_on_ulises() == false)
                    return false;
                return (vm.vdata[0].Value == 3 || vm.vdata[0].Value == 4 || vm.vdata[0].Value == 5);

            case 4:  // Lado.        N5/R2
            case 12: // Periodo Interrupt Warning
                return (vm.vdata[0].Value == 3 || vm.vdata[0].Value == 4);
            case 25: // Time out respuesta llamada
                return (vm.vdata[0].Value == 3);

            case 5: // Test Remoto.
            case 6: // Test Local.
                if (MantService.hide_on_ulises() == false)
                    return false;
                return (vm.vdata[0].Value == 3 || vm.vdata[0].Value == 4);

            case 3: // Tiempo Release.
                if (MantService.hide_on_ulises() == false)
                    return false;
                return ((vm.vdata[0].Value == 3 || vm.vdata[0].Value == 4 || vm.vdata[0].Value == 5) && vm.vdata[2].Value == 1);

            case 8: // Tiempo Supervision Colateral
                if (MantService.hide_on_ulises() == false)
                    return false;
                return ((vm.vdata[0].Value == 3 || vm.vdata[0].Value == 4 || vm.vdata[0].Value == 5) && vm.vdata[7].Value == 1);

            case 9: // Vox BL
                return (vm.vdata[0].Value == 0);
            case 10: // Umbral Vox BL
            case 11: // Cola Umbral BL
                return (vm.vdata[0].Value == 0 && vm.vdata[9].Value == 1);

            case 13: // ID-RED.     Solo en AB
                if (MantService.hide_on_ulises() == true)
                    return false;
                return (vm.vdata[0].Value == 2);

            case 14: // ID-TRONCAL. En R2/N5
                if (MantService.hide_on_ulises() == true)
                    return false;
                return (vm.vdata[0].Value == 3 || vm.vdata[0].Value == 4);

            default:
                return false;
        }
    };

    /* */
    vm.p3_tel_show = function (ind) {
        switch (ind) {
            case 0:             // Tipo de Restriccion
            case 1:
            case 2:
                return vm.lbn_show();
            default:
                return vm.show_rangos_ats();
        }
    };

    /* */
    vm.restore = function () {
        CfgService.restore().then(function () {
            get_telef();
            get_telef_data(parseInt(vm.selected));
        });
    };

    /* */
    function NormalizeTmSuperv(toDisplay, valor) {
        var retorno = 0;
        if (toDisplay === true) {
            retorno = valor < 10 ? 0 :
                valor < 15 ? 1 :
                    valor < 20 ? 2 :
                        valor < 25 ? 3 :
                            valor < 30 ? 4 : 5;
        }
        else {
            retorno = valor == 0 ? 5 :
                valor == 1 ? 10 :
                    valor == 2 ? 15 :
                        valor == 3 ? 20 :
                            valor == 4 ? 25 : 30;
        }
        return retorno;
    }

    /* */
    function get_telef() {
        vm.ltelef = CfgService.ltelef();
    }

    /* */
    function get_telef_data(telef) {
        vm.selected = telef.toString();
        vm.current = telef;
        vm.tdata = CfgService.telef_data_get(telef);
        j2vdata();
    }

    /* */
    function set_telef_data(telef) {
        if (vm.tdata != undefined) {
            v2jdata();
            CfgService.telef_data_set(telef, vm.tdata);
        }
    }

    function Page5OtherParameterShow(LineType) {
        if (MantService.hide_on_ulises() == false)
            return false;
        return true;
    }

    function Page5SupervisionTimeShow(LineType, Sp1, Sp2) {
        if (MantService.hide_on_ulises() == false)
            return false;
        return LineType == 7 ? Sp1 == 1 : Sp1 != 0 || Sp2 != 0;
    }

    function Page5AutomaticRspShow(LineType) {
        if (MantService.hide_on_ulises() == false)
            return false;
        return (LineType == 3 || LineType == 4 || LineType == 5);
    }

    function Page5AdditionalUriAndDataShow(LineType) {
        if (MantService.hide_on_ulises() == false)
            return false;
        return LineType != 7;
    }
    /* */
    function j2vdata() {

        if (vm.tdata == undefined)
            return;
        switch (vm.pagina) {
            case 0:
                vm.vdata = [
                    {
                        Name: /*'Recurso:'*/transerv.translate('TCTRL_P00_RES'),
                        Value: vm.tdata.IdRecurso,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.p0_tel_show,
                        Val: vm.id_val
                    },
                    {
                        Name: /*'SLOT/Posicion:'*/transerv.translate('TCTRL_P00_ST'),
                        Value: vm.tdata.SlotPasarela + " / " + vm.tdata.NumDispositivoSlot,
                        Enable: function () {
                            return false;
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.p0_tel_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Tipo de Interfaz Telefonico:'*/transerv.translate('TCTRL_P00_ITF'),
                        Value: vm.tdata.telefonia.tipo.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [
                              /*"PP-BL"*/transerv.translate('TCTRL_P00_IBL')
                            ,/*"PP-BC"*/transerv.translate('TCTRL_P00_IBC')
                            ,/*"PP-AB"*/transerv.translate('TCTRL_P00_IAB')
                            ,/*"ATS-R2"*/transerv.translate('TCTRL_P00_IR2')
                            ,/*"ATS-N5"*/transerv.translate('TCTRL_P00_IN5')
                            ,/*"LCEN"*/transerv.translate('TCTRL_P00_ILC')
                            ,/*"ATS-QSIG"*/transerv.translate('TCTRL_P00_IQS')
                            ,/*"TUN-LOC"*/transerv.translate('TCTRL_P00_LTU')
                            //,/*"TUN-REM"*/transerv.translate('TCTRL_P00_RTU')
                        ],
                        Show: vm.p0_tel_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'URI:'*/transerv.translate('TCTRL_P00_URI'),
                        Value: jamp_no_sip == 1 ? CfgService.quitar_sip(vm.tdata.Uri_Local) : vm.tdata.Uri_Local,
                        Enable: function () {
                            var global_enable = authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                            return global_enable ?
                                vm.tdata.telefonia.tipo == 0 || vm.tdata.telefonia.tipo == 1 || vm.tdata.telefonia.tipo == 2 || vm.tdata.telefonia.tipo == 5 :
                                global_enable;
                        },
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: vm.p0_tel_show,
                        Val: vm.validate_local_uri
                    },
                    {
                        Name: /*'Enable Registro ?:'*/transerv.translate('TCTRL_P00_REG'),
                        Value: vm.tdata.enableRegistro.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: vm.p0_tel_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Clave:'*/transerv.translate('TCTRL_P00_CLV'),
                        Value: vm.tdata.szClave,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.p0_tel_show,

                        Val: ValidateService.max_long_val
                    },
                    {
                        // 0 - 6
                        Name: transerv.translate('Acepta Llamadas No ED137 ?'),
                        Value: vm.tdata.telefonia.iEnableNoED137.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: function () { return vm.vdata[2].Value != "5"; },    // Todas las Líneas menos LCEN
                        Val: function () { return ""; }
                    }
                ];
                break;
            case 1:
                vm.vdata = [
                    {
                        Name: /*'CODEC :'*/transerv.translate('TCTRL_P01_COD'),
                        Value: vm.tdata.Codec.toString(),
                        Enable: function () {
                            return false;
                        },
                        Input: 1,
                        Inputs: ["G711-A", "G711-U", "G729"],
                        Show: vm.p1_tel_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'AGC en A/D ?:'*/transerv.translate('TCTRL_P01_AGCAD'),
                        Value: vm.tdata.hardware.AD_AGC.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: vm.p1_tel_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Ajuste Cero Digital en A/D (dBm):'*/transerv.translate('TCTRL_P01_AGCAD_AJUSTE'),
                        Value: vm.tdata.hardware.AD_Gain / 10,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.telgain_show,
                        Val: vm.cbmad_val
                    },
                    {
                        Name: /*'AGC en D/A ?:'*/transerv.translate('TCTRL_P01_AGCDA'),
                        Value: vm.tdata.hardware.DA_AGC.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: vm.p1_tel_show,
                        Val: vm.dval
                    },
                    {
                        Name: /*'Ajuste Cero Digital en A/D (dBm)::'*/transerv.translate('TCTRL_P01_AGCDA_AJUSTE'),
                        Value: vm.tdata.hardware.DA_Gain / 10,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.telgain_show,
                        Val: vm.cbmda_val
                    }
                ];
                break;
            case 2:
                vm.vdata = [
                    {
                        // 2 - 00
                        Name: /*'Tipo de Interfaz Telefonico:'*/transerv.translate('TCTRL_P00_ITF'),
                        Value: vm.tdata.telefonia.tipo.toString(),
                        Enable: function () {
                            return false;
                        },
                        Input: 1,
                        Inputs: [
                              /*"PP-BL"   */transerv.translate('TCTRL_P00_IBL'),
                              /*"PP-BC"   */transerv.translate('TCTRL_P00_IBC'),
                              /*"PP-AB"   */transerv.translate('TCTRL_P00_IAB'),
                              /*"ATS-R2"  */transerv.translate('TCTRL_P00_IR2'),
                              /*"ATS-N5"  */transerv.translate('TCTRL_P00_IN5'),
                              /*"LCEN"    */transerv.translate('TCTRL_P00_ILC'),
                              /*"ATS-QSIG"*/transerv.translate('TCTRL_P00_IQS'),
                              /*"TUN-LOC" */transerv.translate('TCTRL_P00_LTU'),
                              /*"TUN-REM" */transerv.translate('TCTRL_P00_RTU')],
                        Show: vm.p2_tel_show,
                        Val: vm.dval
                    },
                    {
                        // 2 - 01
                        Name: /*'URI Remota:'*/transerv.translate('TCTRL_P02_RURI'),
                        Value: jamp_no_sip == 1 ? CfgService.quitar_sip(vm.tdata.telefonia.uri_remota) : vm.tdata.telefonia.uri_remota,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: /*vm.p2_tel_show*/() => { return false; },
                        Val: vm.uri_val
                    },
                    {
                        // 2 - 02
                        Name: /*'Respuesta Automatica Simulada ?:'*/transerv.translate('TCTRL_P02_ARSP'),
                        Value: vm.tdata.telefonia.r_automatica.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: vm.p2_tel_show/*() => { return false; }*/,
                        Val: vm.dval
                    },
                    {
                        // 2 - 03
                        Name: /*'Periodo tonos respuesta estado (s):'*/transerv.translate('TCTRL_P02_PTON'),
                        Value: vm.tdata.telefonia.it_release,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_tel_show,
                        Val: vm.ptre_val
                    },
                    {
                        // 2 - 04
                        Name: /*'Lado:'*/transerv.translate('TCTRL_P02_LADO'),
                        Value: vm.tdata.telefonia.lado.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"Lado A"*/transerv.translate('TCTRL_P02_LADOA'), /*"Lado B"*/transerv.translate('TCTRL_P02_LADOB')],
                        Show: vm.p2_tel_show,
                        Val: vm.dval
                    },
                    {
                        // 2 - 05
                        Name: /*'Numero TEST Colateral:'*/transerv.translate('TCTRL_P02_CTES'),
                        Value: vm.tdata.telefonia.no_test_remoto,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_tel_show,
                        Val: vm.validate_ats_number
                    },
                    {
                        // 2 - 06
                        Name: /*'Numero TEST Local:'*/transerv.translate('TCTRL_P02_LTES'),
                        Value: vm.tdata.telefonia.no_test_local,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_tel_show,
                        Val: vm.validate_ats_number
                    },
                    {
                        // 2 - 07
                        Name: /*'Supervisa Colateral ?:'*/transerv.translate('TCTRL_P02_CSUP'),
                        Value: vm.tdata.telefonia.superv_options.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: /*function () { return vm.vdata[0].Value === "3" || vm.vdata[0].Value === "4" || vm.vdata[0].Value === "5"; }*/() => { return false; },
                        Val: function () { return ""; }
                    },
                    {
                        // 2 - 08
                        Name: /*'Tiempo Supervision (s):'*/transerv.translate('TCTRL_P02_TSUP'),
                        Value: /*NormalizeTmSuperv(true, vm.tdata.telefonia.tm_superv_options).toString()*/vm.tdata.telefonia.tm_superv_options,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [/*"5", "10", "15", "20", "25", "30"*/]/*[]*/,
                        Show: /*function () { return (vm.vdata[0].Value === "3" || vm.vdata[0].Value === "4" || vm.vdata[0].Value === "5") && vm.vdata[7].Value==="1"; }*/() => { return false; },
                        Val: function () { return vm.vdata[8].Value > 0 && vm.vdata[8].Value <= 20 ? "" : transerv.translate("El valor debe estar entre: ") + "1 y 20"; }
                    },
                    {
                        // 2 - 09
                        Name: /*'Deteccion Vox ?:'*/transerv.translate('TCTRL_P02_VOX'),
                        Value: vm.tdata.telefonia.detect_vox.toString(),
                        Enable: function () {
                            //return false;          // 20161213. No se puede editar. authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                            //20210301. Para REDAN se habilita la edición, y se deshabilita la opción para ULISES.
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: vm.p2_tel_show,
                        Val: vm.dval
                    },
                    {
                        // 2 - 10
                        Name: /*'Umbral Vox (dbm):'*/transerv.translate('TCTRL_P02_UVOX'),
                        Value: vm.tdata.telefonia.umbral_vox,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_tel_show,
                        Val: vm.vox_val
                    },
                    {
                        // 2 - 11
                        Name: /*'Cola Vox (s):'*/transerv.translate('TCTRL_P02_CVOX'),
                        Value: vm.tdata.telefonia.tm_inactividad,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_tel_show,
                        Val: vm.cvox_val
                    },
                    {
                        // 2 - 12
                        Name: /*'Periodo Interrupt Warning'*/transerv.translate('TCTRL_P02_IWP'),
                            /** 20210212. El Byte Bajo es Periodo. El Byte alto es el 'P6-P22' Modo Normal o Transito */
                        Value: vm.tdata.telefonia.iT_Int_Warning & 0xFF,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_tel_show,
                        Val: vm.validate_iwp
                    },
                    {
                        // 2 - 13
                        Name: /*'Id RED'*/transerv.translate('Id. RED'),
                        Value: vm.tdata.telefonia.idRed,
                        Enable: function () {
                            return false;
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_tel_show,
                        Val: ValidateService.max_long_val
                    },
                    {
                        // 2 - 14
                        Name: /*'Id Troncal'*/transerv.translate('Id. Troncal'),
                        Value: vm.tdata.telefonia.idTroncal,
                        Enable: function () {
                            return false;
                        },
                        Input: 0,
                        Inputs: [],
                        Show: vm.p2_tel_show,
                        Val: ValidateService.max_long_val
                    },
                    {
                        // 2 - 15
                        Name: transerv.translate('Tiempo maximo de llamada entrante en linea (sg)'),
                        Value: vm.tdata.telefonia.iTmLlamEntrante,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: function () { return vm.vdata[0].Value === "0" || vm.vdata[0].Value === "1" || vm.vdata[0].Value === "2"; },
                        Val: function () { return vm.vdata[15].Value > 0 && vm.vdata[15].Value <= 60 ? "" : transerv.translate("El valor debe estar entre: ") + "1 - 60"; }
                    },
                    {
                        // 2 - 16
                        Name: transerv.translate('Tiempo de deteccion de fin de llamada desde Ring Off  (sg)'),
                        Value: vm.tdata.telefonia.iTmDetFinLlamada,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: function () { return vm.vdata[0].Value === "2"; },
                        Val: function () { return vm.vdata[16].Value > 0 && vm.vdata[16].Value <= 8 ? "" : transerv.translate("El valor debe estar entre: ") + "1 - 8"; }
                    },
                    {
                        // 2 - 17
                        Name: transerv.translate('Tiempo con tonos antes de pasar a reposo (sg)'), 
                        Value: vm.tdata.telefonia.TReleaseBL,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: function () { return vm.vdata[0].Value === "0"; },
                        Val: function () { return vm.vdata[17].Value > 0 && vm.vdata[17].Value <= 10 ? "" : transerv.translate("El valor debe estar entre: ") + "1 - 10"; }
                    },
                    {
                        // 2 - 18
                        Name: transerv.translate('Detección de Caller-Id en llamadas entrantes ?'), 
                        Value: vm.tdata.telefonia.iDetCallerId.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: function () { return vm.vdata[0].Value === "2"; },
                        Val: function () { return ""; }
                    },
                    {
                        // 2 - 19
                        Name: transerv.translate('Tiempo de deteccion de CallerId (ms)'), 
                        Value: vm.tdata.telefonia.iTmCallerId,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: function () { return vm.vdata[0].Value === "2" && vm.vdata[18].Value === "1"; },
                        Val: function () { return vm.vdata[19].Value >= 1000 && vm.vdata[19].Value <= 5000 ? "" : transerv.translate("El valor debe estar entre: ") + "1000 - 5000"; }
                    },
                    {
                        // 2 - 20
                        Name: transerv.translate('Detección de Inversion de Polaridad ?'),
                        Value: vm.tdata.telefonia.iDetInversionPol.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: function () { return vm.vdata[0].Value === "2"; },
                        Val: function () { return ""; }
                    },
                    {
                        // 2 - 21
                        Name: transerv.translate('Periodo de Supervision de señal de ring (ms).'), 
                        Value: vm.tdata.telefonia.iPeriodoSpvRing,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: function () { return vm.vdata[0].Value === "2" || vm.vdata[0].Value === "0"; },
                        Val: function () { return vm.vdata[21].Value >= 50 && vm.vdata[21].Value <= 400 ? "" : transerv.translate("El valor debe estar entre: ") + "50 - 400"; }
                    },
                    {
                        // 2 - 22
                        Name: transerv.translate('Filtro de Supervision de señal de ring.'),
                        Value: vm.tdata.telefonia.iFiltroSpvRing,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [],
                        Show: function () { return vm.vdata[0].Value === "2" || vm.vdata[0].Value === "0"; },
                        Val: function () { return vm.vdata[22].Value > 0 && vm.vdata[22].Value <= 6 ? "" : transerv.translate("El valor debe estar entre: ") + "1 - 6"; }
                    },
                    {
                        // 2 - 23
                        Name: transerv.translate('Detección DTMF ?'), // todo traducir
                        Value: vm.tdata.telefonia.iDetDtmf.toString(),
                        Enable: function () {
                            return false;  // authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]),
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: function () { return vm.vdata[0].Value === "1"; },
                        Val: function () { return ""; }
                    },
                    {
                        // 2 - 24
                        Name: transerv.translate('Detección de Fallo en Línea de Abonado ?'),
                        Value: vm.tdata.telefonia.iDetLineaAB.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: function () { return vm.vdata[0].Value === "2"; },    // Solo en Lineas AB
                        Val: function () { return ""; }
                    },
                    {
                        // 2 - 25
                        Name: /*'Periodo Interrupt Warning'*/transerv.translate('Time out respuesta llamada'),
                            /* 20210212. El Byte Bajo es Periodo. El Byte alto es el 'P6-P22' Modo Normal o Transito */
                        Value: (vm.tdata.telefonia.iT_Int_Warning & 0xFF00) == 0 ? "0" : "1",
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [transerv.translate('Normal'), transerv.translate('Siempre Transito')],
                        Show: vm.p2_tel_show,
                        Val: vm.validate_iwp
                    }
                ];
                break;
            case 3:
                vm.vdata = [
                    {
                        Name: /*'Tipo de Restriccion:'*/transerv.translate('TCTRL_P03_RES'),
                        Value: vm.tdata.restriccion.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [
                            /*"Ninguna"*/transerv.translate('TCTRL_P03_RESN'),
                            /*"Lista Negra"*/transerv.translate('TCTRL_P03_RESB'),
                            /*"Lista Blanca"*/transerv.translate('TCTRL_P03_RESW')],
                        Show: vm.lbn_show,
                        Val: vm.dval
                    },
                    {
                        Name: '',
                        Value: jamp_no_sip == 1 ? CfgService.quitar_sip(vm.tdata.blanca) : vm.tdata.blanca,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: jamp_no_sip == 1 ? 4 : 2,
                        Inputs: [],
                        Show: vm.lbn_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: '',
                        Value: jamp_no_sip == 1 ? CfgService.quitar_sip(vm.tdata.negra) : vm.tdata.negra,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: jamp_no_sip == 1 ? 4 : 2,
                        Inputs: [],
                        Show: vm.lbn_show,
                        Val: vm.uri_val
                    },
                    {
                        Name: /*'Rango Abonados Origen.'*/transerv.translate('TCTRL_P03_OAB'),
                        Value: vm.tdata.restriccion,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: -1,
                        Inputs: [],
                        Show: () => {
                            return vm.AtsRangesShow(true, vm.tdata.telefonia.tipo);
                        },
                        Val: vm.dval
                    },
                    {
                        Name: ' ',
                        Value: ats2lista(vm.tdata.telefonia.ats_rangos_org),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 2,
                        Inputs: [],
                        Show: () => {
                            return vm.AtsRangesShow(true, vm.tdata.telefonia.tipo);
                        },
                        Val: vm.validate_ats_range
                    },
                    {
                        Name: /*'Rango Abonados Destino.'*/transerv.translate('TCTRL_P03_DAB'),
                        Value: vm.tdata.restriccion,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: -1,
                        Inputs: [],
                        Show: () => {
                            return vm.AtsRangesShow(false, vm.tdata.telefonia.tipo);
                        },
                        Val: vm.dval
                    },
                    {
                        Name: ' ',
                        Value: ats2lista(vm.tdata.telefonia.ats_rangos_dst),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 2,
                        Inputs: [],
                        Show: () => {
                            return vm.AtsRangesShow(false, vm.tdata.telefonia.tipo);
                        },
                        Val: vm.validate_ats_range
                    }
                ];
                break;

            case 4:                 // TODO...
                vm.vdata = [
                    {
                        Name: 'Enlaces Internos',
                        Value: vm.tdata.telefonia.listaEnlacesInternos,
                        Enable: function () {
                            return false;
                        },
                        Input: 2,
                        Inputs: [],
                        Show: function () { return true; },
                        Val: vm.dval
                    }
                ];
                break;
            case 5:                 // 20200706. Página específica para colaterales.
                vm.vdata = [
                    {
                        // 5 - 00
                        Name: /*'Tipo de Interfaz Telefonico:'*/transerv.translate('TCTRL_P00_ITF'),
                        Value: vm.tdata.telefonia.tipo.toString(),
                        Enable: function () {
                            return false;
                        },
                        Input: 1,
                        Inputs: [
                              /*"PP-BL"   */transerv.translate('TCTRL_P00_IBL'),
                              /*"PP-BC"   */transerv.translate('TCTRL_P00_IBC'),
                              /*"PP-AB"   */transerv.translate('TCTRL_P00_IAB'),
                              /*"ATS-R2"  */transerv.translate('TCTRL_P00_IR2'),
                              /*"ATS-N5"  */transerv.translate('TCTRL_P00_IN5'),
                              /*"LCEN"    */transerv.translate('TCTRL_P00_ILC'),
                              /*"ATS-QSIG"*/transerv.translate('TCTRL_P00_IQS'),
                              /*"TUN-LOC" */transerv.translate('TCTRL_P00_LTU'),
                              /*"TUN-REM" */transerv.translate('TCTRL_P00_RTU')],
                        Show: () => { return true; },
                        Val: vm.dval
                    },
                    {
                        // 5 - 01
                        Name: /*'Respuesta Automatica Simulada ?:'*/transerv.translate('TCTRL_P02_ARSP'),
                        Value: vm.tdata.telefonia.r_automatica.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: () => { return Page5AutomaticRspShow(vm.vdata[0].Value); },//Cambiar
                        Val: vm.dval
                    },
                    {
                        // 5 - 02
                        Name: /*'Tiempo Supervision (s):'*/transerv.translate('TCTRL_P02_TSUP'),
                        Value: /*NormalizeTmSuperv(true, vm.tdata.telefonia.tm_superv_options).toString()*/vm.tdata.telefonia.tm_superv_options,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 0,
                        Inputs: [/*"5", "10", "15", "20", "25", "30"*/]/*[]*/,
                        Show: () => { return Page5SupervisionTimeShow(vm.vdata[0].Value, vm.vdata[4].Value, vm.vdata[7].Value); },
                        Val: function () { return vm.vdata[2].Value > 0 && vm.vdata[2].Value <= 20 ? "" : transerv.translate("El valor debe estar entre: ") + "1 y 20"; }
                    },
                    {
                        // 5 - 03
                        Name: /*'URI Remota:'*/transerv.translate('TCTRL_P02_RURI'),
                        Value: jamp_no_sip == 1 ? CfgService.quitar_sip(vm.tdata.telefonia.uri_remota) : vm.tdata.telefonia.uri_remota,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: () => { return Page5OtherParameterShow(vm.vdata[0].Value); },
                        Val: vm.uri_or_ip_val
                    },
                    {
                        // 5-4
                        Name: /*'Supervisa Colateral ?:'*/transerv.translate('TCTRL_P02_CSUP'),
                        Value: vm.tdata.telefonia.superv_options.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"User"*/transerv.translate('Usuario'), /*"Domain"*/transerv.translate('Dominio')],
                        Show: () => { return Page5OtherParameterShow(vm.vdata[0].Value); },
                        Val: function () { return ""; }
                    },
                    {
                        // 5-5
                        Name: /*'Cualquier Respuesta Valida ?:'*/transerv.translate('Cualquier Respuesta es válida?'),
                        Value: vm.tdata.telefonia.itiporespuesta.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: () => { return Page5OtherParameterShow(vm.vdata[0].Value); },
                        Val: function () { return ""; }
                    },
                    {
                        // 5 - 6
                        Name: /*'URI Remota adicional'*/transerv.translate('Uri Remota Adicional'),
                        Value: jamp_no_sip == 1 ? CfgService.quitar_sip(vm.tdata.telefonia.additional_uri_remota) : vm.tdata.telefonia.additional_uri_remota,
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: jamp_no_sip == 1 ? 3 : 0,
                        Inputs: [],
                        Show: () => { return Page5AdditionalUriAndDataShow(vm.vdata[0].Value); },
                        Val: vm.uri_or_ip_val
                    },
                    {
                        // 5-7
                        Name: /*'Supervisa Colateral adicional ?:'*/transerv.translate('Supervisa Colateral adicional ?:'),
                        Value: vm.tdata.telefonia.additional_superv_options.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"User"*/transerv.translate('Usuario'), /*"Domain"*/transerv.translate('Dominio')],
                        Show: () => { return Page5AdditionalUriAndDataShow(vm.vdata[0].Value); },
                        Val: function () { return ""; }
                    },
                    {
                        // 5-8
                        Name: /*'Cualquier Respuesta Valida (en adicional) ?:'*/transerv.translate('Cualquier Respuesta Valida (en adicional) ?:'),
                        Value: vm.tdata.telefonia.additional_itiporespuesta.toString(),
                        Enable: function () {
                            return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
                        },
                        Input: 1,
                        Inputs: [/*"No"*/transerv.translate('TCTRL_P00_NO'), /*"Si"*/transerv.translate('TCTRL_P00_SI')],
                        Show: () => { return Page5AdditionalUriAndDataShow(vm.vdata[0].Value); },
                        Val: function () { return ""; }
                    },
                ];
                break;

            default:
                vm.vdata = [];
                break;
        }
    }

    /* */
    function v2jdata() {
        if (vm.tdata == undefined)
            return;
        switch (vm.pagina) {
            case 0:
                vm.tdata.IdRecurso = vm.vdata[0].Value;
                vm.tdata.telefonia.tipo = parseInt(vm.vdata[2].Value);
                vm.tdata.Uri_Local = jamp_no_sip == 1 ? CfgService.poner_sip(vm.vdata[3].Value) : vm.vdata[3].Value;
                vm.tdata.enableRegistro = parseInt(vm.vdata[4].Value);
                vm.tdata.szClave = vm.vdata[5].Value;
                vm.tdata.telefonia.iEnableNoED137 = parseInt(vm.vdata[6].Value);
                break;
            case 1:
                vm.tdata.Codec = parseInt(vm.vdata[0].Value);
                vm.tdata.hardware.AD_AGC = parseInt(vm.vdata[1].Value);
                vm.tdata.hardware.AD_Gain = vm.vdata[2].Value * 10;
                vm.tdata.hardware.DA_AGC = parseInt(vm.vdata[3].Value);
                vm.tdata.hardware.DA_Gain = vm.vdata[4].Value * 10;
                break;
            case 2:
                vm.tdata.telefonia.tipo = vm.vdata[0].Value;
                vm.tdata.telefonia.uri_remota = jamp_no_sip == 1 ? CfgService.poner_sip(vm.vdata[1].Value) : vm.vdata[1].Value;
                vm.tdata.telefonia.r_automatica = parseInt(vm.vdata[2].Value);
                vm.tdata.telefonia.it_release = vm.vdata[3].Value;
                vm.tdata.telefonia.lado = parseInt(vm.vdata[4].Value);
                vm.tdata.telefonia.no_test_remoto = vm.vdata[5].Value;
                vm.tdata.telefonia.no_test_local = vm.vdata[6].Value;
                vm.tdata.telefonia.superv_options = parseInt(vm.vdata[7].Value);// Supervisa Colateral ???
                vm.tdata.telefonia.tm_superv_options = parseInt(/*NormalizeTmSuperv(false, */vm.vdata[8].Value/*)*/);       // Tiempo Supervision Colateral
                vm.tdata.telefonia.detect_vox = parseInt(vm.vdata[9].Value);    // VOX en BL ???
                vm.tdata.telefonia.umbral_vox = vm.vdata[10].Value;             // Umbral VOX en BL
                vm.tdata.telefonia.tm_inactividad = vm.vdata[11].Value;         // Cola VOX
                    /** 20210212. El Byte Bajo es Periodo Interrupt Warning. El Byte alto es el 'P6-P22' Modo Normal o Transito */
                vm.tdata.telefonia.iT_Int_Warning = (vm.vdata[12].Value & 0xff) | (vm.vdata[25].Value=="0" ? 0x0000 : 0x0100);
                vm.tdata.telefonia.idRed = vm.vdata[13].Value;                  // Ulises-idRed.
                vm.tdata.telefonia.idTroncal = vm.vdata[14].Value;              // Ulises-idTroncal.

                vm.tdata.telefonia.iTmLlamEntrante = vm.vdata[15].Value;
                vm.tdata.telefonia.iTmDetFinLlamada = vm.vdata[16].Value;
                vm.tdata.telefonia.TReleaseBL = vm.vdata[17].Value;
                vm.tdata.telefonia.iDetCallerId = parseInt(vm.vdata[18].Value);
                vm.tdata.telefonia.iTmCallerId = vm.vdata[19].Value;
                vm.tdata.telefonia.iDetInversionPol = parseInt(vm.vdata[20].Value);
                vm.tdata.telefonia.iPeriodoSpvRing = vm.vdata[21].Value;
                vm.tdata.telefonia.iFiltroSpvRing = vm.vdata[22].Value;
                vm.tdata.telefonia.iDetDtmf = parseInt(vm.vdata[23].Value);
                vm.tdata.telefonia.iDetLineaAB = parseInt(vm.vdata[24].Value);

                break;
            case 3:
                vm.tdata.telefonia.ats_rangos_org = lista2ats(vm.vdata[4].Value);
                if ((vm.tdata.telefonia.tipo == 3 || vm.tdata.telefonia.tipo == 4 || vm.tdata.telefonia.tipo == 6)) {
                    //vm.tdata.telefonia.ats_rangos_org = lista2ats(vm.vdata[4].Value);
                    vm.tdata.telefonia.ats_rangos_dst = lista2ats(vm.vdata[6].Value);
                    console.log("Salvo ATS");
                }
                else {
                    vm.tdata.restriccion = parseInt(vm.vdata[0].Value);
                    vm.tdata.blanca = jamp_no_sip == 1 ? CfgService.poner_sip(vm.vdata[1].Value) : vm.vdata[1].Value;
                    vm.tdata.negra = jamp_no_sip == 1 ? CfgService.poner_sip(vm.vdata[2].Value) : vm.vdata[2].Value;
                    console.log("Salvo BN");
                }

                break;
            case 4:                     // TODO.
                break;
            case 5:     // Colaterales.
                vm.tdata.telefonia.r_automatica = parseInt(vm.vdata[1].Value);
                vm.tdata.telefonia.tm_superv_options = parseInt(vm.vdata[2].Value); // Tiempo Supervision Colateral
                vm.tdata.telefonia.uri_remota = jamp_no_sip == 1 ? CfgService.poner_sip(vm.vdata[3].Value) : vm.vdata[3].Value;
                vm.tdata.telefonia.superv_options = parseInt(vm.vdata[4].Value);// Supervisa Colateral ???
                vm.tdata.telefonia.itiporespuesta = parseInt(vm.vdata[5].Value);

                vm.tdata.telefonia.additional_uri_remota = jamp_no_sip == 1 ? CfgService.poner_sip(vm.vdata[6].Value) : vm.vdata[6].Value;
                vm.tdata.telefonia.additional_superv_options = parseInt(vm.vdata[7].Value);// Supervisa Colateral ???
                vm.tdata.telefonia.additional_itiporespuesta = parseInt(vm.vdata[8].Value);
                break;
        }
    }

    /* */
    function ats2lista(ats) {
        var lista = [];

        $.each(ats,
            function (index, value) {
                if (value.inicial == '' || value.final == '')
                    lista.push('');
                else
                    lista.push(value.inicial + '-' + value.final);
            });
        return lista;
    }

    /* */
    function lista2ats(lista) {
        var ats = [];
        $.each(lista,
            function (index, value) {
                if (value == "") {
                    ats.push({ inicial: '', final: '' });
                }
                else {
                    var match = value.match(regx_atsrango);
                    if (match) {
                        var abonados = value.split('-');
                        ats.push({ inicial: abonados[0], final: abonados[1] });
                    }
                    else {
                        console.log(/*"Error Formato Rango: "*/transerv.translate('ICTRL_MSG_01') + value);
                    }
                }
            });
        return ats;
    }

    /* */
    function validate_lbn() {
        var validate = true;
        if (vm.vdata[0].Value == 1) {       // lista negra
            angular.forEach(vm.vdata[2].Value, function (value, index) {
                if (vm.uri_val(value) != "") validate = false;
            });
        }
        else if (vm.vdata[0].Value == 2) { // lista blanca.
            angular.forEach(vm.vdata[1].Value, function (value, index) {
                if (vm.uri_val(value) != "") validate = false;
            });
        }
        return validate;
    }

    /* */
    function validate_ranges() {
        var validate = true;

        /** Los rangos origen */
        angular.forEach(vm.vdata[4].Value, function (value, index) {
            if (vm.validate_ats_range(value) != "") validate = false;
        });

        /** Los rangos destino */
        angular.forEach(vm.vdata[6].Value, function (value, index) {
            if (vm.validate_ats_range(value) != "") validate = false;
        });

        return validate;
    }

    /* */
    function validate_page() {

        if (vm.tdata == undefined)
            return true;

        switch (vm.pagina) {
            case 0:
                if (vm.vdata[3].Show(3) && ValidateService.uri_val(vm.vdata[3].Value)!="") return false;
                break;
            case 1:
                if (vm.vdata[2].Show(2) && vm.cbmad_val(vm.vdata[2].Value)!="") return false;
                if (vm.vdata[4].Show(4) && vm.cbmda_val(vm.vdata[4].Value)!="") return false;
                break;
            case 2:
                if (vm.vdata[1].Show(1) && vm.uri_val(vm.vdata[1].Value) != "") return false;
                if (vm.vdata[3].Show(3) && vm.ptre_val(vm.vdata[3].Value) != "") return false;
                if (vm.vdata[5].Show(5) && vm.validate_ats_number(vm.vdata[5].Value) != "") return false;
                if (vm.vdata[6].Show(6) && vm.validate_ats_number(vm.vdata[6].Value) != "") return false;
                // if (vm.vdata[8].Show(8) && !vm.tsup_val(vm.vdata[8].Value)) return false;
                if (vm.vdata[10].Show(10) && vm.vox_val(vm.vdata[10].Value) != "") return false;
                if (vm.vdata[11].Show(11) && vm.cvox_val(vm.vdata[11].Value) != "") return false;
                break;
            case 3:
                if ((vm.tdata.telefonia.tipo == 3 || vm.tdata.telefonia.tipo == 4 || vm.tdata.telefonia.tipo == 6)) {
                    if (!validate_ranges()) return false;
                }
                else {
                    if (!validate_lbn()) return false;
                }
                break;
        }
        return true;
    }

    /* */
    function Init() {
        vm.selected = "0";
        vm.current = -1;
        vm.pagina = 0;
    }

    /* */
    CfgService.init().then(function () {
        get_telef();
        vm.selected = $routeParams.recId == -1 ? CfgService.telef_last_get().toString() :
            CfgService.telef_index_get(Math.floor($routeParams.recId / 4), $routeParams.recId % 4).toString();
        vm.update_telef();
    });

    /* Cambio de Vista... */
    $scope.$on("$locationChangeStart", function (event) {
        if (!authservice.global_validate(validate_page())) {
            /*alert*/alertify.error(/*"Existen Errores de Formato o Rango. No puede cambiar de vista..."*/transerv.translate('ICTRL_MSG_01'));
            event.preventDefault();
        }
        else
            set_telef_data(parseInt(vm.selected));
    });

    /* Se ha pulsado el boton -aplicar- */
    $scope.$on('savecfg', function (data) {
        console.log("savecfg");
        if (!authservice.global_validate(validate_page())) {
            /*alert*/alertify.error(/*"Existen Errores de Formato o Rango. No puede salvar lo cambios..."*/transerv.translate('ICTRL_MSG_01'));
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
        get_telef();
        vm.selected = CfgService.telef_last_get().toString();
        vm.update_telef();
    });

    /* */
    $scope.$on('std_change', function (data) {
        console.log("std_change");
        $route.reload();
    });
}
