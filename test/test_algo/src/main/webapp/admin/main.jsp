<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />
<title>Back-End Management</title>
<link href="statics/admin/css/base.css" rel="stylesheet" type="text/css" />
<link href="statics/admin/css/admin.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="statics/admin/js/jquery.js"></script>
<script src="statics/js/reply.js"></script>
</head>
<body class="index">
<div style="display:none"><input type=text name="webpath" id="webpath" value={$webpath}></div>
<div class="bar">
		Welcom to IoT Eye Inc. Device Management System
	</div>
	<div class="body">
		<div class="bodyLeft" style="width:100%">
			<table class="listTable">
				<tr>
					<th colspan="2">
						System Info
					</th>

				</tr>
				<tr>

					<td>
						JAVA Version: 
					</td>
					<td>
						${ javaVersion}
					</td>
				</tr>
				<tr>
					<td>
						Server Time: 
					</td>
					<td>
						${ systime}
					</td>
			</table>
			<div class="blank"></div>
			<table class="listTable" id="getBbsSubject">
			</table>
		</div>
	</div>
</body>
</html>
