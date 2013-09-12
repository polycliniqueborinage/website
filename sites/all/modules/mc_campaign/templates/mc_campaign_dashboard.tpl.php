<?php
?>
	<script type="text/javascript">
	  google.load("visualization", "1", {packages: ["corechart"]});
    </script>
    <script type="text/javascript">

    // chart and datatable must be global variables in order to access them later.
    var sms_chart;
    var mail_chart;
    var sms;
    var mail;
    
    google.load("visualization", "1", {packages:["corechart"]});
    google.setOnLoadCallback(drawChart);

    function drawChart() {

      // prepare data 
      sms = google.visualization.arrayToDataTable(Drupal.settings.sms);
      sms_chart = new google.visualization.ColumnChart(document.getElementById('sms_chart'));
      google.visualization.events.addListener(sms_chart, 'select', selectSMSHandler);
      sms_chart.draw(
        sms, {
          width: 960,
          height: 250,
          title: 'SMS',
          colors:['orange','red','green','blue'],
          hAxis: {title: 'Date', titleTextStyle: {color: 'black'}
    	}
      });

      mail = google.visualization.arrayToDataTable(Drupal.settings.mail);
      mail_chart = new google.visualization.ColumnChart(document.getElementById('mail_chart'));
      google.visualization.events.addListener(mail_chart, 'select', selectMailHandler);
      mail_chart.draw(
        mail, {
          width: 960,
          height: 250,
          title: 'MAIL',
          colors:['orange','red','green','blue'],
          hAxis: {title: 'Date', titleTextStyle: {color: 'black'}
    	}
      });
      
    }

      function selectSMSHandler() {
    	  var selectedItem = sms_chart.getSelection()[0];
    	  var value = sms.getValue(selectedItem.row, selectedItem.column);
    	  window.location.href ='/account/notifications/sms/' + Drupal.settings.sms[selectedItem.row +1][0] + '/' + selectedItem.column; 
      }

      function selectMailHandler() {
    	  var selectedItem = mail_chart.getSelection()[0];
    	  var value = sms.getValue(selectedItem.row, selectedItem.column);
    	  window.location.href ='/account/notifications/mail/' + Drupal.settings.sms[selectedItem.row +1][0] + '/' + selectedItem.column; 
      }
      
    </script>
    
    <div id="dashboard">
      <table>
        <tr style='vertical-align: top'>
          <td style='width: 960px'>
            <div style="float: left;" id="sms_chart"></div>
          </td>
        </tr>
        <tr style='vertical-align: top'>
          <td style='width: 960px'>
            <div style="float: left;" id="mail_chart"></div>
          </td>
        </tr>
      </table>
    </div>
