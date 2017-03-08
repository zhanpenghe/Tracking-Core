package com.adamhzp.admin.Controller;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.servlet.ModelAndView;

import com.adamhzp.admin.Dao.UserDao;
import com.adamhzp.admin.Model.User;

@Controller
public class LoginController {

	@Autowired
	UserDao dao;
	
	@RequestMapping(value = "/login", method = RequestMethod.GET)
	public String getLoginPage()
	{
		return "login";
	}
	
	@ResponseBody
	@RequestMapping(value = "/login", method = RequestMethod.POST)
	public ModelAndView login(HttpServletRequest request,
			HttpServletResponse response, String username, String password,ModelMap model)
	{
		HashMap<String, String> map=getErrorMsg();
		model.put("map", map);
		if (StringUtils.isBlank(username)){
			model.put("error", "USERNAME IS BLANK");
			return new ModelAndView("login", model);
		}
		if (StringUtils.isBlank(password)){
			model.put("error", "PASSWORD IS BLANK");
			return new ModelAndView("login", model);
		}
		
		User user;
		if((user = dao.checkForAuthentication(username, password))!=null){
			request.getSession().setAttribute("user", user);
			return new ModelAndView("admin/index", model);
		}
		
		model.put("error", "WRONG PASSWORD OR USERNAME");
		return new ModelAndView("login", model);
	}
	
	@RequestMapping(value = "/header")
	@ResponseBody
	private ModelAndView header(HttpServletRequest request,
			HttpServletResponse response, ModelMap model) {
		return new ModelAndView("/admin/header");
	}
	
	/**
	 * Left
	 */
	@RequestMapping(value = "/left")
	@ResponseBody
	private ModelAndView left(HttpServletRequest request,
			HttpServletResponse response, ModelMap model) {
		return new ModelAndView("/admin/left");
	}
	
	/**
	 * Center
	 */
	@RequestMapping(value = "/center")
	@ResponseBody
	private ModelAndView center(HttpServletRequest request,
			HttpServletResponse response, ModelMap model) {
		return new ModelAndView("/admin/center");
	}
	
	/**
	 *  Back-end management
	 */
	@RequestMapping(value = "/main")
	@ResponseBody
	private ModelAndView main(HttpServletRequest request,
			HttpServletResponse response, ModelMap model) {
		model.put("systime", new SimpleDateFormat("yyyy.MM.dd.HH.mm.ss").format( new Date()));
		model.put("javaVersion", System.getProperty("java.version"));
		return new ModelAndView("/admin/main",model);
	}
	
	private HashMap<String, String> getErrorMsg(){
		HashMap<String, String> map=new HashMap<String, String>();
		map.put("WRONG_PASSWORD_OR_USERNAME", "WRONG PASSWORD OR USERNAME");
		return map;
	}
	
	
}
