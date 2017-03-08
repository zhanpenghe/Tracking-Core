package com.tablib.model;

import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import javax.servlet.jsp.JspException;
import javax.servlet.jsp.tagext.TagSupport;

import org.apache.commons.lang3.StringUtils;

public class DateTag extends TagSupport {
	private static final long serialVersionUID = 6464168398214506236L;
	private String value;
	private String format;

	@Override
	public int doStartTag() throws JspException {
		if("0".equals(value)){
			return super.doStartTag();
		}
		String vv = "" + value;
		if(10==vv.length()){
			vv=vv+"000";
		}
		long time = Long.valueOf(vv);
		Calendar c = Calendar.getInstance();
		c.setTimeInMillis(time);
		if(StringUtils.isBlank(format)){
			format="yyyy-MM-dd HH:mm:ss";
		}
		SimpleDateFormat dateformat = new SimpleDateFormat(format);
		String s = dateformat.format(c.getTime());
		try {
			pageContext.getOut().write(s);
		} catch (IOException e) {
			e.printStackTrace();
		}
		return super.doStartTag();
	}

	public void setValue(String value) {
		this.value = value;
	}
}
