// $Id: neo_dashboard.js,v 1.13.4.1 2010/07/28 16:27:36 beretta627 Exp $

Drupal.behaviors.dashboardTab = function(context) {
  $tabList = $('.nav-content-neo-dashboard:not(.tab-processed)', context).addClass('tab-processed');
  $tabNames=$tabList.children().find('>li>a');
  $tabSpan=$tabList.children().find('>li>span');
  var tabLengthSum=0;
  $tabNames.each(function(i, thistab) {
        tabLengthSum=tabLengthSum+$(thistab).width();
  });
  if (tabLengthSum <= 300) {
	$scrollContent=$tabList.parent('.scroll-content');
        $scrollWrap=$scrollContent.parent().removeClass('scroll-pane');
        $scrollContent.css({"width":"100%"});
  }  
  if (tabLengthSum > 300) {
	$(function() {
		var scrollPane = $( ".scroll-pane" ),
			scrollContent = $( ".scroll-content" );
		
		var scrollbar = $( ".scroll-bar" ).slider({
			slide: function( event, ui ) {
				if ( scrollContent.width() > scrollPane.width() ) {
					scrollContent.css( "margin-left", Math.round(
						ui.value / 100 * ( scrollPane.width() - scrollContent.width() )
					) + "px" );
				} else {
					scrollContent.css( "margin-left", 0 );
				}
			}
		});
		
		//append icon to handle
		var handleHelper = scrollbar.find( ".ui-slider-handle" )
		.mousedown(function() {
			scrollbar.width( handleHelper.width() );
		})
		.mouseup(function() {
			scrollbar.width( "100%" );
		})
		.append( "<span class='ui-icon ui-icon-grip-dotted-vertical'></span>" )
		.wrap( "<div class='ui-handle-helper-parent'></div>" ).parent();
		
		//change overflow to hidden now that slider handles the scrolling
		scrollPane.css( "overflow", "hidden" );
		
		//size scrollbar and handle proportionally to scroll distance
		function sizeScrollbar() {
			var remainder = scrollContent.width() - scrollPane.width();
			var proportion = remainder / scrollContent.width();
			var handleSize = scrollPane.width() - ( proportion * scrollPane.width() );
			scrollbar.find( ".ui-slider-handle" ).css({
				width: handleSize,
				"margin-left": -handleSize / 2
			});
			handleHelper.width( "" ).width( scrollbar.width() - handleSize );
		}
		
		//reset slider value based on scroll content position
		function resetValue() {
			var remainder = scrollPane.width() - scrollContent.width();
			var leftVal = scrollContent.css( "margin-left" ) === "auto" ? 0 :
				parseInt( scrollContent.css( "margin-left" ) );
			var percentage = Math.round( leftVal / remainder * 100 );
			scrollbar.slider( "value", percentage );
		}
		
		//if the slider is 100% and window gets larger, reveal content
		function reflowContent() {
				var showing = scrollContent.width() + parseInt( scrollContent.css( "margin-left" ), 10 );
				var gap = scrollPane.width() - showing;
				if ( gap > 0 ) {
					scrollContent.css( "margin-left", parseInt( scrollContent.css( "margin-left" ), 10 ) + gap );
				}
		}
		
		//change handle position on window resize
		$( window ).resize(function() {
			resetValue();
			sizeScrollbar();
			reflowContent();
		});
		//init scrollbar size
		setTimeout( sizeScrollbar, 10 );//safari wants a timeout
		});
  	}
}

Drupal.behaviors.widget = function(context) {
  $myarea = $('.right-corner');
  var bodywidth=$myarea.width();
  var $dashboard = $('#neo-dashboard');
  var $columnwrapper = $dashboard.find('>div#column-wrapper').addClass('column-wrapper');
  var $column = $columnwrapper.find('>div.column');
  $widget = $('.widget-wrapper:not(.widget-processed)', context).addClass('widget-processed');
  if ($widget.length > 0) {
    var dragTimeStamp;
    $widgetcont = $widget.find('>div.widget>div.content');
    jQuery.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage + '/widget-width',
        {
            token: Drupal.settings.dashboardToken,
            user: Drupal.settings.currentUser,
            page: Drupal.settings.dashboardPage
        },
        function (size) {
            var sizes = jQuery.parseJSON(JSON.stringify(size));
            $widget.each(function(i, thiswidget) {
                var wr = thiswidget;
                var wwidth = sizes[i].width;
                var wheight = sizes[i].height;
                var resfl = sizes[i].wrefl;
                $(wr).width(wwidth).css({position: "absolute", overflow:'hidden'});
                $(wr).height(wheight).css({position: "absolute", overflow:'hidden'});
                $widgetdiv = $(wr).find('>div.widget');
                $widgetcont = $widgetdiv.find('>div.content');
                if ($widgetcont.is(':hidden')) {
                    $widgetdiv.addClass('hidden');
                    $(wr).find('>.ui-resizable-handle').css({"visibility": "hidden"});
                } else {
                    if (($widgetcont.is(':empty')) || (resfl == 1)) {
                        $(wr).css("overflow", "visible");
                    } else { 
			$(wr).css({position: "absolute", 'overflow-y':'hidden','height':'auto', 'overflow-x': 'hidden'});
                    }
                    $(wr).find('>.ui-resizable-handle').css({"visibility": "visible"});
                }
                $(wr).find('>div.widget>div.content').addClass($(wr).attr('id'));
            });
    });
  }
}

Drupal.behaviors.widgetpos = function(context) {
  $myarea = $('.right-corner');
  var bodywidth=$myarea.width();
  $widget = $('.widget-wrapper');
  var $dashboard = $('#neo-dashboard');
  var $sepnavbar = $dashboard.find('>div#nav-separate-neo-dashboard');
  var $scrollpane = $dashboard.find('>div.scroll-pane');
  var $columnwrapper = $dashboard.find('>div.column-wrapper');
  var $column = $columnwrapper.find('>div.column');
  var coloffset=$column.offset();
  var cwoffset=$columnwrapper.offset();
  if ($scrollpane) {
     var scrollpaneht = $scrollpane.height();
     var sepnavbarht = $sepnavbar.height();
  } else {
     var scrollpaneht = 0;
     var scrollpaneht = 0;
  }
  var marginht = 17;
  var mymaxwidth = 200;
  var myminwidth = 200;
  
  if ($widget.length > 0) {
    jQuery.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage + '/position-widget',
        {
            token: Drupal.settings.dashboardToken,
            user: Drupal.settings.currentUser,
            page: Drupal.settings.dashboardPage
        },
        function (wpos) {
            var wposs = jQuery.parseJSON(JSON.stringify(wpos));
            var conc = 0;
            widwidths = [];
            var pastheight=0;
            $widget.each(function(i, thiswidget) {
                var w = thiswidget;
                var posfl = wposs[i].wpofl;
                widwidths.push(parseInt($(w).width()));
                if (posfl == 0) {
                    var wleft = 35;
                    var wtemp = wposs[i].wtop; 
                    var wtop = parseInt(wtemp) + parseInt(pastheight) + scrollpaneht - sepnavbarht + marginht;
                    conc = conc + 1
                    pastheight=parseInt($(w).height())+parseInt(pastheight);
                } else {
                    var wleft = wposs[i].wleft;
                    var wtop = wposs[i].wtop;
                }
                $(w).css({position: "absolute", left: parseInt(wleft)});
                $(w).css({position: "absolute", top: parseInt(wtop)});
            });
            maxwidth=Math.max.apply(Math, widwidths);
            var mymaxwidth = maxwidth;
            minwidth=Math.min.apply(Math, widwidths);
            var myminwidth = minwidth;
    });
    var dragTimeStamp;
    var drtop=cwoffset.top + scrollpaneht + sepnavbarht;
    var drleft=cwoffset.left;
    var dbwidth=$dashboard.width();
    var cwwidth=$columnwrapper.width();
    var drright = 1200 - mymaxwidth;
    var drbottom = 10000;

    var perflag = 0;
    $widget.draggable({
        containment: [drleft, drtop, drright, drbottom],
        //revert: "valid",
        //alsoResize: 'parent',
        scroll: false,
        zIndex: 2000,
        drag: function(event,ui) {
            var thisos = $(this).offset();
            var drleft=cwoffset.left;
            var thisle = $(this).position().left;
            var thiswi = $(this).width();
            var thiswidget = $(this).find('>div.widget');
            var rightpe = 1200 -thiswi;
            if (thisle >= rightpe) {
                
                $(this).draggable({revert: true});
            } else {
                $(this).draggable({revert: 'valid'});
            }
        },
        stop: function(event, ui) {
            posflag = [];
            wtops = [];
            left = [];
            $widget.each(function() {
                wtops[$(this).attr('id').replace(/widget-wrapper-/g,'') - 1] = $(this).position().top;
                left[$(this).attr('id').replace(/widget-wrapper-/g,'') - 1] = $(this).position().left;
                if (($(this).position().top == 150) && ($(this).position().left == 750)) {
                    posflag[$(this).attr('id').replace(/widget-wrapper-/g,'') - 1] = 0;
                } else {
                    posflag[$(this).attr('id').replace(/widget-wrapper-/g,'') - 1] = 1;
                }
            });
            jQuery.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage + '/widget-position', {
                user: Drupal.settings.currentUser,
                widpof: posflag,
                widtop: wtops,
                widlef: left,
                page: Drupal.settings.dashboardPage
            });
            dragTimeStamp = event.timeStamp;
        }
    }).resizable({
        handles: 'all',
        helper: 'ui-resizable-helper',
        stop: function(event, ui) {
            resflag = [];
            heights = [];
            widths = [];
            $widget.each(function(i, thiswidget) {
                var wrn = thiswidget;
                widths[$(wrn).attr('id').replace(/widget-wrapper-/g,'') - 1] = $(wrn).width();
                heights[$(wrn).attr('id').replace(/widget-wrapper-/g,'') - 1] = $(wrn).height();
                resflag[$(this).attr('id').replace(/widget-wrapper-/g,'') - 1] = 1;
            });
            jQuery.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage + '/resize-widget', {
                token: Drupal.settings.dashboardToken,
                widref: resflag,
                widwid: widths,
                widhei: heights,
                page: Drupal.settings.dashboardPage
            });
            dragTimeStamp = event.timeStamp;
        }
    }).droppable({
            tolerance: "touch"
    }).find('>div.widget>h2').click(function(event) {
        if ((event.timeStamp - dragTimeStamp) <= 20) {
            return false;
        }
    });
  }
}

Drupal.behaviors.dashboardNavigation = function(context) {
  $navigation = $('.nav-content-neo-dashboard:not(.neo-dashboard-processed)', context).addClass('neo-dashboard-processed');

  if ($navigation.length > 0) {
    // Add IDs for sortable('toArray'). theme('links') can not put IDs here.
    
    $navigation.find('>ul>li').each(function() {
      $this = $(this);
      $this.attr('id', $this.attr('class').replace(/ .*/, ''));
    });

    // Fake hovering for edit & delete icons.
    $dashboardActiveSpan = $navigation.find('>ul>li.active>span').hover(
      function () {
        $dashboardActiveSpan.addClass('hover');
      },
      function () {
        $dashboardActiveSpan.removeClass('hover');
      }
    );

    // Drag & drop tabs
    $navigation.sortable({
      items: '>ul>li:not(.neo-dashboard-link-add, .neo-dashboard-profile, .neo-dashboard-system-widgets, .neo-dashboard-widgets, .0)',
      axis: 'x',
      distance: 5,
      start: function(event, ui) {
        ui.helper.find('>span.hover').removeClass('hover');
      },
      stop: function(event, ui) {
        jQuery.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage + '/reorder-pages', {
          token: Drupal.settings.dashboardToken,
          pages: $navigation.sortable('toArray').join(',').replace(/neo-dashboard-page-/g, '')
        });
        $navigation.find('>ul>li:not(.neo-dashboard-link-add, .neo-dashboard-profile)').each(function() {
          $this = $(this);
          path = $this.attr('class').replace(/.* neo-dashboard-path-([^ ]*).*/, '$1');
          $this.find('>a').attr('href', Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + path);
        });
        $navigation.find('>ul>li:first>a').attr('href', Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser);
      }
    });
    $navigation.find('>ul>li.neo-dashboard-link-add>a').click(function() {
      // Hide add link, show add form.
      $this = $(this).hide().parent().append(Drupal.settings.dashboardPageAddForm);
      $dashboardAddForm = $('#neo-dashboard-page-add-form');
      $dashboardAddForm.find('#edit-title').keyup(function(event) {
        if (event.which == 27) { // Esc pressed.
          dashboardRemoveAddPageForm();
        }
        else {
          // Enable Add button only when text is present.
          if ($(this).attr('value') == '') {
            $dashboardAddForm.find('#edit-submit').attr('disabled', 'disabled');
          }
          else {
            $dashboardAddForm.find('#edit-submit').removeAttr('disabled');
          }
        }
      }).focus();
      Drupal.settings.dashboardBodyClickParent = '#neo-dashboard-page-add-form';
      Drupal.settings.dashboardBodyClickCallback = dashboardRemoveAddPageForm;
      $('body').bind('click', dashboardBodyClick);
      return false;
    });

    // Edit page link
    $dashboardActiveSpan.find('>a.edit').click(function() {
      // Hide edit link, show edit form.
      $dashboardActiveSpan.find('>a').hide().end().append(Drupal.settings.dashboardPageEditForm);
      $dashboardEditForm = $('#neo-dashboard-page-edit-form').find('div.delete').hide().end();
      $dashboardEditForm = $('#neo-dashboard-page-edit-form').find('div.private').hide().end();

      // Correct title in case it has already been edited.
      $dashboardEditForm.find('#edit-edit-title').attr('value', $dashboardActiveSpan.find('>a.edit').text());

      // Allow AHAH form submission, but make changes active instantly.
      Drupal.attachBehaviors($dashboardEditForm);
      $dashboardEditFormSubmit = $dashboardEditForm.find('#edit-edit-submit').click(function() {
        $dashboardActiveSpan.find('>a.edit').html(Drupal.checkPlain($dashboardEditForm.find('#edit-edit-title').attr('value')) + '<span class="edit-icon"></span>');
        dashboardRemoveEditPageForm();
      });
      $dashboardEditForm.find('#edit-edit-title').keyup(function(event) {
        if (event.which == 27) { // Esc pressed.
          dashboardRemoveEditPageForm();
        }
        else {
          // Enable Edit button only when text is present.
          if ($(this).attr('value') == '') {
            $dashboardEditFormSubmit.attr('disabled', 'disabled');
          }
          else {
            $dashboardEditFormSubmit.removeAttr('disabled');
          }
        }
      }).focus();
      Drupal.settings.dashboardBodyClickParent = '#neo-dashboard-page-edit-form';
      Drupal.settings.dashboardBodyClickCallback = dashboardRemoveEditPageForm;
      $('body').bind('click', dashboardBodyClick);
      return false;
    });

    // Delete page link
    $dashboardActiveSpan.find('>a.delete').click(function() {
      // Hide edit link, show edit form.
      $dashboardActiveSpan.find('>a').hide().end().append(Drupal.settings.dashboardPageEditForm);
      $dashboardEditForm = $('#neo-dashboard-page-edit-form').find('div.edit').hide().end();
      $dashboardEditForm = $('#neo-dashboard-page-edit-form').find('div.private').hide().end();

      // Correct title in case it has already been edited.
      $dashboardEditForm.find('#edit-delete').attr('value', Drupal.t('Yes, delete !title', {'!title': $dashboardActiveSpan.find('>a.edit').text()}));
      $dashboardEditForm.find('#edit-delete').click(function() {
        // Set a non-dynamic title so Form API is not confused.
        $(this).attr('value', Drupal.t('Deleting…'));
      });

      Drupal.settings.dashboardBodyClickParent = '#neo-dashboard-page-edit-form';
      Drupal.settings.dashboardBodyClickCallback = dashboardRemoveEditPageForm;
      //$('body').bind('click', dashboardBodyClick);
      $dashboardEditForm.find('a.cancel').click(dashboardRemoveEditPageForm);
      return false;
    });
    // Public page link
    $dashboardActiveSpan.find('>a.private').click(function() {
      // Hide edit link, show edit form.
      $dashboardActiveSpan.find('>a').hide().end().append(Drupal.settings.dashboardPageEditForm);
      $dashboardEditForm = $('#neo-dashboard-page-edit-form').find('div.edit').hide().end();
      $dashboardEditForm = $('#neo-dashboard-page-edit-form').find('div.delete').hide().end();

      // Correct title in case it has already been edited.
      jQuery.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage + '/get-privacy',
        {
            token: Drupal.settings.dashboardToken,
            user: Drupal.settings.currentUser,
            page: Drupal.settings.dashboardPage
        },
        function (privfl) {
            var privfls = jQuery.parseJSON(JSON.stringify(privfl));
            if (parseInt(privfls[0]) == 1) {
                var privacy = 'public';
            } else {
                var privacy = 'private';
            }
            $dashboardEditForm.find('#edit-private').attr('value', Drupal.t('Make !title '+privacy +'?', {'!title': $dashboardActiveSpan.find('>a.edit').text()}));
         }
      );
      //var privacy = 'private';
      $dashboardEditForm.find('#edit-private').click(function() {
        $(this).attr('value', Drupal.t('Processing…'));
        jQuery.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage + '/change-privacy',
        {
            token: Drupal.settings.dashboardToken,
            user: Drupal.settings.currentUser,
            page: Drupal.settings.dashboardPage
        }
      );
        window.location = Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage;
      });
      $dashboardEditForm.find('a.cancel').click(dashboardRemoveEditPageForm);
      return false;
    });
  }
}

/**
 * Widget behaviors - minimize, change dashboard & delete.
 */
Drupal.behaviors.dashboardWidget = function(context) {

  $("#neo-dashboard>div.column-wrapper>div.column>div.widget-wrapper:not(.neo-dashboard-processed)", context).addClass('neo-dashboard-processed').each(function() {
    var $this = $(this);
    var $widgetgrandparentdiv = $this.parent('neo-dashboard');
    var $widgetparentdiv = $this;
    var $widgetdiv = $this.children('div.widget');
    var $mouse_inside = false;

    $('a.minimize-widget', $widgetdiv).data('widget', $widgetdiv).click(function () {
      //console.log($this.children('div.content'));

      var $widget = $widgetdiv.data('widget');
      var $width = $widgetdiv.width();

      if($widgetdiv.children('div.content').is(':hidden')) {
        $widgetdiv.children('div.content').slideDown('fast');
        $widgetdiv.removeClass('hidden');
        jQuery.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage + '/widget-height', {
           token: Drupal.settings.dashboardToken,
           user: Drupal.settings.currentUser,
           page: Drupal.settings.dashboardPage,
           widget_id: $widgetdiv.attr('id').replace(/^widget-/, '')
        },
        function (height) {
            var heights = jQuery.parseJSON(JSON.stringify(height));
            $widgetparent = $widgetdiv.find('>div.ui-resizable-handle');
            $widgetparentdiv.height(parseInt(heights[0]));
        });
      } else {  
        $widgetdiv.width($width);
        $widgetdiv.children('div.content').slideUp('fast');
        $widgetdiv.addClass('hidden');
        $widgetdiv.parent().find('>.ui-resizable-handle').css({"visibility": "hidden"});
      }

      jQuery.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage + '/minimize-widget', {
        token: Drupal.settings.dashboardToken,
        widget_id: $widgetdiv.attr('id').replace(/^widget-/, '')
      });

      return false;
    });
    $('a.remove-widget', $this).data('widget', $this).click(function () {
      var $widget = $(this).data('widget').slideUp('fast');
      jQuery.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage + '/remove-widget', {
        token: Drupal.settings.dashboardToken,
        widget_id: $widgetdiv.attr('id').replace(/^widget-/, '')
      });
      return false;
    });
    $('body').mouseup(function() {
        //if(!$mouse_inside) $('ul.page-list').hide();
    });
    $('a.change-widget-page', $this).data('widget', $this).click(function() {
      var tabWidth = [];
      $(this).data('widget').find('ul.page-list').children('li').children('a').each(function(i,thistab) {
                var testwidth=$(thistab).html().length*10;
                tabWidth.push(parseInt(parseInt(testwidth)));
      });
      maxTabWidth=Math.max.apply(Math, tabWidth);
      //$(this).data('widget').find('ul.page-list').toggle().css('width',parseStr(maxTabWidth)+'px');
      $(this).data('widget').find('ul.page-list').toggle().width(maxTabWidth).css("overflow","auto");
      return false;
    });
    $('ul.page-list>li>a', $this).data('widget', $this).click(function() {
      $this.slideUp('fast');
      jQuery.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + Drupal.settings.dashboardPage + '/change-widget-page', {
        token: Drupal.settings.dashboardToken,
        widget_id: $this.attr('id').replace(/^widget-wrapper-/, ''),
        page_id: $(this).attr('class').replace(/^neo-dashboard-page-/,''),
      });
      return false;
    });
  });


}

/**
 * Widget behaviors - add widget to neo dashboard.
 */
Drupal.behaviors.dashboardWidgetAdd = function(context) {
  $('.neo-dashboard-widget a.add-widget').click(function() {

    // var type is the key from hook_neo_dashboard()
	// Default type is 'user', defined in neo_dashboard_dashboard()
	// TODO: get this value properly
    var type = 'user';
    var widget_id_value = $(this).attr('id').replace(/^add-widget-/, '');
    var link_element = this;

    // Save the data into database
    $.post(Drupal.settings.basePath + 'nd/' + Drupal.settings.currentUser + '/' + type + '/add-widget',
      {
        token: Drupal.settings.dashboardToken,
        widget_id: widget_id_value
      },
      function(data, textStatus, XMLHttpRequest) {
        $(link_element).replaceWith(data.label);
      },
      "json"
    );

    return false;
  });
}

/**
 * Hide the Add page form if click is outside the form.
 */
function dashboardBodyClick(event) {
  if ($(event.target).parents(Drupal.settings.dashboardBodyClickParent).length == 0) {
    Drupal.settings.dashboardBodyClickCallback();
  }
}

/**
 * Remove the Add page form, show the link, remove event.
 */
function dashboardRemoveAddPageForm() {
  $dashboardAddForm.remove();
  $navigation.find('>ul>li.neo-dashboard-link-add>a').show();
  $('body').unbind('click', dashboardBodyClick);
}

/**
 * Remove the Edit page form, show the link, remove event.
 */
function dashboardRemoveEditPageForm() {
  $dashboardEditForm.remove();
  $dashboardActiveSpan.removeClass('hover').find('>a').show();
  $('body').unbind('click', dashboardBodyClick);
}

function dashboardRemoveAddWidget() {
  $dashboardAdd.removeClass('selected').parent().find('>ul').remove();
}

/**
 * Function : dump()
 * Arguments: The data - array,hash(associative array),object
 *    The level - OPTIONAL
 * Returns  : The textual representation of the array.
 * This function was inspired by the print_r function of PHP.
 * This will accept some data as the argument and return a
 * text that will be a more readable version of the
 * array/hash/object that is given.
 * Docs: http://www.openjs.com/scripts/others/dump_function_php_print_r.php
 */
function dump(arr,level) {
	var dumped_text = "";
	if (!level) level = 0;

	//The padding given at the beginning of the line.
	var level_padding = "";
	for(var j=0;j<level+1;j++) level_padding += "    ";

	if (typeof(arr) == 'object') { //Array/Hashes/Objects
		for(var item in arr) {
			var value = arr[item];

			if (typeof(value) == 'object') { //If it is an array,
				dumped_text += level_padding + "'" + item + "' ...\n";
				dumped_text += dump(value,level+1);
			} else {
				dumped_text += level_padding + "'" + item + "' => \"" + value + "\"\n";
			}
		}
	} else { //Stings/Chars/Numbers etc.
		dumped_text = "===>"+arr+"<===("+typeof(arr)+")";
	}
	return dumped_text;
}
