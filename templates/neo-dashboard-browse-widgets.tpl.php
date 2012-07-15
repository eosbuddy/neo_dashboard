<div id="nav-separate-neo-dashboard-hidden" class="right">
  <ul class="links">
   </ul>
</div>
<?php if($links): ?>
  <div class="nav-content-neo-dashboard"><?php print $links; ?></div>
<?php endif; ?>
<?php print $widget_filter; ?>
<?php if($widgets): ?>
  <?php foreach ($widgets as $widget): ?>
    <?php print theme('neo_dashboard_display_widget', $widget); ?> 
  <?php endforeach; ?>
<?php endif; ?>

