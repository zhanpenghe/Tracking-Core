<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />
<title>Back-end Management</title>
<link href="statics/admin/css/base.css" rel="stylesheet" type="text/css" />
<link href="statics/admin/css/admin.css" rel="stylesheet" type="text/css" />
<%@ include file="/common/common.jsp"%>
<script type="text/javascript" src="statics/admin/js/jquery.js"></script>
<script type="text/javascript" src="statics/admin/js/base.js"></script>
<script type="text/javascript">
$().ready(function() {

	var $menuItem = $("#menu.menuItem");
	var $previousMenuItem;
	
	$menuItem.click( function() {
		var $this = $(this);
		if ($previousMenuItem != null) {
			$previousMenuItem.removeClass("current");
		}
		$previousMenuItem = $this;
		$this.addClass("current");
	})
})


function showleft(id) {
	for(var i=1; i<=1; i++){
		window.parent.frames['menuFrame'].document.getElementById("left_tab_"+i).style.display='none';
	}
	window.parent.frames['menuFrame'].document.getElementById("left_tab_"+id).style.display='block';
}
</script>
</head>
<body class="header">
	<div class="body">
		<div class="bodyLeft">
			<div class="logo"></div>
		</div>
		<div class="bodyRight">
			<div class="link">
				<span class="welcome">
					<strong>${user.username }</strong>&nbsp;Welcome!&nbsp;
				</span>
			</div>
			<div id="menu" class="menu">
				<ul>
						<li class="menuItem">
							<a href="javascript:void(0)" onClick="javascript:showleft(1);" target="menuFrame" hidefocus>Agents Management</a>
						</li>
	            </ul>
	            <div class="info">
					<a class="logout" href="${baseUrl}/login" target="_top">Exit</a>
				</div>
			</div>
		</div>

	</div>
</body>
</html>
