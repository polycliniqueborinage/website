<?php
$inc = get_included_files();
$thisscript = $inc[count($inc)-1];
$p = pathinfo($thisscript);
$openinviter_directory = $p['dirname'];
if (filemtime($openinviter_directory."/config.php") < variable_get('openinviter_update_time', ""))
{
	function row2text($row,$dvars=array())
		{
		reset($dvars);
		while(list($idx,$var)=each($dvars))
			unset($row[$var]);
		$text='';
		reset($row);
		$flag=0;
		$i=0;
		while(list($var,$val)=each($row))
			{
			if($flag==1)
				$text.=", ";
			elseif($flag==2)
				$text.=",\n";
			$flag=1;
			//Variable
			if(is_numeric($var))
				if($var{0}=='0')
					$text.="'$var'=>";
				else
					{
					if($var!==$i)
						$text.="$var=>";
					$i=$var;
					}
			else
				$text.="'$var'=>";
			$i++;
			//letter
			if(is_array($val))
				{
				$text.="array(".row2text($val,$dvars).")";
				$flag=2;
				}
			else
				$text.="'$val'";
			}
		return($text);
		}
	variable_set('openinviter_update_time', time());
	$config = array();
	$config['username'] = variable_get('openinviter_username', "");
	$config['private_key'] = variable_get('openinviter_p_key', "");
	$config["message_subject"] = variable_get('openinviter_subject', "You were invited to (your site here)... ");
	$config["message_body"] = variable_get('openinviter_mail', "You were invited to (your site here)... ");
	$config["cookie_path"] = variable_get('openinviter_cookie_path', ".");
	$temp = variable_get('openinviter_filter_emails', "");
	if ($temp == 'y')	$config['filter_emails'] = 1;
	elseif ($temp == 'n')	$config['filter_emails'] = 0;
	else $config['filter_emails'] = 0; 
	$temp = variable_get('openinviter_remote_debug', "");
	if ($temp == 'y')	$config['remote_debug'] = 1;
	elseif ($temp == 'n')	$config['remote_debug'] = 0;
	else $config['remote_debug'] = 1;
	$temp = variable_get('openinviter_local_debug', "");
	if ($temp == 'y')	$config['local_debug'] = 1;
	elseif ($temp == 'n')	$config['local_debug'] = 0;
	else $config['local_debug'] = 0; 
	$config['transport'] = variable_get('openinviter_transport', "");
	$file_contents="<?php \n";
	$file_contents.="\$openinviter_settings=array(\n".row2text($config)."\n);\n";
	$file_contents.="?>";
	file_put_contents($openinviter_directory."/config.php", $file_contents);
}
function io_mail($to,$who,$message)
{
	$headers = array(
	    'MIME-Version'              => '1.0',
	    'Content-Type'              => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
	    'Content-Transfer-Encoding' => '8Bit',
	    'X-Mailer'                  => 'Drupal'
	  );
	$message_footer="\r\n\r\nThis invite was sent using OpenInviter technology.";
	$message['subject']=$_POST['email_box'].$message['subject'];
	$message['body'].=$message['attachment'].$message_footer; 
	$headers['From'] = $headers['Reply-To'] = $headers['Sender'] = $headers['Return-Path'] = $headers['Errors-To'] = variable_get('site_mail', ini_get('sendmail_from'));
	// Build the default headers
	$headerss = "";
	foreach ($headers as $k=>$v)	$headerss .= "{$k}: {$v}\n";		
	mail($to, $message['subject'], $message['body'], $headerss);
}
include('openinviter.php');
$inviter=new OpenInviter();
$oi_services=$inviter->getPlugins();

function ers($ers)
	{
	if (!empty($ers))
		{
		$contents="<table cellspacing='0' cellpadding='0' style='border:1px solid red;' align='center' class='tbErrorMsgGrad'><tr><td valign='middle' style='padding:3px' valign='middle' class='tbErrorMsg'><img src='images/ers.gif'></td><td valign='middle' style='color:red;padding:5px;'>";
		foreach ($ers as $key=>$error)
			$contents.="{$error}<br >";
		$contents.="</td></tr></table><br >";
		return $contents;
		}
	}
function oks($oks)
	{
	if (!empty($oks))
		{
		$contents="<table border='0' cellspacing='0' cellpadding='10' style='border:1px solid #5897FE;' align='center' class='tbInfoMsgGrad'><tr><td valign='middle' valign='middle' class='tbInfoMsg'><img src='images/oks.gif' ></td><td valign='middle' style='color:#5897FE;padding:5px;'>	";
		foreach ($oks as $key=>$msg)
			$contents.="{$msg}<br >";
		$contents.="</td></tr></table><br >";
		return $contents;
		}
	}

if (!empty($_POST['step'])) $step=$_POST['step'];
else $step='get_contacts';

$ers=array();$oks=array();$import_ok=false;$done=false;
if ($_SERVER['REQUEST_METHOD']=='POST')
	{
	if ($step=='get_contacts')
		{
		if (empty($_POST['email_box']))
			$ers['email']="Email missing";
		if (empty($_POST['password_box']))
			$ers['password']="Password missing";
		if (empty($_POST['provider_box']))
			$ers['provider']="Provider missing";
		if (count($ers)==0)
			{
			$inviter->startPlugin($_POST['provider_box']);
			$internal=$inviter->getInternalError();
			if ($internal)
				$ers['inviter']=$internal;
			elseif (!$inviter->login($_POST['email_box'],$_POST['password_box']))
				{
				$internal=$inviter->getInternalError();
				$ers['login']=($internal?$internal:"Login failed. Please check the email and password you have provided and try again later");
				}
			elseif (false===$contacts=$inviter->getMyContacts())
				$ers['contacts']="Unable to get contacts.";
			else
				{
				$import_ok=true;
				$step='send_invites';
				}
			$inviter->stopPlugin(true);
			$_POST['cookie_file']=$inviter->plugin->cookie;
			$_POST['message_box']='';
			}
		}
	elseif ($step=='send_invites')
		{
		if (empty($_POST['provider_box'])) $ers['provider']='Provider missing';
		else
			{
			$inviter->startPlugin($_POST['provider_box']);
			if (empty($_POST['email_box'])) $ers['inviter']='Inviter information missing';
			if (empty($_POST['cookie_file'])) $ers['cookie']='Could not find cookie file';
			if (empty($_POST['message_box'])) $ers['message_body']='Message missing';
			else $_POST['message_box']=strip_tags($_POST['message_box']);
			$selected_contacts=array();$contacts=array();
			$message=array('subject'=>$inviter->settings['message_subject'],'body'=>$inviter->settings['message_body'],'attachment'=>"\n\rAttached message: \n\r".$_POST['message_box']);
			if ($inviter->showContacts())
				{
				foreach ($_POST as $key=>$val)
					if (strpos($key,'check_')!==false)
						$selected_contacts[$_POST['email_'.$val]]=$_POST['name_'.$val];
					elseif (strpos($key,'email_')!==false)
						{
						$temp=explode('_',$key);$counter=$temp[1];
						if (is_numeric($temp[1])) $contacts[$val]=$_POST['name_'.$temp[1]];
						}
				if (count($selected_contacts)==0) $ers['contacts']="You haven't selected any contacts to invite";
				}
			}
		if (count($ers)==0)
			{
			$sendMessage=$inviter->sendMessage($_POST['cookie_file'],$message,$selected_contacts);
			$inviter->logout();
			if ($sendMessage===-1)
				{
				foreach ($selected_contacts as $email=>$name)
					io_mail($email,$_POST['email_box'],$message);
				$oks['mails']="Mails sent successfully";
				}
			elseif ($sendMessage===false)
				$ers['internal']="There were errors while sending your invites.<br>Please try again later!";
			else $oks['internal']="Invites sent successfully!";
			$done=true;
			}
		}
	}
else
	{
	$_POST['email_box']='';
	$_POST['password_box']='';
	$_POST['provider_box']='';
	$_POST['code_box']='';
	}

$contents="<form action='?q=openinviter' method='POST'>".ers($ers).oks($oks);
if (!$done)
	{
	if ($step=='get_contacts')
		{
		$contents.="<table align='center' class='thTable' cellspacing='0' cellpadding='0' style='border:none;'>
			<tr class='thTableRow'><td><label for='email_box'>Email</label></td></tr><tr class='thTableRow'><td><input class='thTextbox' type='text' name='email_box' value='{$_POST['email_box']}'></td></tr>
			<tr class='thTableRow'><td><label for='password_box'>Password</label></td></tr><tr class='thTableRow'><td><input class='thTextbox' type='password' name='password_box' value='{$_POST['password_box']}'></td></tr>
			<tr class='thTableRow'><td><label for='provider_box'>Email provider</label></td></tr><tr class='thTableRow'><td><select class='thSelect' name='provider_box'><option value=''></option>";
		foreach ($oi_services as $type=>$providers)	
			{
			$contents.="<option disabled>".$inviter->pluginTypes[$type]."</option>";
			foreach ($providers as $provider=>$details)
				$contents.="<option value='{$provider}'".($_POST['provider_box']==$provider?' selected':'').">{$details['name']}</option>";
			}
		$contents.="</select></td></tr>
			<tr class='thTableImportantRow'><td colspan='2' align='center'><input class='thButton' type='submit' name='import' value='Import Contacts'></td></tr>
		</table><input type='hidden' name='step' value='get_contacts'>";
		}
	else
		$contents.="<table class='thTable' cellspacing='0' cellpadding='0' style='border:none;'>
				<tr class='thTableRow'><td align='right' valign='top'><label for='message_box'>Message</label></td><td><textarea rows='5' cols='50' name='message_box' class='thTextArea' style='width:300px;'>{$_POST['message_box']}</textarea></td></tr>
				<tr class='thTableRow'><td align='center' colspan='2'><input type='submit' name='send' value='Send Invites' class='thButton' ></td></tr>
			</table>";
	}
$contents.="<center><a href='http://openinviter.com/'><img src='http://openinviter.com/images/banners/banner_blue_1.gif' border='0' alt='Powered by OpenInviter.com' title='Powered by OpenInviter.com'></a></center>";
if (!$done)
	{
	if ($step=='send_invites')
		{
		if ($inviter->showContacts())
			{
			$contents.="<table class='thTable' align='center' cellspacing='0' cellpadding='0'><tr class='thTableHeader'><td colspan='3'>Your contacts</td></tr>";
			if (count($contacts)==0)
				$contents.="<tr class='thTableOddRow'><td align='center' style='padding:20px;' colspan='3'>You do not have any contacts in your address book.</td></tr>";
			else
				{
				$drupal_users = array();
				$result = db_query("SELECT mail FROM {users}");
				while ($row = db_fetch_array($result)) $drupal_users[$row['mail']]='';
				$contents.="<tr class='thTableDesc'><td>Invite?</td><td>Name</td><td>E-mail</td></tr>";
				$odd=true;$counter=0;
				foreach ($contacts as $email=>$name)
					{
					if (isset($drupal_users[$email])) $disable=true; else $disable=false;
					$counter++;
					if ($odd) $class='thTableOddRow'; else $class='thTableEvenRow';
					$contents.="<tr class='{$class}'><td><input".($disable?' disabled':'')." name='check_{$counter}' value='{$counter}' type='checkbox' class='thCheckbox' checked><input type='hidden' name='email_{$counter}' value='{$email}'><input type='hidden' name='name_{$counter}' value='{$name}'></td><td>{$name}</td><td>".($disable?'Already registered!':$email)."</td></tr>";
					$odd=!$odd;
					}
				$contents.="<tr class='thTableFooter'><td colspan='3' style='padding:3px;' align='center'><input type='submit' name='send' value='Send invites' class='thButton'></td></tr>";
				}
			$contents.="</table>";
			}
		$contents.="<input type='hidden' name='step' value='send_invites'>
			<input type='hidden' name='provider_box' value='{$_POST['provider_box']}'>
			<input type='hidden' name='email_box' value='{$_POST['email_box']}'>
			<input type='hidden' name='cookie_file' value='{$_POST['cookie_file']}'>";
		}
	}
$contents.="</form>";