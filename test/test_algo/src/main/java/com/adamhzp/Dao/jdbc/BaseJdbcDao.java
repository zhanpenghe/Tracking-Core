package com.adamhzp.Dao.jdbc;

import java.util.List;
import java.util.Map;

import javax.sql.DataSource;

import org.apache.commons.dbcp2.BasicDataSource;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.BeanPropertyRowMapper;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.namedparam.BeanPropertySqlParameterSource;
import org.springframework.jdbc.core.namedparam.NamedParameterJdbcTemplate;
import org.springframework.stereotype.Repository;
import org.springframework.util.Assert;

@Repository
@SuppressWarnings({"unchecked","rawtypes"})
public class BaseJdbcDao {

	protected DataSource datasource;
	protected JdbcTemplate jdbcTemplate;
	protected NamedParameterJdbcTemplate namedParameterJdbcTemplate;
	
	public BaseJdbcDao(){
		
	}
	
	public BaseJdbcDao(DataSource datasource)
	{
		this.datasource = datasource;
		this.jdbcTemplate = new JdbcTemplate(this.datasource);
		this.namedParameterJdbcTemplate = new NamedParameterJdbcTemplate(this.datasource);
	}

	public BaseJdbcDao(String hostname,String user,String password, String database)
	{
		BasicDataSource ds = new BasicDataSource();
		ds.setDriverClassName("com.mysql.jdbc.Driver");
		ds.setUrl("jdbc:mysql://"+hostname+"/"+database);
		ds.setUsername(user);
		ds.setPassword(password);
		this.datasource = ds;
		this.jdbcTemplate = new JdbcTemplate(this.datasource);
		this.namedParameterJdbcTemplate = new NamedParameterJdbcTemplate(this.datasource);
	}
	
	public void init(String hostname, String user, String password, String database)
	{
		BasicDataSource ds = new BasicDataSource();
		ds.setDriverClassName("com.mysql.jdbc.Driver");
		ds.setUrl("jdbc:mysql://"+hostname+"/"+database);
		ds.setUsername(user);
		ds.setPassword(password);
		this.datasource = ds;
		this.jdbcTemplate = new JdbcTemplate(this.datasource);
		this.namedParameterJdbcTemplate = new NamedParameterJdbcTemplate(this.datasource);
	}
	
	public int executeForObject(final String sql, Object obj) throws Exception{
		Assert.hasText(sql, "Wrong SQL!");
		if(obj!=null)
		{
			return namedParameterJdbcTemplate.update(sql, new BeanPropertySqlParameterSource(obj));
		}else{
			return jdbcTemplate.update(sql);
		}
	}
	
	/*
	 * Usage example:
	 * insert into users (name, login_name, password) values (:name, :login_name, :password)
	 * keys of the map is after ':'
	 * 
	 * @param sql
	 * @param parameters
	 * @return
	 */
	public int executeForMap(final String sql, Map parameters) throws Exception
	{
		Assert.hasText(sql, "Wrong SQL!");
		if(parameters!=null)
		{
			return namedParameterJdbcTemplate.update(sql, parameters);
		}else{
			return jdbcTemplate.update(sql);
		}
	}
	
	public <T> List findListByObject(final String sql, Class<T> clazz, Object obj) throws Exception
	{
		Assert.hasText(sql, "Wrong SQL!");
		Assert.notNull(clazz, "The class cannot be null!");
		if(obj != null)
		{
			return namedParameterJdbcTemplate.query(sql, new BeanPropertySqlParameterSource(obj), new BeanPropertyRowMapper(clazz));
		}else{
			return jdbcTemplate.query(sql, new BeanPropertyRowMapper(clazz));
		}
	}
	
	public <T> List findListByMap(final String sql, Class<T> clazz, Map parameters) throws Exception
	{
		Assert.hasText(sql, "Wrong SQL!");
		Assert.notNull(clazz, "The class cannot be null!");
		if(parameters != null)
		{
			return namedParameterJdbcTemplate.query(sql, parameters, new BeanPropertyRowMapper(clazz));
		}else{
			return jdbcTemplate.query(sql, new BeanPropertyRowMapper(clazz));
		}
	}
	
	public List findListByMap(final String sql, Map parameters) throws Exception{
		Assert.hasText(sql, "Wrong SQL!");
		if(parameters != null)
		{
			return namedParameterJdbcTemplate.queryForList(sql, parameters);
		}
		else{
			return jdbcTemplate.queryForList(sql);
		}
	}
	
	public <T> Object queryForObject(final String sql, Class<T> clazz, Map parameters){
		Assert.hasText(sql, "Wrong SQL!");
		try{
			if(parameters != null)
			{
				return namedParameterJdbcTemplate.queryForObject(sql, parameters,  new BeanPropertyRowMapper(clazz));
			}else{
				return jdbcTemplate.queryForObject(sql,  new BeanPropertyRowMapper(clazz));
			}
		}catch(EmptyResultDataAccessException e)
		{
			return null;
		}
	}
}
