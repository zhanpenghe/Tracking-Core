package com.adamhzp.core.Model;

public class Position {
	
	private Double x;
	private Double y;
	private int room;
	
	public Position(Double x, Double y, int room) {
		this.x = x;
		this.y = y;
		this.room = room;
	}
	
	public Double getX() {
		return x;
	}
	public void setX(Double x) {
		this.x = x;
	}
	public Double getY() {
		return y;
	}
	public void setY(Double y) {
		this.y = y;
	}
	public int getRoom() {
		return room;
	}
	public void setRoom(int room) {
		this.room = room;
	}
	
}
