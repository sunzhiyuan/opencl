package com.qualcomm.common;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.SeekBar;
import android.widget.TextView;


/**
 * AdrenoLauncherActivity provides a generic GUI for launching
 * OpenCL samples in the Adreno SDK with a selection of CPU/GPU
 * and whether to RunTests.
 *
 * The meta-data "adreno_package" name of
 * the package.activity that you want to run.  The activity that will be
 * run is "adreno.intent.action.Launch".
 *
 */
public class AdrenoLauncherActivity extends Activity {

	private static final String TAG = "AdrenoLauncherActivity";
	private static final String ADRENO_LAUNCH_ACTIVITY = "adreno.intent.action.Launch";
	private String mPackageName = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_adreno_launcher);

        // Read the name of the activity from meta-data
        try
        {
        	ActivityInfo app = getPackageManager().getActivityInfo(this.getComponentName(), PackageManager.GET_ACTIVITIES | PackageManager.GET_META_DATA);
        	Bundle bundle = app.metaData;

        	// Store the name of the package
        	mPackageName = bundle.getString("adreno_package");

        	// By default, hide CPU/GPU button
        	Button gpuButton = (Button) findViewById(R.id.gpuButton);
        	Button cpuButton = (Button) findViewById(R.id.cpuButton);
        	Button cpugpuButton = (Button) findViewById(R.id.cpugpuButton);
        	cpugpuButton.setVisibility(View.INVISIBLE);

        	String deviceTypeExclusive = bundle.getString("device_type_exclusive");
        	if(deviceTypeExclusive != null)
        	{
        		// GPU only
        		if (deviceTypeExclusive.equalsIgnoreCase("gpu"))
        		{
        			cpuButton.setVisibility(View.INVISIBLE);
        		}
        		// CPU only
        		else if (deviceTypeExclusive.equalsIgnoreCase("cpu"))
        		{
        			gpuButton.setVisibility(View.INVISIBLE);
        		}
        		// GPU+CPU only
        		else if (deviceTypeExclusive.equalsIgnoreCase("cpugpu"))
        		{
        			cpugpuButton.setVisibility(View.VISIBLE);
        			cpuButton.setVisibility(View.INVISIBLE);
        			gpuButton.setVisibility(View.INVISIBLE);
        		}
            }

            String option = bundle.getString("select_wg_size");
            TextView maxWGLabel = (TextView) findViewById(R.id.maxWGLabel);
            SeekBar maxWGBar = (SeekBar) findViewById(R.id.maxWGBar);
            maxWGLabel.setVisibility(View.INVISIBLE);
            maxWGBar.setVisibility(View.INVISIBLE);
            if(option != null)
            {
                if (option.equalsIgnoreCase("yes"))
                {
                    maxWGLabel.setVisibility(View.VISIBLE);
                    maxWGBar.setVisibility(View.VISIBLE);
                }
            }

            option = bundle.getString("select_2d_wg_ratio");
            TextView whRatioLabel = (TextView) findViewById(R.id.whRatioLabel);
            SeekBar whRatioBar = (SeekBar) findViewById(R.id.whRatioBar);
            whRatioLabel.setVisibility(View.INVISIBLE);
            whRatioBar.setVisibility(View.INVISIBLE);
            if(option != null)
            {
                if (option.equalsIgnoreCase("yes"))
                {
                    whRatioLabel.setVisibility(View.VISIBLE);
                    whRatioBar.setVisibility(View.VISIBLE);
                }
            }

            option = bundle.getString("use_local");
            CheckBox useLocalButton = (CheckBox) findViewById(R.id.useLocalButton);
            useLocalButton.setVisibility(View.INVISIBLE);
            if(option != null)
            {
                if (option.equalsIgnoreCase("yes"))
                {
                    useLocalButton.setVisibility(View.VISIBLE);
                }
            }

        }
        catch(NameNotFoundException e)
        {
            Log.e(TAG, "Failed to load meta-data, NameNotFound: " + e.getMessage());
        }
        catch(NullPointerException e)
        {
            Log.e(TAG, "Failed to load meta-data, NullPointer: " +e.getMessage());
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_adreno_launcher, menu);
        return true;
    }

    public void onGPUButtonClick(View view)
    {
    	runNativeActivity("gpu");
    }

    public void onCPUButtonClick(View view)
    {
    	runNativeActivity("cpu");
    }

    public void onCPUGPUButtonClick(View view)
    {
    	runNativeActivity("all");
    }

    private void runNativeActivity(String deviceType)
    {
        Intent intent = new Intent();
        intent.setAction(ADRENO_LAUNCH_ACTIVITY);
        intent.setPackage(mPackageName);
        intent.putExtra("DEVICE", deviceType);
        CheckBox runTestsCheckBox = (CheckBox) findViewById(R.id.runTestsCheckBox);
        intent.putExtra("RUNTESTS", runTestsCheckBox.isChecked() ? "true" : "false");

        SeekBar maxWGBar = (SeekBar) findViewById(R.id.maxWGBar);
        intent.putExtra("MAX_WORKGROUP_SIZE", maxWGBar.getProgress());
        SeekBar whRatioBar = (SeekBar) findViewById(R.id.whRatioBar);
        intent.putExtra("WORKGROUP_WH_RATIO", whRatioBar.getProgress());
        CheckBox useLocalButton = (CheckBox) findViewById(R.id.useLocalButton);
        intent.putExtra("USE_LOCAL_MEM", useLocalButton.isChecked());

        startActivity(intent);

    }

}
