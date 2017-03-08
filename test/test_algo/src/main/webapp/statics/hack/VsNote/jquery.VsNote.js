(function($) {
	$.VsNote = function(note, options, callback) {
		return $.fn.VsNote(note, options, callback)
	};
	$.fn.VsNote = function(note, options, callback) {
		var position = 'top-center';
		var settings = {
			'speed': 'fast',
			'duplicates': false,
			'autoclose': 5000
		};
		if (!note) {
			note = this.html()
		};
		if (options) {
			$.extend(settings, options)
		};
		var display = true;
		var duplicate = 'no';
		var uniqID = Math.floor(Math.random() * 99999);
		$('.VsNote-note').each(function() {
			if ($(this).html() == note && $(this).is(':visible')) {
				duplicate = 'yes';
				if (!settings['duplicates']) {
					display = false
				}
			};
			if ($(this).attr('id') == uniqID) {
				uniqID = Math.floor(Math.random() * 9999999)
			}
		});
		if (!$('body').find('.VsNote-queue').html()) {
			$('body').append('<div class="VsNote-queue ' + position + '"></div>')
		};
		if (display) {
			$('.VsNote-queue').prepend('<div class="VsNote border-' + position + '" id="' + uniqID + '"></div>');
			$('#' + uniqID).append('<div class="VsNote-note" rel="' + uniqID + '">' + note + '</div>');
			var height = $('#' + uniqID).height();
			$('#' + uniqID).css('height', height);
			$('#' + uniqID).slideDown(settings['speed']);
			display = true
		};
		$('.VsNote').ready(function() {
			if (settings['autoclose']) {
				$('#' + uniqID).delay(settings['autoclose']).fadeOut(settings['speed'])
			}
		});
		$('.VsNote-close').click(function() {
			$('#' + $(this).attr('rel')).dequeue().fadeOut(settings['speed'])
		});
		var response = {
			'id': uniqID,
			'duplicate': duplicate,
			'displayed': display,
			'position': position
		};
		if (callback) {
			callback(response)
		} else {
			return (response)
		}
	}
})(jQuery);