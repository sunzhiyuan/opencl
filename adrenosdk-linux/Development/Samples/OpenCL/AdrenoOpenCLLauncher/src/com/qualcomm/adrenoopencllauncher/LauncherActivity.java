package com.qualcomm.adrenoopencllauncher;

import android.os.Bundle;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;


/**
 * LauncherActivity contains a simple ListView with all of the Adreno OpenCL SDK
 * samples and launches them via Intents. 
 */
public class LauncherActivity extends Activity {

	private String[] mListItems;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_launcher);
        
        ListView listView = (ListView) findViewById(R.id.launcherListView);
        // List of sample names, update when new samples are added.
        mListItems = new String[]
        		{
        			"BandwidthTest",
        			"ClothSimCLGLES",
        			"Concurrency",
        			"DeviceQuery",
        			"DotProduct",
        			"FFT1D",
        			"FFT2D",
        			"ImageBoxFilter",
        			"ImageMedianFilter",
        			"ImageRecursiveGaussianFilter",
        			"ImageSobelFilter",
        			"InteropCLGLES",
        			"MatrixMatrixMul",
        			"MatrixTranspose",
        			"MatrixVectorMul",
        			"ParallelPrefixSum",
        			"ParticleSystemCLGLES",
        			"PostProcessCLGLES",
        			"VectorAdd"
        		};
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
        	android.R.layout.simple_expandable_list_item_1, android.R.id.text1, mListItems);
        
        listView.setAdapter(adapter);
        
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
        	public void onItemClick(AdapterView<?> parent, View view,int position, long id) 
        	{
        		Intent intent = new Intent(Intent.ACTION_MAIN, null);
        		intent.addCategory(Intent.CATEGORY_LAUNCHER);
        		
        		// The CL-GL interop samples use the AdrenoNativeActivity whereas
        		// the pure CL samples start with the AdrenoLauncher activity.  Choose
        		// which activity to launch below
        		String appName = mListItems[position];
        		String activityName;
        		if (appName.contains("CLGLES"))
        		{
        			activityName = "com.qualcomm.common.AdrenoNativeActivity";
        		}
        		else
        		{
        			activityName = "com.qualcomm.common.AdrenoLauncherActivity";
        		}
        		
        		intent.setComponent(new ComponentName("com.qualcomm." + appName, activityName));
            	intent.setPackage("com.qualcomm." + appName);
            	
            	// Start the app
            	startActivity(intent);
        	}        	        
        });        
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_launcher, menu);
        return true;
    }
}
