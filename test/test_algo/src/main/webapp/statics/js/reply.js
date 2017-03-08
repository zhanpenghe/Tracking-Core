function addReply() {
    var uid = $("#uid").val();
    var mid = $("#mid").val();
    var aid = $("#aid").val();
	var email = $("#edit-submitted-email").val();
    var content = $("#edit-submitted-message").val();
	if (uid == "") {
		alert(error1);
		return false;
	}
	if (content == '文明上网，请对您的发言负责！') {
		alert('请填写评论内容!');
		return false;
	}
    posturl= website + "/" + webpath + "index.php?ac=reply_save";
    $.ajax({
        type: "get",
        url:posturl,
        data: "uid=" + uid + "&mid=" + mid + "&aid=" + aid + "&content=" + content+ "&email=" + email,
        beforeSend: function(XMLHttpRequest){
            //
        },
        success: function(data, textStatus){
            if (data == 0) {
                alert(error2);
            } else if (data == 2) {
				alert(error3);
            } else {
				alert(success1);
                getReply(aid,1,mid);
            }
        },
        complete: function(XMLHttpRequest, textStatus){
        //
        },
        error: function(){
			alert(error4);
            return false;
        }
    });
    return false;
}

function getReply(id,page,mid) {
    posturl= website + "/" + webpath + "index.php?ac=reply_getReply";
    $.ajax({
        type: "get",
		async: false,
        url:posturl,
        data: "id=" + id + "&page=" + page+ "&mid=" + mid,
        beforeSend: function(XMLHttpRequest){
            $("#comments").html('<img src="' + website + '/' + webpath + 'statics/images/loading.gif">');
        },
        success: function(data, textStatus){
            $("#comments").html(data);
        },
        complete: function(XMLHttpRequest, textStatus){
        //
        },
        error: function(){
            alert(error4);
            return false;
        }
    });
    return false;
}

function getNewsDetail(id,money) {
	posturl= website + "/" + webpath + "index.php?ac=news_getContent";
    $.ajax({
        type: "get",
		async: false,
        url:posturl,
        data: "id=" + id + "&money=" + money,
        beforeSend: function(XMLHttpRequest){
            $("#MyContent").html('<div style="border:1px dotted #F90; border-left:6px solid #F60; padding:15px; background:#FFC;margin-top:8px;width:90%"><img src="' + website + '/' + webpath + 'statics/images/bigLoad.gif"></div>');
        },
        success: function(data, textStatus){
            $("#MyContent").html(data);
        },
        complete: function(XMLHttpRequest, textStatus){
        //
        },
        error: function(){
            //alert(error4);
            return false;
        }
    });
    return false;
}

function getCategory(mid) {
    posturl= website + "/" + webpath + "index.php?ac=category_getCategoryTree";
    $.ajax({
        type: "get",
        url:posturl,
        data: "mid=" + mid,
        beforeSend: function(XMLHttpRequest){
            $("#categoryStr").html('<img src="' + website + '/' + webpath + 'statics/images/loading.gif">');
        },
        success: function(data, textStatus){
            $("#categoryStr").html(data);
        },
        complete: function(XMLHttpRequest, textStatus){
        //
        },
        error: function(){
            alert(error4);
            return false;
        }
    });
    return false;
}

function getVersion() {
	posturl = website + "/" + webpath + "index.php?ac=common_getVersion";
    $.ajax({
        type: "get",
        url:posturl,
        beforeSend: function(XMLHttpRequest){
			$("#getVersion").html('<img src="' + website + '/' + webpath + 'statics/images/loading.gif"> <font color=#ff0000>' + msg1 + '</font>');
        },
        success: function(data, textStatus){
			if(data.indexOf(version) != -1){
				$("#getVersion").html("<font color=#ff0000>" + msg2 + data + "</font>");
			} else {
				$("#getVersion").html("<font color=#ff0000>" + msg3 + "<a href='http://www.teamcen.com' target='_blank'><font color=#ff0000>" + msg4 + "</font></a>" + msg5 + data + "</font>");
			}
        },
        complete: function(XMLHttpRequest, textStatus){
        //
        },
        error: function(){
            //alert(error4);
            $("#getVersion").html("<font color=#ff0000>" + error4 + "</font>");
			return false;
        }
    });
    return false;
}

function getBbsSubject() {
	posturl = website + "/" + webpath + "index.php?ac=common_getBBS";
    $.ajax({
        type: "get",
        url:posturl,
        beforeSend: function(XMLHttpRequest){
			$("#getBbsSubject").html('<img src="' + website + '/' + webpath + 'statics/images/loading.gif"> <font color=#ff0000>' + msg6+ '</font>');
        },
        success: function(data, textStatus){
			$("#getBbsSubject").html(data);
        },
        complete: function(XMLHttpRequest, textStatus){
        //
        },
        error: function(){
            $("#getBbsSubject").html(error4);
			//alert(error4);
            return false;
        }
    });
    return false;
}


$(document).ready(function(){
	$("#province").hide();
	$("#province").change(function(){
	   var id = $("#province").val();
	   var type='city';
	   $("#areas").html("");
	   getCity(id,type);
	 });

	 $("#citys").change(function(){
	   var cityid = $("#city").val();
	   var type='area';
	   getArea(cityid,type);
	 });

	 $("#infocid").change(function(){
	   var cid = $("#infocid").val();
	   var sid = 0;
	   getSites(cid,sid);
	 });

})

function getSites(cid,sid) {
	posturl = website + "/" + webpath + "index.php?ac=index_getSites";
    $.ajax({
        type: "get",
        url:posturl,
        data: "cid=" + cid + "&sid=" + sid,
        beforeSend: function(XMLHttpRequest){
			$("#sites").html('<img src="' + website + '/statics/images/loading.gif"></font>');
        },
        success: function(data, textStatus){
			$("#sites").html(data);
        },
        complete: function(XMLHttpRequest, textStatus){
        //
        },
        error: function(){
            alert(error4);
            return false;
        }
    });
    return false;
}

function selectCity() {
	$("#province").show();
	$("#noEdit").hide();
	$("#cityValues").html("");
}

function getCity(id,type) {
	posturl = website + "/" + webpath + "index.php?ac=area_getCitys";
    $.ajax({
        type: "get",
        url:posturl,
        data: "id=" + id + "&type=" + type,
        beforeSend: function(XMLHttpRequest){
			$("#citys").html('<img src="' + website + '/statics/images/loading.gif"></font>');
        },
        success: function(data, textStatus){
			$("#citys").html(data);
        },
        complete: function(XMLHttpRequest, textStatus){
        //
        },
        error: function(){
            alert(error4);
            return false;
        }
    });
    return false;
}

function getArea(id,type) {
	posturl = website + "/" + webpath + "index.php?ac=area_getCitys";
    $.ajax({
        type: "get",
        url:posturl,
        data: "id=" + id + "&type=" + type,
        beforeSend: function(XMLHttpRequest){
			$("#areas").html('<img src="' + website + '/statics/images/loading.gif"></font>');
        },
        success: function(data, textStatus){
			$("#areas").html(data);
        },
        complete: function(XMLHttpRequest, textStatus){
        //
        },
        error: function(){
            alert(error4);
            return false;
        }
    });
    return false;
}

function deleteReply(id,aid,moduleId) {
	posturl = website + '/' + webpath + "admin.php?ac=reply_delete";
    $.ajax({
        type: "get",
        url:posturl,
		data: "id=" + id,
        beforeSend: function(XMLHttpRequest){
			//alert('正在删除中......');
        },
        success: function(data, textStatus){
			if(data == 1){
				alert(msg7);
				getReply(aid,1,moduleId);
			} else {
				alert(msg8);
			}
        },
        complete: function(XMLHttpRequest, textStatus){
        //
        },
        error: function(){
            alert(error4);
            return false;
        }
    });
    return false;
}

function getPluginsList() {
	posturl = website + '/' + webpath + "index.php?ac=plugins_getPluginsListAjax";
    $.ajax({
        type: "get",
        url:posturl,
		data: "id=1",
        beforeSend: function(XMLHttpRequest){
			$("#plugins").html('<img src="statics/images/bigLoad.gif">');
        },
        success: function(data, textStatus){
			$("#plugins").html(data);
        },
        complete: function(XMLHttpRequest, textStatus){
        //
        },
        error: function(){
            return false;
        }
    });
    return false;
}

function setCopy(_sTxt){
	alert(_sTxt);
	asyncbox.alert(_sTxt);
}

function viewAd(aid) {
	asyncbox.open({
		url : website + '/' + webpath + 'admin.php?ac=ad_seeAd&id=' + aid
	});
}
function unselectall() {
	if(document.myform.chkAll.checked) {
	document.myform.chkAll.checked = document.myform.chkAll.checked&0;
	}
}

function exportExcel(type) {
	var starttime = document.getElementById("starttime").value;
	var endtime = document.getElementById("endtime").value;
	if (type ==1) {
		location.href="admin.php?ac=evaluate_creport&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value;
	} else if(type==2){
		location.href="admin.php?ac=evaluate_dreport&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value + "&sid=" + document.getElementById("sid").value;
	}else if(type == 3){
		location.href="admin.php?ac=evaluate_allreport&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value;
	}else if(type == 4){
		location.href="admin.php?ac=evaluate_cstat&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value;
	}else if(type == 5){
		location.href="admin.php?ac=evaluate_sstat&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value;
	}else if(type == 6){
		location.href="admin.php?ac=evaluate_dstat&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&sid=" + document.getElementById("sid").value;
	}else if(type == 7){
		location.href="admin.php?ac=evaluate_carrierreport&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value;
	}
}

function return_exportExcel(type) {
	var starttime = document.getElementById("starttime").value;
	var endtime = document.getElementById("endtime").value;
	if (type ==1) {
		location.href="admin.php?ac=returnreport_creport&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value;
	} else if(type==2){
		location.href="admin.php?ac=returnreport_dreport&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value + "&sid=" + document.getElementById("sid").value;
	}else if(type == 3){
		location.href="admin.php?ac=returnreport_all&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value;
	}else if(type == 4){
		location.href="admin.php?ac=returnreport_cstat&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value;
	}else if(type == 5){
		location.href="admin.php?ac=returnreport_sstat&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value;
	}else if(type == 6){
		location.href="admin.php?ac=returnreport_dstat&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&sid=" + document.getElementById("sid").value;
	}else if(type == 7){
		location.href="admin.php?ac=returnreport_carrierreport&starttime=" + starttime + "&endtime=" + endtime + "&excel=1" + "&cid=" + document.getElementById("cid").value;
	}
}

function exportExcel2(type) {
	var starttime = document.getElementById("starttime").value;
	var endtime = document.getElementById("endtime").value;
	
	if (type ==1) {
		location.href="manage.php?ac=evaluate_sreport&starttime=" + starttime + "&endtime=" + endtime + "&excel=1";
	} else {
		var sid = document.getElementById("sid").value;
		location.href="manage.php?ac=evaluate_dreport&starttime=" + starttime + "&endtime=" + endtime + "&excel=1&sid=" + sid;
	}
}

function return_exportExcel2(type) {
	var starttime = document.getElementById("starttime").value;
	var endtime = document.getElementById("endtime").value;
	
	if (type ==1) {
		location.href="manage.php?ac=returnreport_sreport&starttime=" + starttime + "&endtime=" + endtime + "&excel=1";
	} else {
		var sid = document.getElementById("sid").value;
		location.href="manage.php?ac=returnreport_dreport&starttime=" + starttime + "&endtime=" + endtime + "&excel=1&sid=" + sid;
	}
}

function CheckAll(form) {
	for (var i=0;i<form.elements.length;i++) {
  	var e = form.elements[i];
		if (e.Name != "chkAll"  && e.disabled==false) {
	   		e.checked = form.chkAll.checked;
		}
	}
}
