/** */
angular
    .module('Ug5kweb')
    .controller('ug5kGlobalCtrl', ug5kGlobalCtrl);

ug5kGlobalCtrl.$inject = ['$scope', '$rootScope', '$interval', '$translate', 'dataservice', 'authservice', 'CfgService', 'MantService', 'transerv'];

function ug5kGlobalCtrl($scope, $rootScope, $interval, $translate, dataservice, authservice, CfgService, MantService, transerv) {

    var vm = this;

    vm.opcion = 0;
    /** Datos del Modelo Global. */
    vm.date = GetDate();
    vm.user = GetUser();
    vm.logout = function () { Logout(); };
    vm.enable = function (nivel) { return enable(nivel); };
    vm.select = function (path) { return is_select(path); };
    vm.hora = GetHora();

    vm.idsoftware = "UG5K-REDAN";
    vm.version = "1.0.1";

    vm.session = true;

    vm.timer = sesion_sup - 1;                          // Para acelerar el arranque.
    vm.gwid = "TIFX26";
    vm.cfg_act = "23,10/11/2015 11:55:11";
    vm.lcfg = {};

    vm.isDataHasChanges = false;

    /** */
    vm.set_opcion = function (opcion) {
        vm.opcion = CfgService.opcion(opcion);
    };

    /** */
    vm.get_opcion = function () {
        return CfgService.opcion();       // vm.opcion;
    };

    /** */
    vm.mostrar_opcion = function (opcion) {
        switch (opcion) {
            case 0:     // Generales
            case 1:     // Hardware
            case 2:     // Radio
            case 3:     // Telefonia
            case 4:
            case 5:
            case 6:
                return true;
            case 7:     // Aplicar Cambios.
                return (authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE, ALM1_PROFILE]));
            case 8:     // Descartar Cambios.
                return (authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE, ALM1_PROFILE]));
            case 9:     // Tablas de ULISES.
                return !MantService.hide_on_ulises();
        }
        return false;
    };

    /** */
    vm.user_str = function () {
        var profile = authservice.profile();
        var str_profile = " (" + profile + ")";
        if (profile == 128)
            str_profile = " (rt)";
        else {
            if (profiles_multiples == false) {
                switch (profile) {
                    case ADMIN_PROFILE:
                        str_profile = " (adm)";
                        break;
                    case ING_PROFILE:
                        str_profile = " (ing)";
                        break;
                    case GEST_PROFILE:
                        str_profile = " (ges)";
                        break;
                    case CRTL_PROFILE:
                        str_profile = " (ctr)";
                        break;
                    case ALM1_PROFILE:
                        str_profile = " (al1)";
                        break;
                    case ALM2_PROFILE:
                        str_profile = " (al2)";
                        break;
                    case VIS_PROFILE:
                        str_profile = " (vis)";
                        break;
                    case PCFG_PROFILE:
                        str_profile = " (cfg)";
                        break;
                }
            }
            else {
                var txt = "";

                txt += ((profile & ADMIN_PROFILE) != 0) ? "a" : "";
                txt += ((profile & PCFG_PROFILE) != 0) ? "c" : "";
                txt += ((profile & ING_PROFILE) != 0) ? "*" : "";
                txt += ((profile & GEST_PROFILE) != 0) ? "*" : "";
                txt += ((profile & CRTL_PROFILE) != 0) ? "m" : "";
                txt += ((profile & ALM1_PROFILE) != 0) ? "i" : "";
                txt += ((profile & ALM2_PROFILE) != 0) ? "*" : "";
                txt += ((profile & VIS_PROFILE) != 0) ? "v" : "";
                str_profile = " (" + txt + ")";
            }
        }

        var str_user = vm.user.length <= 8 ? vm.user : vm.user.substr(0, 8) + "...";
        return str_user + str_profile;
    };

    /** */
    vm.software_load = function () {
        // ngDialog.open({ template: './app/templates/ug5kweb-swload-templ.html'/*, className: 'ngdialog-theme-default'*/});
    };

    /** */
    vm.software_load_send = function () {
        var file = $scope.myFile;
        console.log('file is ' + JSON.stringify(file));
        var uploadUrl = "/sw_upload";
        dataservice.send_file(uploadUrl, file).then(function () { /*alert*/alertify.success(/*vm.gctrl_sent*/transerv.translate('GCTRL_SENT')); });
    };

    /** */
    vm.restore = function () {
        AltfyConfirm(authservice, transerv.translate('GCTRL_QUESTION'), function () {
            CfgService.restore();
        });
    };

    /** */
    vm.save = function () {
        $rootScope.$broadcast('savecfg', [1, 2, 3]);
    };

    /** */
    vm.change = function () {
        console.log("change...");
    };

    /** */
    vm.gw_id = function () {
        var gen = CfgService.inicio_data_get();
        var strid = gen == null ? "" : gen.name;
        var strntp = vm.lcfg.ntpsync == 1 ? "S" : "N";
        var stridc = "0";   // TODO.

        //strid += vm.lcfg.ntpsync == 1 ? " (Sync)" : " (No Sync: " + vm.lcfg.localdate + ")";
        //return strid;
        //return "ID: " + strid + ", NTP: " + strntp + ", IC: " + stridc + ", LT: " + vm.lcfg.localdate;
        return strid;
    };

    /** */
    vm.gw_ntp = function () {
        return "NTP: " + (vm.lcfg.ntpsync == 1 ? "S" : "N");
    };

    /** */
    vm.gw_lt = function () {
        return vm.lcfg.localdate
    };

    /** */
    vm.gw_ic = function () {
        var modo = MantService.modo() == "ul" ? 1 : 0;
        return "IC: " + CfgService.indice_carga(modo).toString();
    };

    /** */
    vm.gw_modo = function () {
        var modo = MantService.modo();
        var modo_redan = MantService.modo_redan();
        var vmodo = modo == "ul" ? " ULISES" : modo_redan == "0" ? " REDAN config-WARNING" : " REDAN";
        vmodo += (force_rdaudio_normal == 1 ? " 16R" : "");
        return "V: " + vm.version + vmodo;
    };

    /** **/
    function ponerCero(digito) {
        if ((digito >= 0) && (digito <= 9)) {
            digito = '0' + digito;
        }
        return digito;
    }

    /** */
    function GetDate() {
        return (new Date()).toLocaleDateString();
    }

    /** */
    function GetHora() {
        return (new Date()).toLocaleTimeString();
    }

    /** */
    function GetUser() {
        var match = document.cookie.match(new RegExp('ssid' + '=([^;]+)'));
        if (match) {
            return match[1].split('|')[0];
        }
        return "????";
    }

    /** */
    function Logout() {
        dataservice.logout().then(
            function (respuesta) {
                console.log("POST: ", respuesta);
                document.cookie = "ssid=; expires=Thu, 01 Jan 1970 00:00:00 UTC";
                window.location.href = "/login.html";
            }
        );
    }

    /** */
    function enable(nivel) {
        var ret = authservice.profile() >= nivel ? 1 : 0;
        return ret;
    }

    /** */
    function is_select(path) {
        return path == $location.path() ? 1 : 0;
    }

    /** */
    //function gw_id() {
    //    var gen = CfgService.inicio_data_get();
    //    var strid = gen == null ? "" : gen.name;
    //    strid += vm.lcfg.ntpsync == 1 ? " (Sync)" : " (No Sync: " + vm.lcfg.localdate + ")";
    //    return strid;
    //}

    /** */
    function gw_cfg() {
        var cfg = vm.lcfg.std == -1 ? /*vm.gctrl_sync_1 */ transerv.translate('GCTRL_SYNC_1') :
            vm.lcfg.std == -2 ? /*vm.gctrl_sync_2 */ transerv.translate('GCTRL_SYNC_2') :
                vm.lcfg.std == -3 ? /*vm.gctrl_sync_3 */ transerv.translate('GCTRL_SYNC_3') + "<CFG: " + vm.lcfg.idc + ">" :
                    vm.lcfg.std == -4 ? /*vm.gctrl_sync_4 */ transerv.translate('GCTRL_SYNC_4') :
                        vm.lcfg.std == -5 ? /*vm.gctrl_sync_5 */ transerv.translate('GCTRL_SYNC_5') : vm.lcfg.idc;

        /** 20170306. Se quita el TS porque 'puede inducir a error/duda' */
        return cfg /*+ " ( " + vm.lcfg.tim + " )"*/;
    }

    /** */
    CfgService.init().then(function () {
        // vm.gwid = gw_id();
        vm.cfg_act = gw_cfg();
        vm.last_config = JSON.stringify(CfgService.cfg_get());
    });

    /** 
        Cada 30 segundos:
            Supervisa si la session ha expirado localmente.. 
            Mantiene un TICK con el Servidor para que no expire la session.
    */
    var sessionTimer = $interval(function () {

        if (++vm.timer >= sesion_sup) {
            // console.log("SessionTimer TICK.");

            //if (vm.session == true && authservice.check_session() == false) {
            //    console.log("Session Expired");
            //    dataservice.logout();
            //    vm.session = false;
            //}

            dataservice.get_tses().then(
                function (response) {               // OK.
                    console.log(response);
                    var data = response.data;

                    if (response.status == 200 && (typeof data) == 'object') {
                        console.log(data/*.lconfig*/);

                        var ant = vm.lcfg;
                        vm.lcfg = data/*.lconfig*/;
                        vm.version = data.ver;

                        if (ant.tim != data/*.lconfig*/.tim) {

                            CfgService.restore().then(function () {
                                // vm.gwid = gw_id();
                                vm.cfg_act = gw_cfg();
                            });
                        }

                        MantService.modo(data.modo);
                        MantService.modo_redan(data.modo_redan);

                        if (data.std != MantService.global_estado()) {
                            $scope.$broadcast('std_change', [1, 2, 3]);
                        }

                        MantService.global_estado(data.std);
                        MantService.ntpsync(data.ntpsync);

                        force_rdaudio_normal = data.modo_16r == 1 ? true : false;
                    }
                    else {
                        vm.timer = 0;
                        if (session_supervision == true)
                            window.open(routeForLogin, "_self");
                    }
                },
                function () {                 // Error...
                    vm.timer = 0;
                    if (session_supervision == true)
                        window.open(routeForLogin, "_self");
                }
            );
            vm.timer = 0;
            vm.date = GetDate();
        }
        // vm.gwid = gw_id();
        vm.cfg_act = gw_cfg();

        /** Supervisar Cambios */
        //if (vm.isDataHasChanges == false) {
        //    var cfg = JSON.stringify(CfgService.cfg_get());
        //    if (cfg != vm.last_config) {
        //        vm.isDataHasChanges = true;
        //    }
        //}
        /** Simulador de Carga Upload.. */
        if (carga_upload_config == true) {
            if (carga_upload_config_timer == 0) {
                carga_upload_config_timer = Math.floor((Math.random() * (carga_upload_config_timer_max - carga_upload_config_timer_min)) + carga_upload_config_timer_min);
                console.log('Peticion Upload en ' + carga_upload_config_timer + ' segundos');
            }
            else {
                --carga_upload_config_timer;
                if (carga_upload_config_timer == 0) {
                    console.log('Peticion Upload...');
                    CfgService.aplicar_cambios_carga();
                }
            }
        }
        vm.hora = GetHora();
    }, 1000);

    /** Finalizacion */
    $scope.$on("$destroy", function () {
        $interval.cancel(sessionTimer);
    });

    $scope.$on("$viewContentLoaded", function () {

        alertify.defaults.transition = 'zoom';
        alertify.defaults.glossary = {
            title: transerv.translate("ULISES G 5000. Servicio de Configuracion Local"),
            ok: transerv.translate("Si"),
            cancel: transerv.translate("No")
        };
        alertify.defaults.theme = {
            // class name attached to prompt dialog input textbox.
            input: 'btn',
            // class name attached to ok button
            ok: 'btn btn-success',
            // class name attached to cancel button 
            cancel: 'btn btn-danger'
        };
    });
}
