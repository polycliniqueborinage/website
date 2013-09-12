(function() {

var JQUERY = 'jquery-1.5.2.min.js';
var JQUERY_UI = 'jquery-ui-1.8.11.custom.min.js';
var JQUERY_LEGACY = 'jquery-1.3.2.min.js';
var JQUERY_UI_LEGACY = 'jquery-ui-1.7.3.custom.min.js';

var qs = window.location.href.match(/(\?.*)?$/)[0];
var legacy = qs.indexOf('legacy') != -1;
var noui = qs.indexOf('noui') != -1;
var debug;
var prefix;
var tags;


startload();

css('main.css');
css('common/common.css');
css('basic/basic.css');
// TODO BUG
css('resources/resources.css');
//css('resource/resource.css');
css('agenda/agenda.css');
cssprint('common/print.css');

js('defaults.js');
js('main.js');

// TOOD merge calendar.js
//js('Calendar_timeline.js');
js('Calendar_resource.js');

js('Header.js');

//TOOD merge EventManager.js
//js('EventManager_timeline.js');
js('EventManager_resource.js');

//TOOD merge date_util.js
//js('date_util_timeline.js');
js('date_util_resource.js');

//TOOD merge util.js
js('util.js');

js('basic/MonthView.js');
js('basic/BasicWeekView.js');
js('basic/BasicDayView.js');
js('basic/BasicView.js');
js('basic/BasicEventRenderer.js');

js('resource/ResourceDayView.js');
js('resource/ResourceWeekView.js');
js('resource/ResourceMonthView.js');
js('resource/ResourceView.js');
js('resource/ResourceEventRenderer.js');

js('resources/ResourcesDayView.js');
js('resources/ResourcesView.js');
js('resources/ResourcesList.js');
js('resources/ResourcesEventRenderer.js');

js('agenda/AgendaWeekView.js');
js('agenda/AgendaDayView.js');
js('agenda/AgendaView.js');
js('agenda/AgendaEventRenderer.js');

js('common/View.js');
js('common/DayEventRenderer.js');
js('common/SelectionManager.js');
js('common/OverlayManager.js');
js('common/CoordinateGrid.js');
js('common/HoverListener.js');
js('common/HorizontalPositionCache.js');

endload();


if (debug) {
	window.onload = function() {
		$('body').append(
			"<form style='position:absolute;top:0;right:0;text-align:right;font-size:10px;color:#666'>" +
				"<label for='legacy'>legacy</label> " +
				"<input type='checkbox' id='legacy' name='legacy'" + (legacy ? " checked='checked'" : '') +
					" style='vertical-align:middle' onclick='$(this).parent().submit()' />" +
				"<br />" +
				"<label for='ui'>no jquery ui</label> " +
				"<input type='checkbox' id='ui' name='noui'" + (noui ? " checked='checked'" : '') +
					" style='vertical-align:middle' onclick='$(this).parent().submit()' />" +
			"</form>"
		);
	};
}


window.startload = startload;
window.endload = endload;
window.css = css;
window.js = js;
window.jslib = jslib;


function startload() {
	debug = false;
	prefix = '';
	tags = [];
	var scripts = document.getElementsByTagName('script');
	for (var i=0, script; script=scripts[i++];) {
		if (!script._checked) {
			script._checked = true;
			var m = (script.getAttribute('src') || '').match(/^(.*)_loader\.js(\?.*)?$/);
			if (m) {
				prefix = m[1];
				debug = (m[2] || '').indexOf('debug') != -1;
				break;
			}
		}
	}
}


function endload() {
	document.write(tags.join("\n"));
}


function css(file) {
	tags.push("<link rel='stylesheet' type='text/css' href='" + prefix + file + "' />");
}


function cssprint(file) {
	tags.push("<link rel='stylesheet' type='text/css' href='" + prefix + file + "' media='print' />");
}


function js(file) {
	tags.push("<script type='text/javascript' src='" + prefix + file + "'></script>");
}


function jslib(file) {
	js(file);
}


})();
