<?php 
  // $Id: neo-dashboard-list-widget.tpl.php,v 1.2.4.1 2010/07/28 16:27:36 beretta627 Exp $ neo-dashboard-list-widget.tpl.php, v 1.0 2010/07/13 15:27:22 beretta627 Exp $
  drupal_add_js('misc/tableheader.js');
?>
<?php if($help): ?>
  <p><?php print $help; ?></p>
<?php endif; ?>  
<?php print $widget_add; ?>
<?php print $widget_filter; ?>
  <table class="widget-entry sticky-enabled <?php print $widget->classes; ?>">
    <thead>
      <tr>
        <th><?php print t('Widget'); ?></th>
        <th><?php print t('Description'); ?></th>
        <th><?php print t('Operations'); ?></th>
      </tr>
    </thead>
    <tbody>
    <?php $row = 0; ?>
    <?php foreach ($widgets as $widget): ?>
      <tr class="<?php print $row % 2 == 0 ? 'odd' : 'even'; ?>">
        <td class="widget-name">
          <?php print $help_type_icon; ?>
          <?php if ($widget->title): ?>            
            <div class="widget-title"><h4><?php print $widget->title; ?></h4></div>
          <?php endif; ?>
          <?php if (!empty($widget->tag)): ?>
            <span class="widget-tags"><?php print t('tags'); ?>:&nbsp;<em><?php print $widget->tag; ?></em></span>
          <?php endif; ?>
        </td>
        <td class="widget-description">
          <?php if ($widget->description): ?>
            <?php print $widget->description; ?>
          <?php endif; ?>
        </td>
        <td class="widget-ops"><?php print $widget->ops ?></td>
      </tr>
      <?php $row++; ?>
    <?php endforeach; ?>
    </tbody>
  </table>
