jQuery(function($){
	$('.resource_contents').hide();
	$('.showmore').click(function(){
		$(this).parent('div').find('.resource_contents').toggle();
		if($(this).parent('div').find('.resource_contents').is(':visible')){
			$(this).html('&#171; Less');
		} else $(this).html('More &#187;');
	});
	
});