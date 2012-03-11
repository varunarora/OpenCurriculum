jQuery(function($){
		$("#slider").easySlider({
		auto: true,
		continuous: true,
		pause: 5000
});

	//$('#left-panel').hide("slide", { direction: "left" }, 1000);

 $('.ttip').tooltip({
	position: 'bottom center',
	effect: 'fade',
	opacity: 0.9
 });


    $('#loggedOut a#login').click(function(){
		$('#loginBox').slideToggle("fast");
		$(this).toggleClass("bold");
	$(this).attr("background-color", "#ffffff");
    });

	$('#subjectList').hide();
	
	$('#subjectBrowse').click(function(){
		$('#subjectList').toggle("slide", { direction: "up" }, 200);
	});
	
	$('#closeBox').click(function(){
		$('#loggedOut a#login').trigger('click');
	});
	
	$("#usual1").tabs(); 
 
 	$('a[href^="mailto"]').live('click', function(){
		var email = $(this).attr('href');
		// Ideal case below
		var address = email.replace(/\s*\(at\)\s*/, '@').replace(/\s*\(dot\)\s*/g, '.');
		$(this).attr('href', address);
		return true;
	});
 
 	$(".tweet").tweet({
            username: $("#twitter_handle .field-item").html(),
            //join_text: "auto",
            avatar_size: 32,
            count: 4,
            //auto_join_text_default: "we said,",
            auto_join_text_ed: "we",
            auto_join_text_ing: "we were",
            auto_join_text_reply: "we replied to",
            auto_join_text_url: "we were checking out",
            loading_text: "loading tweets..."
        });
	
	$('.question').click(function(){
		$(this).next().animate({ opacity: 'toggle', height: 'toggle' }, "slow");
		$(this).toggleClass('open-qa');
	});


 	Shadowbox.init();
});
