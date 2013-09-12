
fcViews.resourcesDay = ResourcesDayView;

function ResourcesDayView(element, calendar) {
	
	var t = this;
	
	// exports
	t.render = render;
	
	// imports
	ResourcesView.call(t, element, calendar, 'resourcesDay');
	
	console.log(t);

	var opt = t.opt;
	var renderResourcesView = t.renderResourcesView;
	var formatDate = calendar.formatDate;
	
	function render(date, delta, rebuildSkeleton) {
		
		if (delta) {
			addDays(date, delta);
			if (!opt('weekends')) {
				skipWeekend(date, delta < 0 ? -1 : 1);
			}
		}
		var start = cloneDate(date, true);
		var end = addDays(cloneDate(start), 1);
		t.title = formatDate(date, opt('titleFormat'));
		t.start = t.visStart = start;
		t.end = t.visEnd = end;
		renderResourcesView(rebuildSkeleton);
	}

}
