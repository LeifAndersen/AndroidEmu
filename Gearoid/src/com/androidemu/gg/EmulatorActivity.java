package com.androidemu.gg;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.media.AudioManager;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import com.androidemu.Emulator;
import com.androidemu.EmulatorView;
import com.androidemu.EmuMedia;
import com.androidemu.gg.input.*;

public class EmulatorActivity extends Activity implements
		SharedPreferences.OnSharedPreferenceChangeListener,
		SurfaceHolder.Callback,
		View.OnTouchListener,
		EmulatorView.OnTrackballListener,
		Emulator.OnFrameDrawnListener,
		GameKeyListener {

	private static final String LOG_TAG = "Gearoid";

	private static final int REQUEST_LOAD_STATE = 1;
	private static final int REQUEST_SAVE_STATE = 2;

	private static final int DIALOG_QUIT_GAME = 1;
	private static final int DIALOG_REPLACE_GAME = 2;

	private static final int GAMEPAD_LEFT_RIGHT =
			(Emulator.GAMEPAD_LEFT | Emulator.GAMEPAD_RIGHT);
	private static final int GAMEPAD_UP_DOWN =
			(Emulator.GAMEPAD_UP | Emulator.GAMEPAD_DOWN);
	private static final int GAMEPAD_DIRECTION =
			(GAMEPAD_UP_DOWN | GAMEPAD_LEFT_RIGHT);

	private Emulator emulator;
	private EmulatorView emulatorView;
	private Rect surfaceRegion = new Rect();
	private int surfaceWidth;
	private int surfaceHeight;

	private Keyboard keyboard;
	private VirtualKeypad vkeypad;
	private SensorKeypad sensor;
	private boolean flipScreen;
	private boolean inFastForward;
	private float fastForwardSpeed;
	private int trackballSensitivity;

	private int quickLoadKey;
	private int quickSaveKey;
	private int fastForwardKey;
	private int screenshotKey;

	private SharedPreferences sharedPrefs;
	private Intent newIntent;
	private MediaScanner mediaScanner;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		if (!Intent.ACTION_VIEW.equals(getIntent().getAction())) {
			finish();
			return;
		}
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setVolumeControlStream(AudioManager.STREAM_MUSIC);

		sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);
		final SharedPreferences prefs = sharedPrefs;
		prefs.registerOnSharedPreferenceChangeListener(this);

		emulator = Emulator.createInstance(getApplicationContext(),
				getEmulatorEngine(prefs));
		EmuMedia.setOnFrameDrawnListener(this);

		setContentView(R.layout.emulator);

		emulatorView = (EmulatorView) findViewById(R.id.emulator);
		emulatorView.getHolder().addCallback(this);
		emulatorView.setOnTouchListener(this);
		emulatorView.requestFocus();

		// keyboard is always present
		keyboard = new Keyboard(emulatorView, this);

		final String[] prefKeys = {
			"fullScreenMode",
			"flipScreen",
			"fastForwardSpeed",
			"frameSkipMode",
			"maxFrameSkips",
			"refreshRate",
			"soundEnabled",
			"soundVolume",
			"enableTrackball",
			"trackballSensitivity",
			"enableSensor",
			"sensorSensitivity",
			"enableVKeypad",
			"scalingMode",
			"orientation",
			"useInputMethod",
			"quickLoad",
			"quickSave",
			"fastForward",
			"screenshot",
		};

		for (String key : prefKeys)
			onSharedPreferenceChanged(prefs, key);
		loadKeyBindings(prefs);

		if (!loadROM()) {
			finish();
			return;
		}
		startService(new Intent(this, EmulatorService.class).
				setAction(EmulatorService.ACTION_FOREGROUND));
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();

		if (emulator != null)
			emulator.unloadROM();

		stopService(new Intent(this, EmulatorService.class));
	}

	@Override
	protected void onPause() {
		super.onPause();

		pauseEmulator();
		if (sensor != null)
			sensor.setGameKeyListener(null);
	}

	@Override
	protected void onResume() {
		super.onResume();

		if (sensor != null)
			sensor.setGameKeyListener(this);
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);

		pauseEmulator();
		setFlipScreen(sharedPrefs, newConfig);
		resumeEmulator();
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);

		if (hasFocus) {
			// reset keys
			keyboard.reset();
			if (vkeypad != null)
				vkeypad.reset();
			emulator.setKeyStates(0);

			emulator.resume();
		} else
			emulator.pause();
	}

	@Override
	protected void onNewIntent(Intent intent) {
		if (!Intent.ACTION_VIEW.equals(intent.getAction()))
			return;

		newIntent = intent;

		pauseEmulator();
		showDialog(DIALOG_REPLACE_GAME);
	}

	@Override
	protected Dialog onCreateDialog(int id) {
		switch (id) {
		case DIALOG_QUIT_GAME:
			return createQuitGameDialog();
		case DIALOG_REPLACE_GAME:
			return createReplaceGameDialog();
		}
		return super.onCreateDialog(id);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == quickLoadKey) {
			quickLoad();
			return true;
		}
		if (keyCode == quickSaveKey) {
			quickSave();
			return true;
		}
		if (keyCode == fastForwardKey) {
			onFastForward();
			return true;
		}
		if (keyCode == screenshotKey) {
			onScreenshot();
			return true;
		}
		// ignore keys that would annoy the user
		if (keyCode == KeyEvent.KEYCODE_CAMERA ||
				keyCode == KeyEvent.KEYCODE_SEARCH)
			return true;

		if (keyCode == KeyEvent.KEYCODE_BACK) {
			pauseEmulator();
			showDialog(DIALOG_QUIT_GAME);
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);

		getMenuInflater().inflate(R.menu.emulator, menu);
		return true;
	}

	@Override
	public boolean onPrepareOptionsMenu(Menu menu) {
		super.onPrepareOptionsMenu(menu);
		pauseEmulator();

		menu.findItem(R.id.menu_fast_forward).setTitle(
				inFastForward ? R.string.no_fast_forward :
						R.string.fast_forward);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.menu_settings:
			startActivity(new Intent(this, EmulatorSettings.class));
			return true;

		case R.id.menu_reset:
			emulator.reset();
			return true;

		case R.id.menu_fast_forward:
			onFastForward();
			return true;

		case R.id.menu_screenshot:
			onScreenshot();
			return true;

		case R.id.menu_save_state:
			onSaveState();
			return true;

		case R.id.menu_load_state:
			onLoadState();
			return true;

		case R.id.menu_close:
			finish();
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	@Override
	protected void onActivityResult(int request, int result, Intent data) {
		switch (request) {
		case REQUEST_LOAD_STATE:
			if (result == RESULT_OK)
				loadState(data.getData().getPath());
			break;

		case REQUEST_SAVE_STATE:
			if (result == RESULT_OK)
				saveState(data.getData().getPath());
			break;
		}
	}

	public void onSharedPreferenceChanged(SharedPreferences prefs, String key) {
		if (key.startsWith("gamepad")) {
			loadKeyBindings(prefs);

		} else if ("fullScreenMode".equals(key)) {
			WindowManager.LayoutParams attrs = getWindow().getAttributes();
			if (prefs.getBoolean("fullScreenMode", true))
				attrs.flags |= WindowManager.LayoutParams.FLAG_FULLSCREEN;
			else
				attrs.flags &= ~WindowManager.LayoutParams.FLAG_FULLSCREEN;
			getWindow().setAttributes(attrs);

		} else if ("flipScreen".equals(key)) {
			setFlipScreen(prefs, getResources().getConfiguration());

		} else if ("fastForwardSpeed".equals(key)) {
			String value = prefs.getString(key, "2x");
			fastForwardSpeed = Float.parseFloat(
					value.substring(0, value.length() - 1));
			if (inFastForward)
				setGameSpeed(fastForwardSpeed);

		} else if ("frameSkipMode".equals(key)) {
			emulator.setOption(key, prefs.getString(key, "auto"));

		} else if ("maxFrameSkips".equals(key)) {
			emulator.setOption(key, Integer.toString(prefs.getInt(key, 2)));

		} else if ("refreshRate".equals(key)) {
			emulator.setOption(key, prefs.getString(key, "default"));

		} else if ("soundEnabled".equals(key)) {
			emulator.setOption(key, prefs.getBoolean(key, true));

		} else if ("soundVolume".equals(key)) {
			emulator.setOption(key, prefs.getInt(key, 100));

		} else if ("enableTrackball".equals(key)) {
			emulatorView.setOnTrackballListener(
					prefs.getBoolean(key, true) ?  this : null);

		} else if ("trackballSensitivity".equals(key)) {
			trackballSensitivity = prefs.getInt(key, 2) * 5 + 10;

		} else if ("enableSensor".equals(key)) {
			if (!prefs.getBoolean(key, false))
				sensor = null;
			else if (sensor == null) {
				sensor = new SensorKeypad(this);
				sensor.setSensitivity(prefs.getInt("sensorSensitivity", 7));
			}
		} else if ("sensorSensitivity".equals(key)) {
			if (sensor != null)
				sensor.setSensitivity(prefs.getInt(key, 7));

		} else if ("enableVKeypad".equals(key)) {
			if (!prefs.getBoolean(key, true)) {
				if (vkeypad != null) {
					vkeypad.destroy();
					vkeypad = null;
				}
			} else if (vkeypad == null)
				vkeypad = new VirtualKeypad(emulatorView, this);

		} else if ("scalingMode".equals(key)) {
			emulatorView.setScalingMode(getScalingMode(
					prefs.getString(key, "proportional")));

		} else if ("orientation".equals(key)) {
			setRequestedOrientation(getScreenOrientation(
					prefs.getString(key, "unspecified")));

		} else if ("useInputMethod".equals(key)) {
			getWindow().setFlags(prefs.getBoolean(key, false) ?
					0 : WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM,
					WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM);

		} else if ("quickLoad".equals(key)) {
			quickLoadKey = prefs.getInt(key, 0);

		} else if ("quickSave".equals(key)) {
			quickSaveKey = prefs.getInt(key, 0);

		} else if ("fastForward".equals(key)) {
			fastForwardKey = prefs.getInt(key, 0);

		} else if ("screenshot".equals(key)) {
			screenshotKey = prefs.getInt(key, 0);
		}
	}

	public void onGameKeyChanged() {
		int states = keyboard.getKeyStates();
		if (sensor != null)
			states |= sensor.getKeyStates();

		if (flipScreen)
			states = flipGameKeys(states);

		if (vkeypad != null)
			states |= vkeypad.getKeyStates();

		// resolve conflict keys
		if ((states & GAMEPAD_LEFT_RIGHT) == GAMEPAD_LEFT_RIGHT)
			states &= ~GAMEPAD_LEFT_RIGHT;
		if ((states & GAMEPAD_UP_DOWN) == GAMEPAD_UP_DOWN)
			states &= ~GAMEPAD_UP_DOWN;

		emulator.setKeyStates(states);
	}

	public boolean onTrackball(MotionEvent event) {
		float dx = event.getX();
		float dy = event.getY();
		if (flipScreen) {
			dx = -dx;
			dy = -dy;
		}

		int duration1 = (int) (dx * trackballSensitivity);
		int duration2 = (int) (dy * trackballSensitivity);
		int key1 = 0;
		int key2 = 0;

		if (duration1 < 0)
			key1 = Emulator.GAMEPAD_LEFT;
		else if (duration1 > 0)
			key1 = Emulator.GAMEPAD_RIGHT;

		if (duration2 < 0)
			key2 = Emulator.GAMEPAD_UP;
		else if (duration2 > 0)
			key2 = Emulator.GAMEPAD_DOWN;

		if (key1 == 0 && key2 == 0)
			return false;

		emulator.processTrackball(key1, Math.abs(duration1),
				key2, Math.abs(duration2));
		return true;
	}

	public void surfaceCreated(SurfaceHolder holder) {
		emulator.setSurface(holder);
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		if (vkeypad != null)
			vkeypad.destroy();

		emulator.setSurface(null);
	}

	public void surfaceChanged(SurfaceHolder holder,
			int format, int width, int height) {

		surfaceWidth = width;
		surfaceHeight = height;

		if (vkeypad != null)
			vkeypad.resize(width, height);

		final int w = emulator.getVideoWidth();
		final int h = emulator.getVideoHeight();
		surfaceRegion.left = (width - w) / 2;
		surfaceRegion.top = (height - h) / 2;
		surfaceRegion.right = surfaceRegion.left + w;
		surfaceRegion.bottom = surfaceRegion.top + h;

		emulator.setSurfaceRegion(
				surfaceRegion.left, surfaceRegion.top, w, h);
	}

	public void onFrameDrawn(Canvas canvas) {
		if (vkeypad != null)
			vkeypad.draw(canvas);
	}

	public boolean onTouch(View v, MotionEvent event) {
		if (vkeypad != null)
			return vkeypad.onTouch(event, flipScreen);

		return false;
	}

	private void pauseEmulator() {
		emulator.pause();
	}

	private void resumeEmulator() {
		if (hasWindowFocus())
			emulator.resume();
	}

	private void setFlipScreen(SharedPreferences prefs, Configuration config) {
		if (config.orientation == Configuration.ORIENTATION_LANDSCAPE)
			flipScreen = prefs.getBoolean("flipScreen", false);
		else
			flipScreen = false;

		emulator.setOption("flipScreen", flipScreen);
	}

	private int flipGameKeys(int keys) {
		int newKeys = (keys & ~GAMEPAD_DIRECTION);
		if ((keys & Emulator.GAMEPAD_LEFT) != 0)
			newKeys |= Emulator.GAMEPAD_RIGHT;
		if ((keys & Emulator.GAMEPAD_RIGHT) != 0)
			newKeys |= Emulator.GAMEPAD_LEFT;
		if ((keys & Emulator.GAMEPAD_UP) != 0)
			newKeys |= Emulator.GAMEPAD_DOWN;
		if ((keys & Emulator.GAMEPAD_DOWN) != 0)
			newKeys |= Emulator.GAMEPAD_UP;

		return newKeys;
	}

	private static int getScalingMode(String mode) {
		if (mode.equals("original"))
			return EmulatorView.SCALING_ORIGINAL;
		if (mode.equals("2x"))
			return EmulatorView.SCALING_2X;
		if (mode.equals("proportional"))
			return EmulatorView.SCALING_PROPORTIONAL;
		return EmulatorView.SCALING_STRETCH;
	}

	private static int getScreenOrientation(String orientation) {
		if (orientation.equals("landscape"))
			return ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
		if (orientation.equals("portrait"))
			return ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
		return ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED;
	}

	private String getEmulatorEngine(SharedPreferences prefs) {
		return "gg";
	}

	private void loadKeyBindings(SharedPreferences prefs) {
		final int[] gameKeys = EmulatorSettings.gameKeys;
		final int[] defaultKeys = DefaultPreferences.getKeyMappings(this);
		keyboard.clearKeyMap();

		String[] gameKeysPref = EmulatorSettings.gameKeysPref;
		for (int i = 0; i < gameKeysPref.length; i++) {
			keyboard.mapKey(gameKeys[i],
					prefs.getInt(gameKeysPref[i], defaultKeys[i]));
		}
	}

	private Dialog createQuitGameDialog() {
		DialogInterface.OnClickListener l =
			new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int which) {
						switch (which) {
						case 1:
							quickSave();
							// fall through
						case 2:
							finish();
							break;
						}
					}
			};

		return new AlertDialog.Builder(this).
				setTitle(R.string.quit_game_title).
				setItems(R.array.exit_game_options, l).
				create();
	}

	private Dialog createReplaceGameDialog() {
		DialogInterface.OnClickListener l =
			new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int which) {
					if (which == DialogInterface.BUTTON_POSITIVE) {
						setIntent(newIntent);
						loadROM();
					}
					newIntent = null;
				}
			};

		return new AlertDialog.Builder(this).
				setCancelable(false).
				setTitle(R.string.replace_game_title).
				setMessage(R.string.replace_game_message).
				setPositiveButton(android.R.string.yes, l).
				setNegativeButton(android.R.string.no, l).
				create();
	}

	private String getROMFilePath() {
		return getIntent().getData().getPath();
	}

	private boolean isROMSupported(String file) {
		file = file.toLowerCase();

		String[] filters = getResources().
				getStringArray(R.array.file_chooser_filters);
		for (String f : filters) {
			if (file.endsWith(f))
				return true;
		}
		return false;
	}

	private boolean loadROM() {
		String path = getROMFilePath();

		if (!isROMSupported(path)) {
			Toast.makeText(this, R.string.rom_not_supported,
					Toast.LENGTH_SHORT).show();
			finish();
			return false;
		}
		if (!emulator.loadROM(path)) {
			Toast.makeText(this, R.string.load_rom_failed,
					Toast.LENGTH_SHORT).show();
			finish();
			return false;
		}
		// reset fast-forward on ROM load
		inFastForward = false;

		emulatorView.setActualSize(
				emulator.getVideoWidth(), emulator.getVideoHeight());

		if (sharedPrefs.getBoolean("quickLoadOnStart", true))
			quickLoad();
		return true;
	}

	private void onLoadState() {
		Intent intent = new Intent(this, StateSlotsActivity.class);
		intent.setData(getIntent().getData());
		startActivityForResult(intent, REQUEST_LOAD_STATE);
	}

	private void onSaveState() {
		Intent intent = new Intent(this, StateSlotsActivity.class);
		intent.setData(getIntent().getData());
		intent.putExtra(StateSlotsActivity.EXTRA_SAVE_MODE, true);
		startActivityForResult(intent, REQUEST_SAVE_STATE);
	}

	private void setGameSpeed(float speed) {
		pauseEmulator();
		emulator.setOption("gameSpeed", Float.toString(speed));
		resumeEmulator();
	}

	private void onFastForward() {
		inFastForward = !inFastForward;
		setGameSpeed(inFastForward ? fastForwardSpeed : 1.0f);
	}

	private void onScreenshot() {
		File dir = new File("/sdcard/screenshot");
		if (!dir.exists() && !dir.mkdir()) {
			Log.w(LOG_TAG, "Could not create directory for screenshots");
			return;
		}
		String name = Long.toString(System.currentTimeMillis()) + ".png";
		File file = new File(dir, name);

		pauseEmulator();

		FileOutputStream out = null;
		try {
			try {
				out = new FileOutputStream(file);
				Bitmap bitmap = getScreenshot();
				bitmap.compress(Bitmap.CompressFormat.PNG, 100, out);
				bitmap.recycle();

				Toast.makeText(this, R.string.screenshot_saved,
						Toast.LENGTH_SHORT).show();

				if (mediaScanner == null)
					mediaScanner = new MediaScanner(this);
				mediaScanner.scanFile(file.getAbsolutePath(), "image/png");

			} finally {
				if (out != null)
					out.close();
			}
		} catch (IOException e) {}

		resumeEmulator();
	}

	private void saveState(String fileName) {
		pauseEmulator();

		ZipOutputStream out = null;
		try {
			try {
				out = new ZipOutputStream(new BufferedOutputStream(
						new FileOutputStream(fileName)));
				out.putNextEntry(new ZipEntry("screenshot.png"));

				Bitmap bitmap = getScreenshot();
				bitmap.compress(Bitmap.CompressFormat.PNG, 100, out);
				bitmap.recycle();
			} finally {
				if (out != null)
					out.close();
			}
		} catch (Exception e) {}

		emulator.saveState(fileName);
		resumeEmulator();
	}

	private void loadState(String fileName) {
		if (new File(fileName).exists())
			emulator.loadState(fileName);
	}

	private Bitmap getScreenshot() {
		final int w = emulator.getVideoWidth();
		final int h = emulator.getVideoHeight();

		ByteBuffer buffer = ByteBuffer.allocateDirect(w * h * 2);
		emulator.getScreenshot(buffer);

		Bitmap bitmap = Bitmap.createBitmap(w, h, Bitmap.Config.RGB_565);
		bitmap.copyPixelsFromBuffer(buffer);
		return bitmap;
	}

	private String getQuickSlotFileName() {
		return StateSlotsActivity.getSlotFileName(getROMFilePath(), 0);
	}

	private void quickSave() {
		saveState(getQuickSlotFileName());
	}

	private void quickLoad() {
		loadState(getQuickSlotFileName());
	}
}
