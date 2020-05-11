/** */
var Simulate = location.port == 8888;
var Desarrollando = false;
var session_supervision = !Simulate;
var altfy = true;
var force_rdaudio_normal = false;

var ug5kApp = angular.module('Ug5kweb', ['ngRoute', 'ui.bootstrap', 'smart-table', 'pascalprecht.translate']);

/** */
ug5kApp.config(function ($translateProvider) {
    // Our translations will go in here
    $translateProvider.useStaticFilesLoader({
        prefix: './languages/',
        suffix: '.json'
    });
    var userLang = navigator.languages[0];

    if (userLang.indexOf("en") == 0)
        $translateProvider.use('en_US');
    else if (userLang.indexOf("fr") == 0)
        $translateProvider.use('fr_FR');
    else
        $translateProvider.use('es_ES');
});

/** */
ug5kApp.directive('fileModel', ['$parse', function ($parse) {
    return {
        restrict: 'A',
        link: function (scope, element, attrs) {
            var model = $parse(attrs.fileModel);
            var modelSetter = model.assign;
            element.bind('change', function () {
                scope.$apply(function () {
                    modelSetter(scope, element[0].files[0]);
                });
            });
        }
    };
}]);

/** */
 ug5kApp.directive('bsPopover', function () {
     return function (scope, element, attrs) {
         element.popover({ placement: 'top', html: 'true' });
     };
 });

/** Variables Globales */
var pollingTime = 5000;
var maxPreconf = 8;
var InciPoll = 10;
var profiles_multiples = true;

/** Perfiles */
var PCFG_PROFILE  = 0x1000;
var ADMIN_PROFILE = 0x0040;
var ING_PROFILE   = 0x0020;
var GEST_PROFILE  = 0x0010;
var ALM1_PROFILE  = 0x0008;
var ALM2_PROFILE  = 0x0004;
var CRTL_PROFILE  = 0x0002;
var VIS_PROFILE   = 0x0001;
var PUERTA_ATRAS = (PCFG_PROFILE | ADMIN_PROFILE | CRTL_PROFILE | VIS_PROFILE); // 0x1043;

/** Rangos Recursos Radio */
var rr_ad_rng = {
    min: -13.5, max: 1.20, toString: function () { return " min: -13.5, max: 1.2 "; }
};
var rr_da_rng = {
    min: -24.3, max: 1.10, toString: function () { return " min: -24.3, max: 1.1 "; }
};
var rr_vad_rng = {
    min: -35, max: -15, toString: function () { return " min: -35, max: -15 "; }
};
var rr_bssw_rng = {
    min: 10, max: 5000, toString: function () { return " min: 10, max: 5000 "; }
};
var rr_clxt_rng = {
    min: 0, max: 250, toString: function () { return " min: 0, max: 250 "; }
};

/** Rangos Recursos Telefonia */
var pr_ad_rng = {
    min: -13.5, max: 1.20, toString: function () { return " min: -13.5, max: 1.20 "; }
};
var pr_da_rng = {
    min: -24.3, max: 1.10, toString: function () { return " min: -24.3, max: 1.10 "; }
};
var pr_vad_rng = {
    min: -35, max: -15, toString: function () { return " min: -35, max: -15 "; }
};
var pr_vxq_rng = {
    min: 0, max: 30, toString: function () { return " min: 0, max: 30 "; }
};
var pr_ptre_rng = {
    min: 0, max: 10, toString: function () { return " min: 0, max: 10 "; }
};
var pr_tsup_rng = {
    min: 90, max: 180, toString: function () { return " min: 90, max: 180 "; }
};
var pr_iwp_rng = {
    min: 5, max: 15, toString: function () { return " min: 5, max: 15 "; }
};


/** */
var carga_upload_config = false;
var carga_upload_config_timer = 0;              // Temporizador Aleatorio en Segundos...
var carga_upload_config_timer_max = 60;         // Máximo del Temporizador.
var carga_upload_config_timer_min = 20;         // Minimo del Temporizador.


/** */
var sesion_sup = 10;

/** */
var rest_url_logout = '/logout';
var rest_url_tses = '/tses';
var rest_url_config = '/config';
var rest_url_upconfig = '/uploadcfg';
var rest_url_preconfig = '/preconf';
var rest_url_impexp = '/impexp';
var rest_url_mnt_std = '/mant/std';
var rest_url_mnt_reset = '/mant/reset';
var rest_url_mnt_bite = "/mant/bite";
var rest_url_mnt_versiones = '/mant/lver';
var rest_url_mnt_version = '/mant/ver';
var rest_url_mnt_filelogs = '/mant/logs';
var rest_url_dev_hist = '/dev/hist';
var routeForUnauthorizedAccess = '/noaut';
var routeForLogin = '/login.html';

/** */
var jamp_no_sip = 1;
/** */
var max_id_length = 32;

/** Validadores. */
var regx_ipval = /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$/;
var regx_trpval = /^[1-2]+,(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\/[0-9]{2,5}$/;
var regx_atsrango = /^([2-3][0-9]{5})-([2-3][0-9]{5})$/;
var regx_atsnumber = /^([2-3][0-9]{5})$/;
var regx_urlval = /^(http(?:s)?\:\/\/[a-zA-Z0-9]+(?:(?:\.|\-)[a-zA-Z0-9]+)+(?:\:\d+)?(?:\/[\w\-]+)*(?:\/?|\/\w+\.[a-zA-Z]{2,4}(?:\?[\w]+\=[\w\-]+)?)?(?:\&[\w]+\=[\w\-]+)*)$/;
var regx_ipportval = /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])(:([\d]{1,5}))?$/;
var regx_urival = /^sip:([^:]+)@(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])(:[\d]{1,5})?$/;
var regx_urival_nosip = /^([^:]+)@((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))(:([\d]{1,5}))?$/;
var regx_urirtspval =/^rtsp:(.+)@((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))(:([\d]{1,5}))?$/;
var regx_idval = /^[a-zA-Z0-9\-_\.]{1,32}$/;
//var regx_fid = /^(1|2|3)[0-9]{2}\.[0-9]{2}(0|5)$/;
var regx_hf = /^((00[3-9])|(0[1|2][0-9])).[0-9]{2}[0|5]$/;                  /** 003.000 - 029.995 */
var regx_aereal_vhf = /^((11[8|9])|(12[0-9])|(13[0-7])).[0-9]{2}[0|5]$/;    /** 118.000 - 137.995 */
var regx_aereal_uhf = /^([2-3][0-9][0-9]).[0-9]{2}[0|5]$/;                      /** 200.000 - 399.995 */
var regx_fid = /(0[1|2|3][0-9].[0-9]{2}[0|5])|(1[1|2|3][0-9].[0-9]{2}[0|5])|(4[1|2|3][0-9].[0-9]{2}[0|5])/;

function AltfyConfirm(authservice, msg, sifunc, nofunc) {
    if (authservice.check_session() == false)
        return;
    alertify.confirm(msg, sifunc, nofunc).set({ 'modal': false, 'closable': false });
}
