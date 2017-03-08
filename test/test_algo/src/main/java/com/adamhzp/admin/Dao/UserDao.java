package com.adamhzp.admin.Dao;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.adamhzp.Dao.jdbc.BaseJdbcDao;
import com.adamhzp.admin.Model.User;

@Repository
public class UserDao {

	BaseJdbcDao dao = new BaseJdbcDao();
	
	public UserDao()
	{
		dao = new BaseJdbcDao("localhost","root", 
				"ioteye1234", "test_db");
	}
	
	public User checkForAuthentication(String username, String password)
	{
		if(username == null || password == null) return null;
		
		Map<String, Object> param = new HashMap<String, Object>();
		param.put("username", username);
		param.put("password", password);
		User user = (User)dao.queryForObject("select * from test_table where username = :username and password = :password", User.class, param);
		if(user != null){
			user.setPassword(null); //clear up the password field
		}

		return user;
		
	}
	
	
}
