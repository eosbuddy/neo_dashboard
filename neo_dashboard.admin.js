if(Drupal.jsEnabled) {
  $(document).ready(function(){
    Drupal.CTools.AJAX.commands.modal_dismiss = null;
    Drupal.CTools.AJAX.commands.modal_dismiss = function(){
      Drupal.CTools.Modal.dismiss();
      window.location.reload(true);
    }
  });

  /**
   * Default behaviors - add default to default list.
   */
  Drupal.behaviors.dashboardMakeAvailable = function(context) {
    $('a.make-available-default').click(function() {
      var thisparent = $(this).parent();
      $(thisparent).siblings().find('>a.make-permanent-default').css({"visibility":"hidden"});

      var default_subtype_value = $(this).attr('id').replace(/^make-available-default-/, '');
      var link_element = this;

      // Save the data into database
      $.post(Drupal.settings.basePath + 'admin/settings/neodash/default-options/make-available',
        {
          subtype: default_subtype_value 
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
   * Default behaviors - add default to system list.
   */
  Drupal.behaviors.dashboardMakePermanent = function(context) {
    $('a.make-permanent-default').click(function() {
      var thisparent = $(this).parent();
      $(thisparent).siblings().find('>a.make-available-default').css({"visibility":"hidden"});

      var default_subtype_value = $(this).attr('id').replace(/^make-permanent-default-/, '');
      var link_element = this;

      // Save the data into database
      $.post(Drupal.settings.basePath + 'admin/settings/neodash/default-options/make-permanent',
        {
          subtype: default_subtype_value 
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
   * Default behaviors - Remove default to default list.
   */
  Drupal.behaviors.dashboardRemove = function(context) {
    $('a.remove-default').click(function() {

      var default_subtype_value = $(this).attr('id').replace(/^remove-default-/, '');
      var link_element = this;

      // Save the data into database
      $.post(Drupal.settings.basePath + 'admin/settings/neodash/default-options/remove',
        {
          subtype: default_subtype_value 
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
   * Default behaviors - Remove default to default list.
   */
  Drupal.behaviors.dashboardRemoveSys = function(context) {
    $('a.remove-permanent').click(function() {

      var default_subtype_value = $(this).attr('id').replace(/^remove-permanent-/, '');
      var link_element = this;

      // Save the data into database
      $.post(Drupal.settings.basePath + 'admin/settings/neodash/default-options/removesys',
        {
          subtype: default_subtype_value 
        },
        function(data, textStatus, XMLHttpRequest) {
          $(link_element).replaceWith(data.label);
        },
        "json"
      );

      return false;
    });
  }

}

