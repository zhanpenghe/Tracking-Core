<%@ taglib prefix="c" uri="http://java.sun.com/jstl/core_rt" %>
<%@ taglib prefix="spring" uri="http://www.springframework.org/tags" %>
<%@ taglib prefix="form" uri="http://www.springframework.org/tags/form" %>
<%@ taglib prefix="fmt" uri= "http://java.sun.com/jsp/jstl/fmt"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<%@ taglib prefix="html" uri="/WEB-INF/tld/html.tld" %>
<%@ taglib uri="http://java.sun.com/jsp/jstl/fmt" prefix="fmt" %>
<%
	String baseUrl=request.getContextPath();
	String basePath = request.getScheme() + "://"
			+ request.getServerName() + ":" + request.getServerPort()
			+ baseUrl;
%>
<link rel="stylesheet" type="text/css" href="<%=baseUrl%>/statics/css/page.css" />
<link href="<%=baseUrl%>/statics/admin/css/base.css" rel="stylesheet" type="text/css" />
<link href="<%=baseUrl%>/statics/admin/css/admin.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="<%=baseUrl%>/statics/admin/js/jquery.js"></script>
<%-- <script src="<%=baseUrl%>/statics/hack/flot/jquery-1.10.2.min.js" type="text/javascript"></script> --%>
<script type="text/javascript" src="<%=baseUrl%>/statics/admin/js/jquery.tools.js"></script>
<script type="text/javascript" src="<%=baseUrl%>/statics/admin/js/jquery.validate.js"></script>
<script type="text/javascript" src="<%=baseUrl%>/statics/admin/js/jquery.validate.methods.js"></script>
<script type="text/javascript" src="<%=baseUrl%>/statics/hack/My97DatePicker/WdatePicker.js"></script>
<script type="text/javascript" src="<%=baseUrl%>/statics/js/common.js"></script>
<script type="text/javascript" src="<%=baseUrl%>/statics/admin/js/base.js"></script>
<script type="text/javascript" src="<%=baseUrl%>/statics/admin/js/admin.js"></script>
<script type="text/javascript" src="<%=baseUrl%>/statics/hack/ueditor/ueditor.config.js"></script>
<script type="text/javascript" src="<%=baseUrl%>/statics/hack/ueditor/ueditor.all.min.js"></script>
<script src="<%=baseUrl%>/statics/hack/artDialog/artDialog.js?skin=idialog"></script>
<script src="<%=baseUrl%>/statics/hack/artDialog/plugins/iframeTools.js"></script>
<script src="<%=baseUrl%>/statics/js/reply.js"></script>
<c:set var="imagesUrl" value="${pageContext.request.scheme}://${pageContext.request.serverName }:${pageContext.request.serverPort}/images/"/>
<c:set var="baseUrl" value="${pageContext.request.contextPath}"/>
<script>
	var baseUrl='<%=baseUrl%>';
	var basePath='<%=basePath%>';
	var imagesUrl = '${imagesUrl}';
	function changeMenu(obj){
		parent.document.getElementById("middleFrame").src=obj;
	}
</script>