/** */
angular
	.module('Ug5kweb')
	.controller('ug5kUlisesCtrl', ug5kUlisesCtrl);

ug5kUlisesCtrl.$inject = ['$scope', '$route', 'dataservice', 'authservice', 'CfgService', 'transerv', 'MantService'];

function ug5kUlisesCtrl($scope, $route, dataservice, authservice, CfgService, transerv, MantService) {
    var vm = this;

    CfgService.opcion(9);

    /** */
    vm.pagina = 0;
    vm.opcion = 0;
    vm.traduce = transerv;

    /** */
    vm.Pagina = function (n) {
        if (authservice.check_session() == true) {
            vm.pagina = n;
            vm.opcion = 0;
        }
    };

    /** */
    vm.Opcion = function (n) {
        if (authservice.check_session() == true)
            vm.opcion = n;
    };

    /** */
    CfgService.init().then(function () {
        vm.ulises = CfgService.ulises();
        vm.pdir_ip = vm.ulises.plandireccionamientoip;
        vm.pdir_sip = vm.ulises.plandireccionamientosip;
        vm.pasg_usu = vm.ulises.planasignacionusuarios;
        vm.pasg_rec = vm.ulises.planasignacionrecursos;
        vm.ats_red = vm.ulises.planredes;
        vm.ats_trc = vm.ulises.plantroncales;
        vm.ats_num = vm.ulises.plannumeracionats;

        /** Ordenar los arrays */
        vm.pasg_usu.sort(function (a, b) {
            var x = a.idusuario.toLowerCase();
            var y = b.idusuario.toLowerCase();
            if (x < y) { return -1; }
            if (x > y) { return 1; }
            return 0;
        });
        vm.pasg_rec.sort(function (a, b) {
            var x = a.idrecurso.toLowerCase();
            var y = b.idrecurso.toLowerCase();
            if (x < y) { return -1; }
            if (x > y) { return 1; }
            return 0;
        });
        vm.ats_red.sort(function (a, b) {
            return a.prefijo > b.prefijo;
        });
        vm.ats_num.sort(function (a, b) {
            return a.centralpropia < b.centralpropia;
        });
        vm.ats_num.listarutas.sort(function (a, b) {
            return a.tiporuta > b.tiporuta;
        });
    });

    /** */
    vm.id_tipo_rec = function (tp) {
        return MantService.textoTipoItf(tp);
        //return "?" + tp.toString();
    };

    /** */
    vm.show_rutas_central = function (cen) {
        return cen.centralpropia == 0;
    };

    vm.id_central = function (cen) {
        var retorno = cen.centralpropia == 1 ? "P, " : "O, ";
        retorno += (cen.no_test == "" ? "------" : cen.no_test);
        retorno += (cen.throwswitching == 0 ? "" : ", TS");
        return retorno;
    };

    /** */
    vm.id_tipo_ruta = function (ruta) {
        return ruta == undefined ? "???" : ruta.tiporuta == 0 ? "Directa" : "Alternativa";
    };

    vm.id_red_byprefix = function (prefix) {
        var result = $.grep(vm.ats_red, function (e) { return e.prefijo == prefix; });
        return result.lenght == 0 ? "?" + prefix.toString() : result[0].idred;
    };

    vm.show_rec_red = function (red) {
        return red.listarecursos.length != 0;
    };

    vm.show_rec_troncal = function (troncal) {
        return troncal.listarecursos.length != 0;
    };

    vm.show_rangos_central = function (central) {
        return central.rangosoperador.length != 0;
    };

    vm.show_rangosp_central = function (central) {
        return central.rangosprivilegiados.length != 0;
    };

    /** Se ha pulsado el boton -aplicar- */
    $scope.$on('savecfg', function (data) {
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
    });

    /** */
    $scope.$on("$destroy", function () {
    });

    /** */
    $scope.$on('std_change', function (data) {
        console.log("std_change");
        $route.reload();
    });
}
