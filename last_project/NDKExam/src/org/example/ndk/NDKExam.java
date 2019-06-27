package org.example.ndk;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.widget.TextView;
import org.example.ndk.EVSystem;

public class NDKExam extends Activity {
	@Override
	protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.main);
    	System.loadLibrary("ndk-exam");
        TextView tv = new TextView(this);

        tv.setText("The sum of " + 1 + " and " + 2 + " is " );
        
        setContentView(tv);
        EVSystem temp = new EVSystem();
    
        temp.run();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
