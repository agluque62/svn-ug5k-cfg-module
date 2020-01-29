/** */
angular
    .module('Ug5kweb')
    .controller('ug5kServCtrl', ug5kServCtrl);

ug5kServCtrl.$inject = ['$scope', '$route', 'authservice', 'CfgService', 'ValidateService', 'transerv', 'MantService'];

function ug5kServCtrl($scope, $route, authservice, CfgService, ValidateService, transerv, MantService) {
    var vm = this;
    var url = '/servicios';

    /** */
    vm.jserv = {};
    vm.index = 0;
    vm.data;
    vm.autosave = data2j;
    vm.def_show = function () { return true; }
    vm.noshow = function () { return false; };
    vm.v2commshow = function () { return vm.data[3].Value == 1 ? true : false; }

    CfgService.opcion(4);

    /** */
    vm.validate_trap = function (value) {
        return value == "" ? true : value.match(regx_trpval) != null ? "" : transerv.translate("Error Formato TRAP");
    };

    /** */
    vm.validate_sup_sip = function (value) {
        return (value >= 90 && value <= 1800) ? "" : transerv.translate("El valor debe estar entre: ") + "90 - 1800";
    };

    /** */
    vm.set_index = function (index) {
        if (!authservice.global_validate(validate_page())) {
            /*alert*/alertify.error(/*"Existen Errores de Formato o Rango. No puede cambiar de vista..."*/transerv.translate('ICTRL_MSG_01'));
        }
        else if (authservice.check_session() == true) {
            post_data();
            vm.index = index;
            j2data();
        }
    };

    /** */
    vm.restore = function () {
        CfgService.restore().then(function () {
            get_data();
        });
    };

    /** Get Data */
    function get_data() {
        vm.jserv = CfgService.ser_data_get();
        j2data();
    };

    /** Post Data. */
    function post_data() {
        if (data2j() == false)
            return;
        CfgService.ser_data_set(vm.jserv);
    }

    /** */
    function j2data() {
        switch (vm.index) {
            case 0:	// SIP
                vm.data = [
                    {
                        Name: /*'Puerto'*/transerv.translate('SCTRL_P00_PORT'),
                        Value: vm.jserv.sip.PuertoLocalSIP,
                        Enable: false,
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'Keep Alive Period:'*/transerv.translate('SCTRL_P00_KAP'),
                        Value: vm.jserv.sip.KeepAlivePeriod,
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.noshow,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'Keep Alive Mult.:'*/transerv.translate('SCTRL_P00_KAM'),
                        Value: vm.jserv.sip.KeepAliveMultiplier,
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.noshow,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'Periodo Supervision:'*/transerv.translate('SCTRL_P00_SPT'),
                        Value: vm.jserv.sip.PeriodoSupervisionSIP,
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: vm.validate_sup_sip
                    },
                    {
                        Name: /*'Proxy:'*/transerv.translate('SCTRL_P00_PRX'),
                        Value: serv2iplist(vm.jserv.sip.proxys),
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 2,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.ip_val
                    },
                    {
                        Name: /*'Registrar:'*/transerv.translate('SCTRL_P00_REG'),
                        Value: serv2iplist(vm.jserv.sip.registrars),
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 2,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.ip_val
                    }
                ];
                break;
            case 1:	// Sincronizacion
                vm.data = [
                    {
                        Name: /*'Tipo:'*/transerv.translate('SCTRL_P01_TP'),
                        Value: vm.jserv.sincr.ntp.toString(),
                        Enable: false,
                        Input: 1,
                        Inputs: [
                            /*"NADA"*/transerv.translate('SCTRL_P01_NONE'),
                            /*"OTRO"*/transerv.translate('SCTRL_P01_OTH'),
                            /*"NTP"*/transerv.translate('SCTRL_P01_NTP')],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'Servidor NTP:'*/transerv.translate('SCTRL_P01_NTPS'),
                        Value: serv2iplist(vm.jserv.sincr.servidores),
                        Enable: authservice.global_enable([ADMIN_PROFILE, ALM1_PROFILE]),
                        Input: 2,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.ip_val
                    },
                    {
                        Name: /*'Servidor NTP Default'*/transerv.translate('SCTRL_P01_NTPSD'),
                        Value: CfgService.inicio_data_get().ips.split(":")[0],
                        Enable: false,
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    }
                ];
                break;
            case 2:	// SNMP
                vm.data = [
                    {
                        Name: /*'Puerto Servicio:'*/transerv.translate('SCTRL_P02_PORT'),
                        Value: vm.jserv.snmp.sport,
                        Enable: false,
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'Puerto SNMP:'*/transerv.translate('SCTRL_P02_SPORT'),
                        Value: vm.jserv.snmp.snmpp,
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'Destinos TRAPS:'*/transerv.translate('SCTRL_P02_TRAP'),
                        Value: vm.jserv.snmp.traps,
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 2,
                        Inputs: [],
                        Show: vm.def_show, Val: vm.validate_trap
                    },
                    {
                        Name: /*'SNMP-V2?:'*/transerv.translate('SCTRL_P02_SV2'),
                        Value: vm.jserv.snmp.agv2.toString(),
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 1,
                        Inputs: [
                            /*"No"*/transerv.translate('SCTRL_P00_NO'),
                            /*"Si"*/transerv.translate('SCTRL_P00_SI')],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'SNMP-V2-COMMUNITY:'*/transerv.translate('SCTRL_P02_SV2C'),
                        Value: vm.jserv.snmp.agcomm,
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.v2commshow,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'SNMP-NAME:'*/transerv.translate('SCTRL_P02_NAM'),
                        Value: vm.jserv.snmp.agname,
                        Enable: false,
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'SNMP-LOCATION:'*/transerv.translate('SCTRL_P02_LOC'),
                        Value: vm.jserv.snmp.agloc,
                        Enable: false,
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'SNMP-CONTACT:'*/transerv.translate('SCTRL_P02_CON'),
                        Value: vm.jserv.snmp.agcont,
                        Enable: false,
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    }
                ];
                break;
            case 3:	// WEB
                vm.data = [
                    {
                        Name: /*'Puerto Servicio:'*/transerv.translate('SCTRL_P03_PORT'),
                        Value: vm.jserv.web.wport,
                        Enable: false,
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'Tiempo de Session (seg):'*/transerv.translate('SCTRL_P03_STIME'),
                        Value: vm.jserv.web.stime,
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    }
                ];
                break;
            case 4:	// Grabador
                vm.data = [
                    {
                        Name: /*'Puerto Servicio:'*/transerv.translate('SCTRL_P04_PORT'),
                        Value: vm.jserv.grab.sport,
                        Enable: false,
                        Input: 0,
                        Inputs: [],
                        Show: vm.noshow,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'RTSP-URI:'*/transerv.translate('SCTRL_P04_URI'),
                        Value: vm.jserv.grab.rtsp_uri,
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.noshow,
                        Val: ValidateService.def_val
                    },
                    {
                        Name: /*'RTSP-IP:'*/transerv.translate('SCTRL_P04_IP'),
                        Value: vm.jserv.grab.rtsp_ip,
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.ip_val
                    },
                    {
                        Name: /*'RTSP-IP:'*/transerv.translate('SCTRL_P04_IPB'),
                        Value: vm.jserv.grab.rtspb_ip,
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show/*MantService.is_ulises*/,
                        Val: ValidateService.ip_val
                    },
                    {
                        Name: /*'PUERTO RTSP:'*/transerv.translate('SCTRL_P04_RPORT'),
                        Value: vm.jserv.grab.rtsp_port,
                        Enable: authservice.global_enable([ADMIN_PROFILE]),
                        Input: 0,
                        Inputs: [],
                        Show: vm.def_show,
                        Val: ValidateService.def_val
                    }
                    //{ Name: /*'TRAMAS RTP?:'*/transerv.translate('SCTRL_P04_RTP'), Value: vm.jserv.grab.rtsp_rtp, Enable: true, Input: 1, Inputs: ["No", "Si"], Show: vm.def_show, Val: ValidateService.def_val },
                    //{ Name: /*'RTP PAYLOAD:'*/transerv.translate('SCTRL_P04_PAY'), Value: vm.jserv.grab.rtp_pl, Enable: false, Input: 0, Inputs: [], Show: vm.def_show, Val: ValidateService.def_val },
                    //{ Name: /*'RTP RATE:'*/transerv.translate('SCTRL_P04_RES'), Value: vm.jserv.grab.rtp_sr, Enable: false, Input: 0, Inputs: [], Show: vm.def_show, Val: ValidateService.def_val }
                ];
                break;
            default:
                vm.data = [];
        }
    }

    /** */
    function data2j() {
        switch (vm.index) {
            case 0: // SIP...
                vm.jserv.sip.PuertoLocalSIP = vm.data[0].Value;
                vm.jserv.sip.KeepAlivePeriod = vm.data[1].Value;
                vm.jserv.sip.KeepAliveMultiplier = vm.data[2].Value;
                vm.jserv.sip.PeriodoSupervisionSIP = vm.data[3].Value;
                vm.jserv.sip.proxys = iplist2serv(vm.data[4].Value);
                vm.jserv.sip.registrars = iplist2serv(vm.data[5].Value);
                break;
            case 1: // NTP...
                vm.jserv.sincr.ntp = parseInt(vm.data[0].Value);
                vm.jserv.sincr.servidores = iplist2serv(vm.data[1].Value);
                break;
            case 2: // SNMP...
                vm.jserv.snmp.sport = vm.data[0].Value;
                vm.jserv.snmp.snmpp = vm.data[1].Value;
                vm.jserv.snmp.traps = vm.data[2].Value;
                vm.jserv.snmp.agv2 = parseInt(vm.data[3].Value);
                vm.jserv.snmp.agcomm = vm.data[4].Value;
                vm.jserv.snmp.agname = vm.data[5].Value;
                vm.jserv.snmp.agloc = vm.data[6].Value;
                vm.jserv.snmp.agcont = vm.data[7].Value;
                break;
            case 3: // WEB...
                vm.jserv.web.wport = vm.data[0].Value;
                vm.jserv.web.stime = vm.data[1].Value;
                break;
            case 4: // Grabador
                vm.jserv.grab.sport = vm.data[0].Value;
                vm.jserv.grab.rtsp_uri = vm.data[1].Value;
                vm.jserv.grab.rtsp_ip = vm.data[2].Value;
                vm.jserv.grab.rtspb_ip = vm.data[3].Value;
                vm.jserv.grab.rtsp_port = vm.data[4].Value;
                //vm.jserv.grab.rtsp_rtp = vm.data[4].Value;
                //vm.jserv.grab.rtp_pl = vm.data[5].Value;
                //vm.jserv.grab.rtp_sr = vm.data[6].Value;
                break;
        }
        return true;
    }


    /** */
    function validate_page() {
        var validate = true;
        switch (vm.index) {
            case 0:
                if (vm.validate_sup_sip(vm.data[3].Value) != "") validate = false;
                /** PROXY */
                $.each(vm.data[4].Value, function (index, value) {
                    if (ValidateService.ip_val(value) != "") validate = false;
                });
                /** REGISTRAR */
                $.each(vm.data[5].Value, function (index, value) {
                    if (ValidateService.ip_val(value) != "") validate = false;
                });
                break;
            case 1:
                /** NTP SERVER */
                $.each(vm.data[1].Value, function (index, value) {
                    if (ValidateService.ip_val(value) != "") validate = false;
                });
                break;
            case 2:
                $.each(vm.data[2].Value, function (index, value) {
                    if (vm.validate_trap(value) != "") validate = false;
                });
                break;
            case 4:
                if (ValidateService.ip_val(vm.data[2].Value) != "") validate = false;
                break;
        }

        return validate;
    }

    /**  */
    function serv2iplist(lserv) {
        var lista = [];

        $.each(lserv,
            function (index, value) {
                lista.push(value.ip);
            }
        );

        return lista;
    }

    /**  */
    function iplist2serv(iplist) {
        var lserv = [];

        $.each(iplist,
            function (index, value) {
                lserv.push({ ip: value, selected: 0 });
            }
        );

        return lserv;
    }

    /** */
    function Error(msg) {
        /*alert*/alertify.error("Error: " + msg);
        return false;
    }

    /** */
    CfgService.init().then(function () {
        get_data();
        vm.set_index(0);
        //Validate();
    });

    /** Cambio de Vista... */
    $scope.$on("$locationChangeStart", function (event) {
        if (!authservice.global_validate(validate_page())) {
            /*alert*/alertify.error(/*"Existen Errores de Formato o Rango. No puede cambiar de vista..."*/transerv.translate('ICTRL_MSG_01'));
            event.preventDefault();
        }
        else
            post_data();
    });

    /** Se ha pulsado el boton -aplicar- */
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

    /** */
    $scope.$on('loadcfg', function (data) {
        get_data();
    });

    /** */
    $scope.$on('std_change', function (data) {
        console.log("std_change");
        $route.reload();
    });
}
