/** */
angular
    .module('Ug5kweb')
    .factory('transerv', transerv);

transerv.$inject = ['$filter'];

/** */
function transerv($filter) {

    var $translate = $filter('translate');

    /** */
    return {
        translate: function (key) {
            return $translate(key);
        }
    };
}