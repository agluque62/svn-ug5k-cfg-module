/** */
angular
    .module('Ug5kweb')
    .controller('ug5kConfCtrl', ug5kConfCtrl);

ug5kConfCtrl.$inject = ['$scope', '$route', 'dataservice', 'authservice', 'CfgService', 'transerv'];

function ug5kConfCtrl($scope, $route, dataservice, authservice, CfgService, transerv) {
    var vm = this;

    CfgService.opcion(5);

    /** */
    vm.pcfgNumber = undefined;

    /** */
    vm.preconf = [];
    vm.preconfname = /*"Introduzca nombre"*/transerv.translate('CCTRL_MSG_00');
    vm.pagina = 1;
    vm.traduce = transerv;
    /** */
    vm.Pagina = function (n) {
        if (authservice.check_session() == true)
            vm.pagina = n;
    };

    /** */
    vm.mostrar = function (boton) {
        switch (boton) {
            case 0:         // Exportar
            case 1:         // Importar
            case 2:         // Guardar
            case 4:         // Borrar
                return authservice.ProfilePermission(true, [ADMIN_PROFILE, PCFG_PROFILE]);

            case 3:         // Activar
                return authservice.global_enable([ADMIN_PROFILE, PCFG_PROFILE]);
        }
        return false;
    };

    /** */
    vm.salvar = function () {
        CfgService.aplicar_cambios();
    };

    /** */
    vm.descartar = function () {
        AltfyConfirm(authservice, /*"¿Realmente desea descartar los cambios efectuados?"*/transerv.translate('CCTRL_MSG_01'), function () {
            CfgService.restore();
        });
    };

    /** */
    vm.salvarcomo = function () {
        var existe = false;
        var listaconfs = vm.preconf.preconfs;

        if (validate_max_preconf() == false) return;


        alertify.prompt(transerv.translate('CCTRL_MSG_00')/*"Introduzca Identificador"*/, transerv.translate('CCTRL_MSG_00'),
            function (evt, value) {
                if (value == null) return;
                vm.preconfname = value;

                /** */
                for (var i = 0; i < listaconfs.length; i++) {
                    if (vm.preconfname == listaconfs[i].name) {
                /*alert*/alertify.error(/*"Ya existe una preconfiguración con el nombre:"*/transerv.translate('CCTRL_MSG_03') + listaconfs[i].name +
                    /*" con fecha: "*/transerv.translate('CCTRL_MSG_04') + listaconfs[i].date);
                        existe = true;
                    }
                }
                if ((existe == false) && ((vm.preconfname != /*"Introduzca nombre"*/transerv.translate('CCTRL_MSG_00')) && (vm.preconfname != ""))) {
                    var pre = /*"¿Quiere salvar la configuracion actual como "*/transerv.translate('CCTRL_MSG_05') + vm.preconfname + " ?";
                    AltfyConfirm(authservice, pre, function () {
                        $("body").css("cursor", "progress");
                        dataservice.act_preconf(vm.preconfname, {}).then(
                            function (res) {
                                vm.get_preconf();
                        /*alert*/alertify.success(/*"Preconfiguración "*/transerv.translate('CCTRL_MSG_06') + vm.preconfname + /*" guardada correctamente."*/transerv.translate('CCTRL_MSG_07'));
                                vm.preconfname = /*"Introduzca nombre"*/transerv.translate('CCTRL_MSG_00');
                                $("body").css("cursor", "default");
                            }
                        );
                    });
                }
                else {
            /*alert*/alertify.error(/*"Debe introducir un nombre y este debe se inexistente"*/transerv.translate('CCTRL_MSG_08'));
                    vm.preconfname = /*"Introduzca nombre"*/transerv.translate('CCTRL_MSG_00');
                }
            }).set({ 'modal': false, 'closable': false });
    };


    /** */
    vm.activar = function (name) {
        name = preconf_name_get(name);
        if (name == undefined) return;
        var pre = /*"¿Quiere activar la Preconfiguracion "*/transerv.translate('CCTRL_MSG_09') + name + " ?";
        AltfyConfirm(authservice, pre, function () {
            CfgService.restore();
            $("body").css("cursor", "progress");
            dataservice.set_preconf(name, {})
                .then(function (res) {
                    console.log("PUT: ", res);
                    CfgService.restore();
                    $("body").css("cursor", "default");
                    /*alert*/alertify.success(transerv.translate('Preconfiguracion') + ": " + name + ", " + transerv.translate(' Activada'));
                });
        });
    };

    /** */
    vm.eliminar = function (name) {
        name = preconf_name_get(name);
        if (name == undefined) return;
        var pre = /*"¿Quiere Eliminar la Preconfiguracion "*/transerv.translate('CCTRL_MSG_10') + name + " ?";
        AltfyConfirm(authservice, pre, function () {
            dataservice.del_preconf(name)
                .then(
                    function (res) {
                        console.log("DELETE: ", res);
                        vm.get_preconf();
                        vm.pcfgNumber = undefined;
                    });
        });
    };

    /** */
    vm.get_preconf = function () {

        dataservice.get_preconf().then(
            function (respuesta) {
                console.log("res:", respuesta.data);
                vm.preconf = respuesta.data;
            }
        );
    };

    /** */
    vm.import = function () {
        if (window.File && window.FileReader && window.FileList && window.Blob) {
            if (validate_max_preconf() == false) return;
            var chooser = $('#openDialog4Import');
            chooser.unbind('change');
            chooser.change(function (evt) {
                var filename = $(this).val().replace("C:\\fakepath\\", "");
                var file = evt.target.files[0];
                var reader = new FileReader();
                reader.onload = function (e) {
                    var text = reader.result;
                    try {
                        AltfyConfirm(authservice,
                            transerv.translate('CCTRL_MSG_11')/*"¿Desea Importar el fichero "*/ + filename + "?",
                            function () {
                                var obj = JSON.parse(text);
                                dataservice.set_import(filename, obj).then(function (response) {
                                    if (response.data.res === "ok") {
                                /*alert*/alertify.success(/*"Preconfiguración "*/transerv.translate('CCTRL_MSG_06') + /*" guardada correctamente."*/transerv.translate('CCTRL_MSG_07'));
                                        vm.get_preconf();
                                    }
                                    else {
                                /*alert*/alertify.error("Error: " + response.data.res);
                                    }
                                }, function (response) {
                                    console.log(response);
                            /*alert*/alertify.error(transerv.translate('CCTRL_MSG_12')/*"Error Comunicaciones. Mire Log Consola..."*/);
                                });
                            });
                    }
                    catch (e) {
                        /*alert*/alertify.error("Error: " + e);
                    }
                };
                reader.readAsText(file);
                // Reset the selected value to empty ('')
                $(this).val('');
                console.log("NAME: " + filename + ",File: " + file);
            });
            chooser.trigger('click');
        } else {
            /*alert*/alertify.error('The File APIs are not fully supported in this browser...');
        }
    };

    /** */
    vm.export = function () {
        var name = preconf_name_get(undefined);
        if (name == undefined) return;
        try {
            AltfyConfirm(authservice, transerv.translate('CCTRL_MSG_13')/*"¿Desea Exportar la preconfiguracion "*/ + name + "?", function () {
                dataservice.get_export(name).then(function (response) {
                    download(name + ".json", JSON.stringify(response.data));
                /*alert*/alertify.success(transerv.translate('CCTRL_MSG_14')/*"Preconfiguración exportada correctamente"*/);
                }, function (response) {
                    console.log(response);
                /*alert*/alertify.error(transerv.translate('CCTRL_MSG_12')/*"Error Comunicaciones. Mire Log Consola..."*/);
                });
            });
        }
        catch (error) {
            /*alert*/alertify.error("Error: " + e);
        }
    };

    /** */
    function validate_max_preconf() {
        var listaconfs = vm.preconf.preconfs;
        /** */
        if (listaconfs.length >= 8) {
            /*alert*/alertify.error(/*"Se ha alcanzado el número máximo de preconfiguraciones (8) que se pueden almacenar. Elimine alguna antes de generar una nueva."*/
                transerv.translate('CCTRL_MSG_02'));
            return false;
        }
        return true;
    }

    /** */
    function preconf_name_get(name) {
        if (name == undefined) {
            if (vm.pcfgNumber == undefined) {
                /*alert*/alertify.error(transerv.translate('CCTRL_MSG_15')/*"Seleccione una Preconfiguracion!!!"*/);
                return undefined;
            }
            name = vm.pcfgNumber.name;
        }
        return name;
    }

    /** */
    function linkDownload(a, filename, content) {
        contentType = 'data:application/octet-stream,';
        uriContent = contentType + encodeURIComponent(content);
        a.setAttribute('href', uriContent);
        a.setAttribute('download', filename);
    }

    /** */
    function download(filename, content) {
        var a = document.createElement('a');
        linkDownload(a, filename, content);
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
    }

    /** */
    function Confirma(msg) {
        if (authservice.check_session() && confirm(msg))
            return true;
        return false;
    }

    /** */
    CfgService.init().then(function () {
        vm.get_preconf();
    });

    /** Se ha pulsado el boton -aplicar- */
    $scope.$on('savecfg', function (data) {

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
        //if (CfgService.test_ip_virtual() == true ||
        //    confirm(transerv.translate('GCTRL_IPV_WARNING')) == true) {
        //    CfgService.aplicar_cambios();
        //}

    });

    /** */
    $scope.$on("$destroy", function () {
    });

    /** */
    $scope.$on('std_change', function (data) {
        console.log("std_change");
        $route.reload();
    });

    /** */
}
