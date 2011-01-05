package com.androidemu.nes;

import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceGroup;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.widget.Toast;
import java.io.File;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;

import com.androidemu.Emulator;
import com.androidemu.nes.wrapper.Wrapper;

public class EmulatorSettings extends PreferenceActivity
		implements Preference.OnPreferenceChangeListener {

	private static final String SEARCH_ROM_URI =
			"http://www.romfind.com/nes-roms.html?sid=YONG";
	private static final Uri ABOUT_URI = Uri.parse(
			"file:///android_asset/about.html");
	private static final String MARKET_URI =
			"http://market.android.com/details?id=";
	private static final String GAME_GRIPPER_URI = 
			"https://sites.google.com/site/gamegripper";

	private static final int REQUEST_LOAD_KEY_PROFILE = 1;
	private static final int REQUEST_SAVE_KEY_PROFILE = 2;
	private static final int REQUEST_GG_ROM = 100;
	private static final int REQUEST_FDS_ROM = 101;

	public static final int[] gameKeys = {
		Emulator.GAMEPAD_UP,
		Emulator.GAMEPAD_DOWN,
		Emulator.GAMEPAD_LEFT,
		Emulator.GAMEPAD_RIGHT,
		Emulator.GAMEPAD_UP_LEFT,
		Emulator.GAMEPAD_UP_RIGHT,
		Emulator.GAMEPAD_DOWN_LEFT,
		Emulator.GAMEPAD_DOWN_RIGHT,
		Emulator.GAMEPAD_SELECT,
		Emulator.GAMEPAD_START,
		Emulator.GAMEPAD_A,
		Emulator.GAMEPAD_B,
		Emulator.GAMEPAD_A_TURBO,
		Emulator.GAMEPAD_B_TURBO,
		Emulator.GAMEPAD_AB,
	};

	public static final String[] gameKeysPref = {
		"gamepad_up",
		"gamepad_down",
		"gamepad_left",
		"gamepad_right",
		"gamepad_up_left",
		"gamepad_up_right",
		"gamepad_down_left",
		"gamepad_down_right",
		"gamepad_select",
		"gamepad_start",
		"gamepad_A",
		"gamepad_B",
		"gamepad_A_turbo",
		"gamepad_B_turbo",
		"gamepad_AB",
	};

	public static final String[] gameKeysPref2 = {
		"gamepad2_up",
		"gamepad2_down",
		"gamepad2_left",
		"gamepad2_right",
		"gamepad2_up_left",
		"gamepad2_up_right",
		"gamepad2_down_left",
		"gamepad2_down_right",
		"gamepad2_select",
		"gamepad2_start",
		"gamepad2_A",
		"gamepad2_B",
		"gamepad2_A_turbo",
		"gamepad2_B_turbo",
		"gamepad2_AB",
	};

	private static final int[] gameKeysName = {
		R.string.gamepad_up,
		R.string.gamepad_down,
		R.string.gamepad_left,
		R.string.gamepad_right,
		R.string.gamepad_up_left,
		R.string.gamepad_up_right,
		R.string.gamepad_down_left,
		R.string.gamepad_down_right,
		R.string.gamepad_select,
		R.string.gamepad_start,
		R.string.gamepad_A,
		R.string.gamepad_B,
		R.string.gamepad_A_turbo,
		R.string.gamepad_B_turbo,
		R.string.gamepad_AB,
	};

	static {
		final int n = gameKeys.length;
		if (gameKeysPref.length != n ||
				gameKeysPref2.length != n ||
				gameKeysName.length != n)
			throw new AssertionError("Key configurations are not consistent");
	}

	public static Intent getSearchROMIntent() {
		return new Intent(Intent.ACTION_VIEW, Uri.parse(SEARCH_ROM_URI)).
				setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
	}

	// FIXME
	private static ArrayList<String> getAllKeyPrefs() {
		ArrayList<String> result = new ArrayList<String>();
		result.addAll(Arrays.asList(gameKeysPref));
		result.addAll(Arrays.asList(gameKeysPref2));
		return result;
	}

	private SharedPreferences settings;

	private Map<String, Integer> getKeyMappings() {
		TreeMap mappings = new TreeMap<String, Integer>();

		for (String key : getAllKeyPrefs()) {
			KeyPreference pref = (KeyPreference) findPreference(key);
			mappings.put(key, new Integer(pref.getKeyValue()));
		}
		return mappings;
	}

	private void setKeyMappings(Map<String, Integer> mappings) {
		SharedPreferences.Editor editor = settings.edit();

		for (String key : getAllKeyPrefs()) {
			Integer value = mappings.get(key);
			if (value != null) {
				KeyPreference pref = (KeyPreference) findPreference(key);
				pref.setKey(value.intValue());
				editor.putInt(key, value.intValue());
			}
		}
		editor.commit();
	}

	private void enableDisablePad2(String device) {
		findPreference("gamepad2").setEnabled("gamepad".equals(device));
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setTitle(R.string.settings);
		addPreferencesFromResource(R.xml.preferences);

		settings = PreferenceManager.getDefaultSharedPreferences(this);
		enableDisablePad2(settings.getString("secondController", "none"));

		// game genie rom
		String rom = settings.getString("gameGenieRom", null);
		if (rom != null)
			findPreference("gameGenieRom").setSummary(rom);

		// fds rom
		rom = settings.getString("fdsRom", null);
		if (rom != null)
			findPreference("fdsRom").setSummary(rom);

		final int[] defaultKeys = DefaultPreferences.getKeyMappings(this);

		// gamepad 1
		PreferenceGroup group = (PreferenceGroup) findPreference("gamepad1");
		for (int i = 0; i < gameKeysPref.length; i++) {
			KeyPreference pref = new KeyPreference(this);
			pref.setKey(gameKeysPref[i]);
			pref.setTitle(gameKeysName[i]);
			pref.setDefaultValue(defaultKeys[i]);
			group.addPreference(pref);
		}
		//  gamepad 2
		group = (PreferenceGroup) findPreference("gamepad2");
		for (int i = 0; i < gameKeysPref2.length; i++) {
			KeyPreference pref = new KeyPreference(this);
			pref.setKey(gameKeysPref2[i]);
			pref.setTitle(gameKeysName[i]);
			group.addPreference(pref);
		}

		if (!Wrapper.supportsMultitouch(this)) {
			findPreference("enableVKeypad").
					setSummary(R.string.multitouch_unsupported);
		}

		findPreference("accurateRendering").setOnPreferenceChangeListener(this);
		findPreference("secondController").setOnPreferenceChangeListener(this);

		findPreference("about").setIntent(new Intent(
				this, HelpActivity.class).setData(ABOUT_URI));
		findPreference("upgrade").setIntent(new Intent(
				Intent.ACTION_VIEW, Uri.parse(MARKET_URI + getPackageName())));
		findPreference("searchRoms").setIntent(getSearchROMIntent());

		findPreference("gameGripper").setIntent(new Intent(
				Intent.ACTION_VIEW, Uri.parse(GAME_GRIPPER_URI)));
	}

	@Override
	protected void onActivityResult(int request, int result, Intent data) {
		switch (request) {
		case REQUEST_GG_ROM:
			if (result == RESULT_OK) {
				String rom = data.getData().getPath();
				settings.edit().putString("gameGenieRom", rom).commit();
				findPreference("gameGenieRom").setSummary(rom);
			}
			break;

		case REQUEST_FDS_ROM:
			if (result== RESULT_OK) {
				String rom = data.getData().getPath();
				settings.edit().putString("fdsRom", rom).commit();
				findPreference("fdsRom").setSummary(rom);
			}
			break;

		case REQUEST_LOAD_KEY_PROFILE:
			if (result == RESULT_OK) {
				setKeyMappings(KeyProfilesActivity.
						loadProfile(this, data.getAction()));
			}
			break;

		case REQUEST_SAVE_KEY_PROFILE:
			if (result == RESULT_OK) {
				KeyProfilesActivity.saveProfile(this, data.getAction(),
						getKeyMappings());
			}
			break;
		}
	}

	@Override
	public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen,
			Preference preference) {
		String key = preference.getKey();

		if ("gameGenieRom".equals(key)) {
			String path = settings.getString(key, null);
			Uri uri = null;
			if (path != null)
				uri = Uri.fromFile(new File(path));

			Intent intent = new Intent(this, FileChooser.class);
			intent.setData(uri);
			intent.putExtra(FileChooser.EXTRA_TITLE,
					getResources().getString(R.string.game_genie_rom));
			intent.putExtra(FileChooser.EXTRA_FILTERS,
					new String[] { ".nes", ".rom", ".bin" });

			startActivityForResult(intent, REQUEST_GG_ROM);
			return true;
		}
		if ("fdsRom".equals(key)) {
			String path = settings.getString(key, null);
			Uri uri = null;
			if (path != null)
				uri = Uri.fromFile(new File(path));

			Intent intent = new Intent(this, FileChooser.class);
			intent.setData(uri);
			intent.putExtra(FileChooser.EXTRA_TITLE,
					getResources().getString(R.string.fds_rom));
			intent.putExtra(FileChooser.EXTRA_FILTERS,
					new String[] { ".nes", ".rom", ".bin" });

			startActivityForResult(intent, REQUEST_FDS_ROM);
			return true;
		}

		if ("loadKeyProfile".equals(key)) {
			Intent intent = new Intent(this, KeyProfilesActivity.class);
			intent.putExtra(KeyProfilesActivity.EXTRA_TITLE,
					getString(R.string.load_profile));
			startActivityForResult(intent, REQUEST_LOAD_KEY_PROFILE);
			return true;
		}
		if ("saveKeyProfile".equals(key)) {
			Intent intent = new Intent(this, KeyProfilesActivity.class);
			intent.putExtra(KeyProfilesActivity.EXTRA_TITLE,
					getString(R.string.save_profile));
			startActivityForResult(intent, REQUEST_SAVE_KEY_PROFILE);
			return true;
		}
		return super.onPreferenceTreeClick(preferenceScreen, preference);
	}

	public boolean onPreferenceChange(Preference preference, Object newValue) {
		String key = preference.getKey();

		if ("accurateRendering".equals(key)) {
			Toast.makeText(this, R.string.accurate_rendering_prompt,
					Toast.LENGTH_SHORT).show();

		} else if ("secondController".equals(key))
			enableDisablePad2(newValue.toString());

		return true;
	}
}
