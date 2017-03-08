<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@page import="java.util.HashMap"%>
<%@page import="org.apache.commons.lang3.StringEscapeUtils" %>
<%@page session="false"%>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />
<title>Management v3.3</title>
<link href="statics/admin/css/base.css" rel="stylesheet" type="text/css" />
<link href="statics/admin/css/admin.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="statics/admin/js/jquery.js"></script>
<script type="text/javascript" src="statics/admin/js/base.js"></script>
<script type="text/javascript" src="statics/js/md5.js"></script>


</head>
<body class="login">
	<%
		String error=request.getParameter("error");
		Object temp=request.getAttribute("times");
		Object msg=request.getAttribute("map");
		HashMap<String,String> map=null;
		if(msg!=null){
			map=(HashMap<String,String>)msg;
		}
		String times="";
		if(error==null){
			Object obj=request.getAttribute("error");
			if(obj==null){
				error="";
			}else{
				String _msg=map.get(obj.toString());
				error=StringEscapeUtils.escapeHtml4(_msg);
			}
			
		}
		if(temp==null){
			times="";
		}else{
			times=StringEscapeUtils.escapeHtml4(temp.toString());
		}
	%>
	<script type="text/javascript">
		if (self != top) {
			top.location = self.location;
		};
		var error='<%=error%>';
		var times='<%=times%>';
		if(error!=null&&error!='' && error!='null'){
			if(times!=''){
				alert("Wrong password"+times);
			}else{
				alert(error);
			}
			
		}
	</script>
	
	<div class="blank"></div>
	<div class="blank"></div>
	<div class="blank"></div>
	<div class="body">
		<form id="loginForm" action="/login" method="post">
            <table class="loginTable">
            	<tr>
            		<td rowspan="3">
            			<img src="statics/admin/images/iot logo.png" alt="Management"  style="width:128px;height:128px;"/>
            		</td>
                    <th>
                    	Username:
                    </th>
					<td>
                    	<input type="text" id="username" name="username" class="formText"/>
                    </td>
                </tr>
                <tr>
					<th>
						Password:
					</th>
                    <td>
                    	<input type="password" id="password" name="password" class="formText"/>
                    </td>
                </tr>
                
                <tr>
                    <td>
                        <input type="button" class="homeButton" value="" onclick="window.open('/')" hidefocus /><input type="submit" class="submitButton" value="LOGIN" hidefocus />
                    </td>
                </tr>
            </table>
            <div class="powered">
            	Made by ADAMHZP.
            </div>
        </form>
	</div>
</body>
<script type="text/javascript">
$().ready( function() {
	var $loginForm = $("#loginForm");
	var $username = $("#username");
	var $password = $("#password");

	$loginForm.submit( function() {
		if ($username.val() == "") {
			$.dialog({type: "warn", content: "Please enter your username", modal: true, autoCloseTime: 1000});
			return false;
		}
		if ($password.val() == "") {
			$.dialog({type: "warn", content: "Please enter your password", modal: true, autoCloseTime: 1000});
			return false;
		}
		
	});
	
});

function init(){
	var dt = new Date().getTime();
}
init();
</script>
</html>
