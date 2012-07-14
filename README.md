neo_dashboard
=============

The neo_dashboard module is a fork of the dashboard module in drupal project. It is a 
tool for providing personalized dashboard interfaces, similar to iGoogle or MyYahoo. 

Improvements in Neo Dashboard: (1) The dashboards so created can be shared with users
                                   (public dashboard is default behaviour - it can be
                                   made private if desired).
                               (2) There is a facility to push in "System Widgets" or
                                   content into user dashboards.  These can be specific
                                   system notifications or
                                   advertisement widgets.
                               (3) The widgets can be resized and moved about within the
                                   dashboard tab.
                               (4) Uses the latest jquery and jquery-ui framework

Required Modules
-------------------

- JQuery UI, Chaos Tools, Views

Installation
------------

1) Copy the dashboard directory to the modules folder in your installation.

2) Enable the module using Administer -> Modules (/admin/build/modules)

3) Clear the theme registry by going to admin/settings/performance and
   clicking on the 'Clear Cached Data' button at the bottom of the page.
   This wipes the theme registry clear and allows dashboard to function.

Configuration
-------------

The configuration for Dashboard is split between the admin and user screens.

1) Add & Configure the widgets for your site at
   Administer -> Site configuration -> NeoDash (/admin/settings/neodash)

   You will first need to go to the "Select Default Widgets" tab
   (/admin/settings/neodash/default-options) and select what you want to provide to the
   users in general and what you want to reserve as system widgets (you can create
   new content types and use views and make these available as blocks for specific
   situations).  Any decisions made can be reversed on this page by clicking "Remove".

   You will then need to go the General Options (do not click the "set default layout" yet)
   and configure the "Personalization" "Widget Types" and "Widget Thumbnails".

2) Go to the "Permissions" page (/admin/user/permissions) and configure the permissions
   for the various roles you have on your site.

3) Go to the neodash page (/nd/<admin id>/neodash) and click on "More Widgets" and/or
   "System Widgets" and add widgets.

4) Go back to the neodash page (/nd/<admin id>/neodash) and you should see all the widgets you
   added.  Hovering over the "NeoDash" link in the tab bar should popup your configuration
   options.

5) Go back to the Administer -> Site configuration -> NeoDash (/admin/settings/neodash) page
   and click on the "Set default layout" button and you're done.

Errata
-------

Dashboard depends on Ctools, Jquery UI and Views to operate. The sole reason Views needs
to be installed is because of how some forms are generated. This requirement for Views
will be eliminated from future editions of the module.

This is work in progress.

Support
-------

If you experience a problem with dashboard or have a problem, file a
request or issue on the dashboard queue at


--TODO--
Need to add support information.
------------

Licensed under the GPL 2.0.
http://www.gnu.org/licenses/gpl-2.0.txt
