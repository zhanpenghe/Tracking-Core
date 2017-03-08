package com.tablib.model;

import java.io.Serializable;
import java.util.List;

import javax.servlet.http.HttpServletRequest;

import org.springframework.util.StringUtils;

public class Page extends QueryParameter implements Serializable{

	private static final long serialVersionUID = 1L;

	private List<?> result = null;

	private int pageCount = -1;
	private int totalCount = -1;

	public Page() { }

	public Page(int pageSize) {
		this.pageSize = pageSize;
	}

	public Page(int pageSize, boolean autoCount) {
		this.pageSize = pageSize;
		this.autoCount = autoCount;
	}

	public Page(int pageSize, boolean autoCount,String order,String orderBy) {
		this.pageSize = pageSize;
		this.autoCount = autoCount;
		if (ASC.equalsIgnoreCase(order) || DESC.equalsIgnoreCase(order)) {
			this.order = order.toLowerCase();
			this.orderBy=orderBy;
		} 
	}
	
	public Page(int pageNo,int pageSize, boolean autoCount,String order,String orderBy){
		this.pageSize = pageSize;
		this.pageNo = pageNo;
		this.autoCount = autoCount;
		if (StringUtils.hasText(orderBy) && (ASC.equalsIgnoreCase(order) || DESC.equalsIgnoreCase(order)) ) {
			this.order = order.toLowerCase();
			this.orderBy=orderBy;
		} 
	}
	
	public static Page getInstance(HttpServletRequest request){
		String strPage=request.getParameter("page"); 
		String strRows=request.getParameter("rows"); 
		String sidx=request.getParameter("sidx");  
		String sord=request.getParameter("sord");
		int no = strPage==null ? 1 : Integer.parseInt(strPage);
		int size = strRows == null ?  20 : Integer.parseInt(strRows);
		if(!StringUtils.hasText(sidx))sidx=null;
		return new Page(no,size, true,sord,sidx);
	}
	
	public static Page initPage(HttpServletRequest request){
		String strPageNo=request.getParameter("pageNo");     
		String strPageSize=request.getParameter("pageSize");
		String strOrder=request.getParameter("order");    
		String strOrderBy=request.getParameter("orderBy"); 

		int pageNo = strPageNo==null ? 1 : Integer.parseInt(strPageNo);
		int pageSize = strPageSize == null ?  13 : Integer.parseInt(strPageSize);
		if(!StringUtils.hasText(strOrder)) strOrder="desc";
		if(!StringUtils.hasText(strOrderBy)) strOrderBy=null;

		return new Page(pageNo,pageSize, true,strOrder,strOrderBy);
	}

	public String getInverseOrder() {
		if (order.equalsIgnoreCase(DESC))
			return ASC;
		else
			return DESC;
	}

	public List<?> getResult() {
		return result;
	}

	public void setResult(List<?> result) {
		this.result = result;
	}

	public int getPageCount() {
		return pageCount;
	}

	public void setPageCount(int pageCount) {
		this.pageCount = pageCount;
	}

	public int getTotalCount() {
		return totalCount;
	}

	public void setTotalCount(int totalCount) {
		this.totalCount = totalCount;
	}

	public int getTotalPages() {
		if (totalCount == -1)
			return -1;

		int count = totalCount / pageSize;
		if (totalCount % pageSize > 0) {
			count++;
		}
		pageCount = count;
		return count;
	}

	public boolean isHasNext() {
		return (pageNo + 1 <= getTotalPages());
	}

	public int getNextPage() {
		if (isHasNext())
			return pageNo + 1;
		else
			return pageNo;
	}

	public boolean isHasPre() {
		return (pageNo - 1 >= 1);
	}

	public int getPrePage() {
		if (isHasPre())
			return pageNo - 1;
		else
			return pageNo;
	}	
	
}
