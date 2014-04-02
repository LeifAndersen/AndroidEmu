package com.androidemu.nes.input;

public interface Keycodes
{
	public static final int GAMEPAD_A = 0x01;
	public static final int GAMEPAD_B = 0x02;
	public static final int GAMEPAD_SELECT = 0x04;
	public static final int GAMEPAD_START = 0x08;
	public static final int GAMEPAD_UP = 0x10;
	public static final int GAMEPAD_DOWN = 0x20;
	public static final int GAMEPAD_LEFT = 0x40;
	public static final int GAMEPAD_RIGHT = 0x80;
	public static final int GAMEPAD_A_TURBO = (GAMEPAD_A << 8);
	public static final int GAMEPAD_B_TURBO = (GAMEPAD_B << 8);

	public static final int GAMEPAD_UP_LEFT = (GAMEPAD_UP | GAMEPAD_LEFT);
	public static final int GAMEPAD_UP_RIGHT = (GAMEPAD_UP | GAMEPAD_RIGHT);
	public static final int GAMEPAD_DOWN_LEFT = (GAMEPAD_DOWN | GAMEPAD_LEFT);
	public static final int GAMEPAD_DOWN_RIGHT = (GAMEPAD_DOWN | GAMEPAD_RIGHT);
	public static final int GAMEPAD_AB = (GAMEPAD_A | GAMEPAD_B);
}
