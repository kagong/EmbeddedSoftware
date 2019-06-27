package com.example.androidex;

import com.example.androidex.R;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;



public class MainActivity2 extends Activity{
	public MusicService mService;
	
	ServiceConnection sconn = new ServiceConnection(){
        @Override //Call when service start
        public void onServiceConnected(ComponentName name, IBinder service) {
            MusicService.MyBinder myBinder = (MusicService.MyBinder) service;
            mService = myBinder.getService();
            Log.e("LOG", "onServiceConnected()");
        }

        @Override //Call when Service end
        public void onServiceDisconnected(ComponentName name) {
            mService = null;
            Log.e("LOG", "onServiceDisconnected()");
        }
	};
		@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main2);
		//Start Timer Service
		Intent intent = new Intent(MainActivity2.this,MusicService.class);
		bindService(intent, sconn,Context.BIND_AUTO_CREATE);

		

	};
	

}
