/** */
angular
    .module('Ug5kweb')
    .factory('ValidateService', ValidateService);

ValidateService.$inject = ['$q', 'transerv'];

/** */
function ValidateService($q, transerv) {

    /** */
    function local_max_length_val(value, maximo) {
        if (maximo === undefined)
            maximo = max_id_length;

        if (typeof value === "string") {
            return value.length > maximo ? false : true;
        }
        return true;
    }

    /** */
    function local_id_val(value) {
        if (local_max_length_val(value) == false)
            return false;
        if (value != "" && value.match(regx_idval) == null)
            return false;
        return true;
    }

    /** */
    function local_fid_val(fid) {
        if (fid.match(regx_hf) != null || fid.match(regx_aereal_vhf) != null || fid.match(regx_aereal_uhf) != null)
            return true;
        return false;
    }

    /** */
    return {
        def_val: function () {
            return "";
        }
        , ip_val: function (value) {
            if (value != "" && value.match(regx_ipval) == null) {
                return transerv.translate("Formato IP Incorrecto");
            }
            return "";
        }
        , url_val: function (value) {
            if (value != "") {
                var fields = value.match(regx_ipportval);
                if (!fields)
                    return transerv.translate("Formato Endpoint Incorrecto");
                var port = fields[5] != undefined ? parseInt(fields[5]) : -1;
                if (port >= 65535)
                    return transerv.translate("Formato de Puerto Incorrecto");
            }
            return "";
        }
        , uri_val: function (value) {
            if (value != "")
            {
                var fields = value.match(jamp_no_sip == 1 ? regx_urival_nosip : regx_urival);
                if (fields) {
                    if (local_max_length_val(fields[1], max_id_length) === false) {
                        return transerv.translate("Longitud Usuario Excedida");
                    }
                    if (local_id_val(fields[1]) === false) {
                        return transerv.translate("Caracteres No permitidos en Usuario");
                    }
                    var port = fields[7] != undefined ? parseInt(fields[7]) : -1;
                    if (port >= 65535)
                        return transerv.translate("Formato de Puerto Incorrecto");
                    return "";
                }
                return transerv.translate("Formato Uri Incorrecto");
            }
            return transerv.translate("El Campo no puede estar vacio");
        }
        , uri_rtsp_val: function (value) {
            if (value != "") {
                var fields = value.match(regx_urirtspval);
                if (fields) {
                    if (local_max_length_val(fields[1], max_id_length) === false) {
                        return transerv.translate("Longitud Usuario Excedida");
                    }
                    if (local_id_val(fields[1]) === false) {
                        return transerv.translate("Caracteres No permitidos en Usuario");
                    }
                    var port = fields[7] != undefined ? parseInt(fields[7]) : -1;
                    if (port >= 65535)
                        return transerv.translate("Formato de Puerto Incorrecto");
                    return "";
                }
                return transerv.translate("Formato RTSP Incorrecto");
            }
            return "";
        }
        , max_long_val: function (value) {
            if (local_max_length_val(value) == false)
                return transerv.translate("Longitud de Identificador Excedida");
            if (value != "" && value.match(regx_idval) == null)
                return transerv.translate("Caracteres No permitidos en Identificador");
            return "";
        }
        , fid_val: function (fid) {
            if (local_fid_val(fid) == false)
                return transerv.translate("FID No Valido. Valores admitidos: " + "HF (003.000, 029.995) VHF (118.000, 137.995) UHF (200.000, 339.995)");
            return "";
        }
    };
}