package com.tablib.model;

import javax.servlet.jsp.JspException;
import javax.servlet.jsp.JspWriter;
import javax.servlet.jsp.tagext.JspFragment;
import javax.servlet.jsp.tagext.SimpleTagSupport;

public class PaginationTag extends SimpleTagSupport {

    private String id;
    private int pageSize = 20;
    private int totalCount;
    
    private int totalPage;
    private int currentPageNo;

    @Override
    public void doTag() throws JspException {
        JspWriter out = getJspContext().getOut();

        try {
            if (totalPage < 1) {
                totalPage = 1;
            }
            StringBuilder html = new StringBuilder();
            html.append("<div class='ZB_PageL'>");
            html.append("<table width='100%' border='0' cellspacing='0' cellpadding='0'>");
            html.append("<tr>");
            if(currentPageNo==1){
            	html.append("<td  class='ZB_PageLine'>Homepage&nbsp;&nbsp;Previous&nbsp;&nbsp;<a href='javascript:pageDown()'>Next</a>&nbsp;&nbsp;<a href='javascript:jumpLastPage()'>Last</a></td>");
            }else if(totalCount <= pageSize && currentPageNo == totalPage){
            	html.append("<td  class='ZB_PageLine'><a href='javascript:jumpFirstPage()'>First</a>&nbsp;&nbsp;Previous&nbsp;&nbsp;Next&nbsp;&nbsp;Last</td>");
            }else if(totalCount > pageSize && currentPageNo == totalPage){
            	html.append("<td  class='ZB_PageLine'><a href='javascript:jumpFirstPage()'>First</a>&nbsp;&nbsp;<a href='javascript:pageUp()'>Previous</a>&nbsp;&nbsp;Next&nbsp;&nbsp;Last</td>");
            }else{
            	html.append("<td  class='ZB_PageLine'><a href='javascript:jumpFirstPage()'>First</a>&nbsp;&nbsp;<a href='javascript:pageUp()'>Previous</a>&nbsp;&nbsp;<a href='javascript:pageDown()'>Next</a>&nbsp;&nbsp;<a href='javascript:jumpLastPage()'>Last</a></td>");
            }
            html.append("</tr></table></div>");
            html.append("<div class='ZB_PageR'>");
            html.append("<input type='hidden' id='pageTotalPages' name='pageTotalPages' value='");
            html.append(totalPage);
            html.append("' />");
            html.append("<input type='hidden' id='fromPagination' name='fromPagination' value='' />");
            html.append("<input type='hidden' id='limit' name='limit' value='20' />");
            html.append("<input type='hidden' id='start' name='start' value='0 '/>");
            html.append("<input type='hidden' id='page.pageNo' name='page.pageNo' value='");
            html.append(currentPageNo);
            html.append("' />");
            html.append("<font color='red'>Total");
            html.append(totalCount);
            html.append("Records</font> Each page:");
            html.append("<select id='page.pageSize' name='page.pageSize' value='20' onchange='changePageSize()'>"
                    + "<option value='20' >20</option>"
                    + "<option value='40' >40</option>"
                    + "<option value='60' >60</option>"
                    + "<option value='80' >80</option>"
                    + "<option value='100' >100</option>"
                    + "</select>Records");
            html.append(" Current page：<font color='red'>");
            html.append(currentPageNo);
            html.append("/");
            html.append(totalPage);
            html.append("Page</font> ");

            html.append("<a href='javascript:jumpPage()'>Jump to</a>");
            html.append(" <input type='text' value='' name='txtPageNo' id='txtPageNo' onkeydown='if (event.keyCode==13)jumpPage()' style='width:25px; height:18px;'/>");
            html.append("Page</div>");
            html.append("<script>document.getElementById('page.pageSize').value=");
            html.append(pageSize);
            html.append(";document.getElementById('limit').value=");
            html.append(pageSize);
            html.append(";</script>");
            out.println(html.toString());

            JspFragment f = getJspBody();
            if (f != null) {
                f.invoke(out);
            }
        } catch (java.io.IOException ex) {
            throw new JspException("Error in Pagination tag", ex);
        }
    }

    public void setId(String id) {
        this.id = id;
    }

    public void setPageSize(int pageSize) {
        this.pageSize = pageSize;
    }

    public void setTotalCount(int totalCount) {
        this.totalCount = totalCount;
    }

    public void setTotalPage(int totalPage) {
        this.totalPage = totalPage;
    }

    public void setCurrentPageNo(int currentPageNo) {
        this.currentPageNo = currentPageNo;
    }
}
