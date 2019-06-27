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


public class MainActivity2 extends Activity{
	public MusicService mService;
	public Button test_btn1;
	public Button test_btn2;
	public int a = 0;
	
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
		test_btn1 = (Button)findViewById(R.id.button1);
		test_btn2 = (Button)findViewById(R.id.button2);
		//Start Timer Service
		Intent intent = new Intent(MainActivity2.this,MusicService.class);
		bindService(intent, sconn,Context.BIND_AUTO_CREATE);
		
		OnClickListener ltn1=new OnClickListener(){
			public void onClick(View v){
				mService.PlayMusic(MusicService.MusicType.DOOR_OPENING);
			};
		};
		test_btn1.setOnClickListener(ltn1);
		

		OnClickListener ltn2=new OnClickListener(){
			public void onClick(View v){
				mService.PlayMusic(MusicService.MusicType.DOOR_CLOSING);
			};
		};
		test_btn2.setOnClickListener(ltn2);
	};
	

}
