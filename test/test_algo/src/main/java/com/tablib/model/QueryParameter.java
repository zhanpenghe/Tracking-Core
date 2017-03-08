package com.tablib.model;

import java.io.Serializable;

import org.apache.commons.lang3.StringUtils;

public class QueryParameter implements Serializable{

	private static final long serialVersionUID = 1L;

	public static final String ASC = "asc";
	
	public static final String DESC = "desc";
	
	protected int pageNo = 1; 	
	protected int pageSize = 20;
	protected String orderBy = null;
	protected String order = DESC; 
	protected boolean autoCount = true;


	public int getPageSize() {
		return pageSize;
	}

	public void setPageSize(int pageSize) {
		this.pageSize = pageSize;
	}

	public boolean isPageSizeSetted() {
		return pageSize > -1;
	}

	public int getPageNo() {
		return pageNo;
	}

	public void setPageNo(int pageNo) {
		this.pageNo = pageNo;
	}

	public int getFirst() {
		if (pageNo < 1 || pageSize < 1)
			return -1;
		else
			return ((pageNo - 1) * pageSize);
	}

	public boolean isFirstSetted() {
		return (pageNo > 0 && pageSize > 0);
	}

	public String getOrderBy() {
		return orderBy;
	}

	public void setOrderBy(String orderBy) {
		this.orderBy = orderBy;
	}

	public boolean isOrderBySetted() {
		//return StringUtils.isNotBlank(orderBy);
		return (StringUtils.isNotBlank(orderBy) && StringUtils.isNotBlank(order));
	}

	public String getOrder() {
		return order;
	}

	public void setOrder(String order) {
		if (ASC.equalsIgnoreCase(order) || DESC.equalsIgnoreCase(order)) {
			this.order = order.toLowerCase();
		} else
			throw new IllegalArgumentException("order should be 'desc' or 'asc'");
	}

	public boolean isAutoCount() {
		return autoCount;
	}

	public void setAutoCount(boolean autoCount) {
		this.autoCount = autoCount;
	}
}