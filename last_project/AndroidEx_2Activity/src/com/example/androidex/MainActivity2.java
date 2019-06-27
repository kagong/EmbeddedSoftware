package com.example.androidex;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;



public class MainActivity2 extends Activity{
	public MusicService mService;
	
	int[][] customer = new int[8][10];
	int elevator_pos;
	int dest_elevator;
	
	final int[] floor_margin = {0, 434, 365 ,292, 219, 146, 73, 0};

	
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


        test_start();
        
        
		new Thread(new get_data()).start();

	}
		

	private class get_data implements Runnable{
		
		int[][] get_customer = new int[8][10];
		int get_elevator_pos;
		int get_dest_elevator;
		
		@Override
		public void run(){
			while(true){
				
				
				
			}
	
		}
	}
	

	
	protected void test_start(){

		

	    for(int i = 0 ; i < 8 ; i++){
	    	for (int j = 0 ; j < 10 ; j++){
	       		customer[i][j] = 0;
	       	}
	    }
	        /*
	        DisplayMetrics dm = getApplicationContext().getResources().getDisplayMetrics();
	        W = dm.widthPixels;
	        H = dm.heightPixels;
	        */
	        
		customer[6][4] = 1;
		customer[1][1] = -1;
		customer[1][2] = 1;
	               
	    elevator_pos = 1;
	    dest_elevator = 5;
	        
	    print_customer();
			
	    move_elevator();	

	}
	protected void print_customer(){
		LinearLayout[] linear = new LinearLayout[8];
		ImageView img;
		
		linear[0] = (LinearLayout)findViewById(R.id.floor1);
		linear[1] = (LinearLayout)findViewById(R.id.floor2);
		linear[2] = (LinearLayout)findViewById(R.id.floor3);
		linear[3] = (LinearLayout)findViewById(R.id.floor4);
		linear[4] = (LinearLayout)findViewById(R.id.floor5);
		linear[5] = (LinearLayout)findViewById(R.id.floor6);
		linear[6] = (LinearLayout)findViewById(R.id.floor7);
		
		for(int i = 0 ; i < 7 ; i++){
			for(int j = 0 ; j < 10 ; j++){
				img = (ImageView)findViewById(i*10+j);
				
				if (img != null){
					img.setVisibility(View.GONE);
				}
			}
		}

		
		for (int i = 0 ; i < 7 ; i++){
			for(int j = 0 ; j < 10 ; j++){
				if(customer[i][j] == 0) continue;
				img = new ImageView(this);
				LayoutParams param = new LayoutParams(80, 55);
				

				img.setLayoutParams(param);
				if (customer[i][j] > 0){
					img.setImageResource(R.drawable.going_up);
				}
				else if (customer[i][j] < 0){
					img.setImageResource(R.drawable.going_down);
				}
				img.setId(i*10 + j);
				linear[i].addView(img);				
			}
		}		
		
		Log.d("tt", "11");
	}
	
	int howto;
	
	protected void move_elevator(){

		if (dest_elevator > elevator_pos) howto = -1;			// going up
		else if (dest_elevator < elevator_pos) howto = 1;		// going down
		else howto = 0;
		
		
		if (howto == 0) return;

		new Thread(new moving_ele()).start();
		
		return;
	}
	
	private class moving_ele implements Runnable{
		TextView Tv = (TextView)findViewById(R.id.elevator_tv);
		@Override
		public void run(){
			int src = floor_margin[elevator_pos];
			int dest = floor_margin[dest_elevator];
	//		android.os.Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
			
			
			for (int i = 0 ; i < (elevator_pos - dest_elevator) * howto * 9; i++){
				
				try {
					Thread.sleep(111);
					final int finalI = src + howto * i*8;
					runOnUiThread(new Runnable(){
						@Override
						public void run(){
							LinearLayout.LayoutParams mLayoutParams =
									(LinearLayout.LayoutParams) Tv.getLayoutParams();
							mLayoutParams.topMargin = finalI;
							Tv.setLayoutParams(mLayoutParams);
						}
					});
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			runOnUiThread(new Runnable(){
				@Override
				public void run(){
					LinearLayout.LayoutParams mLayoutParams =
							(LinearLayout.LayoutParams) Tv.getLayoutParams();
					mLayoutParams.topMargin = floor_margin[dest_elevator];
					Tv.setLayoutParams(mLayoutParams);
				}
			});
			
		}
	}
}


