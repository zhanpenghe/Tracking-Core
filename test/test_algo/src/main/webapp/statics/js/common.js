function getCheckValue() {

	var foo = "";
	$("[name=checkItem]").each(function() {
		if ($(this).attr("checked")) {
			foo = foo + $(this).val() + ",";
		}
	});
	return foo;
}

function confirmDelete(url) {
	var ids = getCheckValue();
	if (ids == "") {
		alert("请先选择记录");
		return;
	}
	if (confirm("确定删除所选记录吗？")) {
		document.getElementById("ids").value = getCheckValue();
		document.listForm.action = url;
		document.listForm.submit();
	}
}

function delRecord(url) {
	if (confirm("确定删除所选记录吗？")) {
		document.listForm.action = url;
		document.listForm.submit();
	}
}

/**
 *按用户输入的页码进行跳转
 */
function jumpPage() {
	try {
		var goPageNo = parseInt($("[id=txtPageNo]").val());
		var re = /^[1-9]\d*$/;//验证正整数
		if ("" == $("[id=txtPageNo]").val()) {
			showMessage("请输入要调转的页数。");
			return;
		}
		if (!re.test($("[id=txtPageNo]").val())) {
			showMessage("页码格式有误，只能输入正整数。");
			return;
		}
		//if(isNaN(goPageNo))    {
		//showMessage("页码格式有误，请重新输入。");
		//}
		//当前页不用跳转
		if (goPageNo == getCurrentPageNo()) {
			return;
		}
		if (goPageNo > 0 && goPageNo < (getMaxPageCount() + 1)) {
			//$("[id=page.pageNo]").val(goPageNo);
			document.getElementById('page.pageNo').value=goPageNo;
			doJumpPage((goPageNo - 1) * getPageSize());
		} else {
			showMessage("请输入正确的跳转页面,不能为负数或者大于当前总页数");
		}
	} catch (err) {
		showMessage("页码格式有误，请重新输入。");
	}
}

/**
 * 跳转到首页
 */
function jumpFirstPage() {
	if (getCurrentPageNo() > 1){
		//$("[id=page.pageNo]").val(1);
		document.getElementById('page.pageNo').value=1
		doJumpPage(0);
	}
}

/**
 * 跳转到尾页
 */
function jumpLastPage() {
	var tempCurPageNo = getCurrentPageNo();
	if (tempCurPageNo < getMaxPageCount()){
		//$("[id=page.pageNo]").val(getMaxPageCount());
		document.getElementById('page.pageNo').value=getMaxPageCount();
		doJumpPage((getMaxPageCount() - 1) * getPageSize());
	}
}

/**
 * 下一页
 */
function pageDown() {
	var tempCurPageNo = getCurrentPageNo();
	if (tempCurPageNo < getMaxPageCount()){
		//$("[id=page.pageNo]").val(tempCurPageNo + 1);
		document.getElementById('page.pageNo').value=tempCurPageNo + 1;
		doJumpPage(tempCurPageNo * getPageSize());
	}
}

/**
 * 上一页
 */
function pageUp() {
	var tempCurPageNo = getCurrentPageNo();
	if ( tempCurPageNo > 1){
		//$("[id=page.pageNo]").val(tempCurPageNo -1);
		document.getElementById('page.pageNo').value=tempCurPageNo - 1;
		doJumpPage((tempCurPageNo - 2) * getPageSize());
	}
}

/**
 * 用户下拉框选择不同的“每页记录数”
 */
function changePageSize() {
	$("#limit").val(getPageSize());
	//$("[id=page.pageNo]").val(1); 
	document.getElementById('page.pageNo').value=getCurrentPageNo();
	doJumpPage(0);
}

/**
 * 获取当前页码
 */
function getCurrentPageNo() {
	//return parseInt($("[id=page.pageNo]").val());
	return parseInt(document.getElementById('page.pageNo').value);
}

/**
 * 获取最大页数
 */
function getMaxPageCount() {
	return parseInt($("[id=pageTotalPages]").val());
}

/**
 * 获取每页记录数
 */
function getPageSize() {
	//return parseInt($("[id=page.pageSize]").val());
	return parseInt(document.getElementById('page.pageSize').value);
}

/**
 * 分页跳转函数
 */
function doJumpPage(start) {
	$("#start").val(start);  //开始记录数
	$("#fromPagination").val(1);  //开始记录数
	//$("#listForm")[0].action=$("#search_url").val();
	$("#listForm")[0].submit();
	//$("#listForm").submit();
//	var form=document.getElementById('listForm');
//	form.submit();
}

/**
 * 弹出窗口，显示消息
 */
function showMessage(message){
    alert(message);
}

/* 搜索 */
function submit_serach(){
	//$("#listForm")[0].action=$("#search_url").val();
	doSimpleSearch();	
}

/**
 * 简单关键字搜索
 */
function doSimpleSearch() {
	//搜索前，先将分页参数重置
	document.getElementById('page.pageSize').value=1;
	//$("#page.pageNo").val(1);
	$("#start").val(0);
	//$("#limit").val(20);
	$("#listForm").submit();
}
/**
 * 全选 checkbox
 */
function switchCheckAll(sender){
//    var checkItem = $("[name=checkItem]");
//    checkItem.each(function(){
//        $(this).attr("checked", sender.checked);
//    });
	var inputs = document.getElementsByName("checkItem");     
	for(var i =0;i<inputs.length;i++)     
	    {     
	if(inputs[i].type == "checkbox")     
	        {     
	            inputs[i].checked =sender.checked;      
	        }     
	    }     

}

function goBack(){
    history.back();
}