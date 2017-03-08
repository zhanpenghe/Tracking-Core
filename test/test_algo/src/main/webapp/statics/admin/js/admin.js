$().ready( function() {

	/* ---------- List ---------- */

	var $listForm = $("#listForm");
	if ($listForm.size() > 0) {
		var $searchButton = $("#searchButton");
		var $allCheck = $("#listTable input.allCheck");
		var $listTableTr = $("#listTable tr:gt(0)");
		var $idsCheck = $("#listTable input[id='ids']");
		var $deleteButton = $("#deleteButton");
		var $linkButton = $("#linkButton"); 

		$allCheck.click( function() {
			var $this = $(this);
			if ($this.attr("checked")) {
				$idsCheck.attr("checked", true);
				$deleteButton.attr("disabled", false);
				$listTableTr.addClass("checked");
			} else {
				$idsCheck.attr("checked", false);
				$deleteButton.attr("disabled", true);
				$listTableTr.removeClass("checked");
			}
		});

		// 无复选框被选中时,删除按钮不可用
		$idsCheck.click( function() {
			var $this = $(this);
			if ($this.attr("checked")) {
				$this.parent().parent().addClass("checked");
				$deleteButton.attr("disabled", false);
			} else {
				$this.parent().parent().removeClass("checked");
				var $idsChecked = $("#listTable input[id='ids']:checked");
				if ($idsChecked.size() > 0) {
					$deleteButton.attr("disabled", false);
				} else {
					$deleteButton.attr("disabled", true)
				}
			}
		});

		$deleteButton.click( function() {
			$.dialog({type: "warn", content: msg9, ok: msg10, cancel: msg11, modal: true, okCallback: batchDelete});
			function batchDelete() {
				$listForm.submit();
			}
		});


		$searchButton.click( function() {
			$pageNumber.val("1");
			$listForm.submit();
		});

		$linkButton.click( function() {
			$listForm.submit();
		});
	}

});