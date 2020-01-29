/** */
angular
    .module('Ug5kweb')
    .config(config);

function config($routeProvider) {
    $routeProvider
        .when('/', {
            templateUrl: 'app/views/ug5kweb-inicio.html',
            controller:  'ug5kInicioCtrl',
            controllerAs: 'vm',
            resolve: {
					permission: function(authservice) {
					    return authservice.check_session_route();
				}
			}
			})
		.when('/hard', {
            templateUrl: 'app/views/ug5kweb-hardware.html',
            controller:  'ug5kHardCtrl',
            controllerAs: 'vm',
            resolve: {
					permission: function(authservice) {
					    return authservice.check_session_route();
				}
			}
			})
		.when('/serv', {
            templateUrl: 'app/views/ug5kweb-servicios.html',
            controller:  'ug5kServCtrl',
            controllerAs: 'vm',
            resolve: {
					permission: function(authservice) {
					    return authservice.check_session_route();
				}
			}
			})
		.when('/pconf', {
            templateUrl: 'app/views/ug5kweb-config.html',
            controller:  'ug5kConfCtrl',
            controllerAs: 'vm',
            resolve: {
					permission: function(authservice) {
					    return authservice.check_session_route();
				}
			}
			})
		.when('/precr/:recId', {
            templateUrl: 'app/views/ug5kweb-recr.html',
            controller:  'ug5kRecrCtrl',
            controllerAs: 'vm',
            resolve: {
					permission: function(authservice) {
					    return authservice.check_session_route();
				}
			}
			})
		.when('/prect/:recId', {
            templateUrl: 'app/views/ug5kweb-rect.html',
            controller:  'ug5kRectCtrl',
            controllerAs: 'vm',
            resolve: {
					permission: function(authservice) {
					    return authservice.check_session_route();
				}
			}
			})
		.when('/pmant', {
		    templateUrl: 'app/views/ug5kweb-mant.html',
		    controller: 'ug5kMantCtrl',
		    controllerAs: 'vm',
		    resolve: {
		        permission: function (authservice) {
		            return authservice.check_session_route();
		        }
		    }
		})
		.when('/ulises', {
		    templateUrl: 'app/views/ug5kweb-ulises.html',
		    controller: 'ug5kUlisesCtrl',
		    controllerAs: 'vm',
		    resolve: {
		        permission: function (authservice) {
		            return authservice.check_session_route();
		        }
		    }
		})
		.when(routeForUnauthorizedAccess, {
		    templateUrl: 'app/views/session-expired.html'
		    // templateUrl: 'session-expired.html'
            }
		);
}