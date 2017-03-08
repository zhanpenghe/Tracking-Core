function fileQueued(file) {
	try {
		var progress = new FileProgress(file, this.customSettings.progressTarget);
		progress.setStatus(watingUpload);
		progress.toggleCancel(true, this);

	} catch (ex) {
		this.debug(ex);
	}

}

function fileQueueError(file, errorCode, message) {
	try {
		if (errorCode === SWFUpload.QUEUE_ERROR.QUEUE_LIMIT_EXCEEDED) {
			alert("Sorry，Every Time Upload Files Max "+message);
			return;
		}

		var progress = new FileProgress(file, this.customSettings.progressTarget);
		progress.setError();
		progress.toggleCancel(false);

		switch (errorCode) {
		case SWFUpload.QUEUE_ERROR.FILE_EXCEEDS_SIZE_LIMIT:
			progress.setStatus("File Is Too Big");
			this.debug("Error Code: File too big, File name: " + file.name + ", File size: " + file.size + ", Message: " + message);
			break;
		case SWFUpload.QUEUE_ERROR.ZERO_BYTE_FILE:
			progress.setStatus("File Is Zero KB");
			this.debug("Error Code: Zero byte file, File name: " + file.name + ", File size: " + file.size + ", Message: " + message);
			break;
		case SWFUpload.QUEUE_ERROR.INVALID_FILETYPE:
			progress.setStatus("Unknow File");
			this.debug("Error Code: Invalid File Type, File name: " + file.name + ", File size: " + file.size + ", Message: " + message);
			break;
		default:
			if (file !== null) {
				progress.setStatus("Unknow Error");
			}
			this.debug("Error Code: " + errorCode + ", File name: " + file.name + ", File size: " + file.size + ", Message: " + message);
			break;
		}
	} catch (ex) {
        this.debug(ex);
    }
}

function fileDialogComplete(numFilesSelected, numFilesQueued) {
	try {
		if (numFilesSelected > 0) {
			document.getElementById('btnUpload').disabled = false;
			document.getElementById(this.customSettings.cancelButtonId).disabled = false;
		}
	} catch (ex)  {
        this.debug(ex);
	}
}

function uploadStart(file) {
	try {
		var progress = new FileProgress(file, this.customSettings.progressTarget);
		progress.setStatus(Uploading);
		progress.toggleCancel(true, this);
	}
	catch (ex) {}

	return true;
}

function uploadProgress(file, bytesLoaded, bytesTotal) {
	try {
		var percent = Math.ceil((bytesLoaded / bytesTotal) * 100);

		var progress = new FileProgress(file, this.customSettings.progressTarget);
		progress.setProgress(percent);
		progress.setStatus(Uploading);
	} catch (ex) {
		this.debug(ex);
	}
}
function SetCopy(obj)
{
   var str=obj.parent().prev().eq(0).children().attr("href");
   if (document.all){//If IE?
	   window.clipboardData.setData('text', str);
	   alert("Copy Success");
	   }else{
	   alert("Copy Error,Please Try Copy By Hands");
	     }
}
function uploadSuccess(file, serverData) {
	try {

		 var obj=window.location;
		 var url="http://";
		 var path=obj.pathname;
		 path=path.substring(path.indexOf("/"),path.lastIndexOf("/"));
		 if(obj.protocol.indexOf("http:")!=-1||obj.protocol.indexOf("https:")!=-1)
	     {
			 url=obj.protocol+"//";
	     }
		 url+=obj.host;
		 if(path!="")
	     {
			 url+=path;
	     }
		if (serverData.indexOf("suc")!=-1) {
			var fileinfo = serverData.substring(5).split(",");
			var progress = new FileProgress(file, this.customSettings.progressTarget);
			progress.setComplete();
			//progress.setText("恭喜你，文件上传成功");
			//progress.setText("<a href=\""+url+"/"+fileinfo[0]+fileinfo[1]+"\" target=\"_blank\">"+fileinfo[1]+"</a>");
			//var status = "大小："+parseFloat(parseInt(fileinfo[2])/1024).toFixed(2)+ "KB<br />";
			//var status = "大图地址：" + fileinfo[0];
			//progress.setStatus(status);
			progress.toggleCancel(false);
			//var picList = "<div style='width:100%;height:80px;float:left;margin:5px;'><input name='pics[path][" + file.id + "]' value='" + fileinfo[1] + "' type='text'><br><input name='pics[info][" + file.id + "]' value='' type='text'></div></div>";
			//echo "suc:pictureAry",".$picb.",".$pics.",Config::get("upload_dir");
			var picList = "<div id='pic_" + file.id + "' style='width:100%;float:left;margin-top:6px;'>图片路径:<input type='text' name='pics[photo][" + file.id + "]' value='" + fileinfo[1] + "' style= 'width:460px;' class='formText'/><input type='hidden' name='pics[photo_s][" + file.id + "]' value='" + fileinfo[2] + "' style= 'width:500px;' class='formText'/><br>图片描述:<input style= 'width:460px;' type='text' name='pics[memo][" + file.id + "]' value='' class='formText'/><br>跳转链接:<input style= 'width:460px;' type='text' name='pics[url][" + file.id + "]' value='' class='formText'/>&nbsp;&nbsp;&nbsp;&nbsp;<a onclick=\"removePic('pic_" + file.id + "','" + fileinfo[1] + "')\" href='javascript:void(0)'><img src='statics/images/icons/application_delete.png' border='0' title='删除'></a>&nbsp;&nbsp;<a href='" + fileinfo[3] + "' target=_blank><img src='statics/images/icons/camera.png' border='0' title='预览'></a></div>";
			document.getElementById('imglist').innerHTML += picList;
			getimgtip();
		}else{
			var progress = new FileProgress(file, this.customSettings.progressTarget);
			progress.setError();
			progress.setStatus("Upload Fail："+serverData.substring(5));
			progress.toggleCancel(false);
		}

	} catch (ex) {
		this.debug(ex);
	}
}

function uploadError(file, errorCode, message) {
	try {
		var progress = new FileProgress(file, this.customSettings.progressTarget);
		progress.setError();
		progress.toggleCancel(false);

		switch (errorCode) {
		case SWFUpload.UPLOAD_ERROR.HTTP_ERROR:
			progress.setStatus("Upload Fail:" + message);
			this.debug("Error Code: HTTP Error, File name: " + file.name + ", Message: " + message);
			break;
		case SWFUpload.UPLOAD_ERROR.UPLOAD_FAILED:
			progress.setStatus("Upload Fail");
			this.debug("Error Code: Upload Failed, File name: " + file.name + ", File size: " + file.size + ", Message: " + message);
			break;
		case SWFUpload.UPLOAD_ERROR.IO_ERROR:
			progress.setStatus("IO Error");
			this.debug("Error Code: IO Error, File name: " + file.name + ", Message: " + message);
			break;
		case SWFUpload.UPLOAD_ERROR.SECURITY_ERROR:
			progress.setStatus("Safe Error");
			this.debug("Error Code: Security Error, File name: " + file.name + ", Message: " + message);
			break;
		case SWFUpload.UPLOAD_ERROR.UPLOAD_LIMIT_EXCEEDED:
			progress.setStatus("File Is Too Big");
			this.debug("Error Code: Upload Limit Exceeded, File name: " + file.name + ", File size: " + file.size + ", Message: " + message);
			break;
		case SWFUpload.UPLOAD_ERROR.FILE_VALIDATION_FAILED:
			progress.setStatus("Skip,Upload Error");
			this.debug("Error Code: File Validation Failed, File name: " + file.name + ", File size: " + file.size + ", Message: " + message);
			break;
		case SWFUpload.UPLOAD_ERROR.FILE_CANCELLED:
			if (this.getStats().files_queued === 0) {
				document.getElementById('btnUpload').disabled = true;
				document.getElementById(this.customSettings.cancelButtonId).disabled = true;
			}
			progress.setStatus("Channer Upload");
			progress.setCancelled();
			break;
		case SWFUpload.UPLOAD_ERROR.UPLOAD_STOPPED:
			progress.setStatus("Stop Upload");
			break;
		default:
			progress.setStatus("Unkonw Error: " + errorCode);
			this.debug("Error Code: " + errorCode + ", File name: " + file.name + ", File size: " + file.size + ", Message: " + message);
			break;
		}
	} catch (ex) {
        this.debug(ex);
    }
}

function uploadComplete(file) {
	if (this.getStats().files_queued === 0) {
		document.getElementById('btnUpload').disabled = true;
		document.getElementById(this.customSettings.cancelButtonId).disabled = true;
	}
}

function queueComplete(numFilesUploaded) {
	var status = document.getElementById("divStatus");
	status.innerHTML = numFilesUploaded + " Files Uploaded";
}

function AddonloadEvent(func)
{
  var oldonload=window.onload;
  if(typeof oldonload!='function')
  {
       window.onload=func;
  }
  else
  {
      window.onload=function(){
        oldonload();
        func();
      };
  }
}
function AddEvent(el, type, fn){
	el.addEventListener ? el.addEventListener(type, fn, false) : el.attachEvent('on' + type, fn);
}
function DelEvent(el, type, fn){
	el.removeEventListener ? el.removeEventListener(type, fn, false) : el.detachEvent('on' + type, fn);
}