<%@ page contentType="text/html;charset=UTF-8" %>
<%@ include file="/common/common.jsp" %>
<%
    Integer statusCode = (Integer) request.getAttribute("javax.servlet.error.status_code");
    String message = (String) request.getAttribute("javax.servlet.error.message");
    String servletName = (String) request.getAttribute("javax.servlet.error.servlet_name");
    String uri = (String) request.getAttribute("javax.servlet.error.request_uri");
    Throwable t = (Throwable) request.getAttribute("javax.servlet.error.exception");
    if(t == null) {
        t = (Throwable) request.getAttribute("exception");
    }
    Class<?> exception = (Class<?>) request.getAttribute("javax.servlet.error.exception_type");
    String exceptionName = "";
    if (exception != null) {
        exceptionName = exception.getName();
    }
    if (statusCode == null) {
        statusCode = 0;
    }
    String queryString = request.getQueryString();
    String url = uri + (queryString == null || queryString.length() == 0 ? "" : "?" + queryString);
    url = url.replaceAll("&amp;", "&").replaceAll("&", "&amp;");
%>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <title>页面500错误</title>
    <style>
		body , ul , li , p , h4{ padding:0px; margin:0px;}
		ul{ list-style:none;}
		body{ color:#323031; font-family:'微软雅黑',Arial, Helvetica, sans-serif; background:#f4f4f4;}
		a{ color:#4f9dd3; text-decoration:none;}
		h4{ font-size:14px; height:30px; line-height:30px; font-weight:bold; font-family:'微软雅黑',Arial, Helvetica, sans-serif; margin:10px 0; }
		/*End reset*/
		.main{ width:600px; height:600px; margin:100px auto;}
		.head , .mid{ border-bottom:1px solid #e3e3e3;}
		.head{ text-align:left; font-size:72px;}
		.head span{ font-size:36px; margin:0 20px;}
		.head img{ vertical-align:middle}
		.head  p{font-size:32px; margin:20px 0;}
		/*End head*/
		.mid{ height:120px; font-size:12px; padding:0 0 40px 0; overflow:hidden} 
		.mid p{ height:90px; width:600px; text-indent:2em; white-space:pre-wrap; word-spacing:normal; word-break:break-all; overflow-y:auto;}
		/*End mid */
		.contern{height:200px; background:url(${ctx }/common/images/warning.png) right center no-repeat;}
		.contern .list{ background:url(${ctx }/common/images/arrow.png) left center no-repeat; height:30px; line-height:30px; padding:0 0 0 1em; font-size:12px;}
	</style>
</head>
<body>
	<div class="main">
	    <div class="head">
	    	500<span>崩溃啦!</span>
	    	<img src="${ctx }/common/images/face.png" />
	    	<p>对不起，内部服务器错误！</p>
	    </div>
	    <div class="mid">
	    	<h4>错误分析：</h4>
	        <p>&nbsp;&nbsp; <%="错误编码："+statusCode + "，错误消息：" + message + "，访问资源：" + uri + "，异常：" + exceptionName + "，RemoteAddr：" + request.getRemoteAddr() %></p>
	    </div>
	    <div class="contern">
	    	<h4>你可以尝试</h4>
	        <ul>
	        	<li class="list"><a href="javascript:void(0);" onclick="history.go(-1)">返回上页</a></li>
	        	<li class="list"><a href="${ctx }/">返回首页</a></li>
	        	<li class="list">
		        	<script>setTimeout("window.location.href ='${ctx }/';", 10000);</script>
		        	<a href="${ctx }/">10秒后自动跳转到首页</a>
	        	</li>
	        </ul>
	    </div>
	</div>
</body>
</html>

