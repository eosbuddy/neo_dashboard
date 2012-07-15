<?php
// $Id: neo-dashboard-page.tpl.php,v 1.3.2.1 2010/07/28 16:27:36 beretta627 Exp $

/**
 * @file
 * Page template file for Neo Dashboard module.
 */
global $user; 
$user2 = user_load(arg(1));
if (user_access('access neo dashboard browser')) {
   $widgetpath='widgets';
   if ($user->uid == 1) {
      $syswidgetpath='system';
   }
}
?>
<div id="neo-dashboard" class="clear-block">
  <div class="messages warning noscript"><?php print t('Enable Javascript to customize your Neo dashboard.') ?></div>
  <?php 
    global $user; 
    $user2 = user_load(arg(1)); 
    if (user_access('access neo dashboard browser')) {
       if ($user->uid == 1) { 
  ?>
  <div id="nav-separate-neo-dashboard" class="right">
    <ul class="links"> 
       <li><a href="<?php echo $syswidgetpath?>">System Widgets</a></li>
  <?php 
          if ($user2->uid==1) { 
  ?>
       <li><a href="<?php echo $widgetpath?>">More Widgets</a></li> 
  <?php 
          }
  ?>
     </ul>
  </div>
  <? 
       } elseif ($user->uid == $user2->uid) { 
  ?> 
  <div id="nav-separate-neo-dashboard" class="right">
    <ul class="links"> 
       <li><a href="<?php echo $widgetpath?>">More Widgets</a></li> 
     </ul>
  </div>
  <?php 
       } else {
  ?>
  <div id="nav-separate-neo-dashboard-hidden" class="right">
    <ul class="links"> 
     </ul>
  </div>
  <?php 
       }
    } else {
  ?>
  <div id="nav-separate-neo-dashboard-hidden" class="right">
    <ul class="links"> 
     </ul>
  </div>
  <?php 
    }
  ?> 
  <div class="scroll-pane ui-widget ui-widget-header ui-corner-all">
      <div class="scroll-content">
          <div class="nav-content-neo-dashboard"><?php print theme('links', $tabs); ?></div>
          <div id="test" class="scroll-bar-wrap ui-widget-content ui-corner-bottom">
		<div class="scroll-bar"></div>
          </div>
      </div>
  </div>
  <div id="column-wrapper" class="clear-block">
    <div class="column">
        <?php print $widgets[0] ?>
    </div>
  </div>
</div>
