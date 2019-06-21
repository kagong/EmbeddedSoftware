package com.example.androidex;

import java.util.Random;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.DisplayMetrics;

import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.example.androidex.MyCounterService.MyBinder;


public class MainActivity2 extends Activity{
	EditText data;
	TextView timetext;
	Button makeBtn;
	LinearLayout linear,buttonLinear;
	int row,col;
	float widthDp,heightDp;
	Button btns[];
	MyCounterService ms; 
	boolean isService = false,running = true,isstart= false;
	ServiceConnection conn = new ServiceConnection(){
        
        public void onServiceConnected(ComponentName name, IBinder service){
        	MyBinder mb = (MyBinder) service;
        	ms = mb.getService(); //return service object by binder
        	isService = true;
        }        
        public void onServiceDisconnected(ComponentName name) { 
        	isService = false;
        }
    };
	
	private void initButtons(){
		buttonLinear.removeAllViews();
	}
    //buttons's listener it works for puzzle game
	OnClickListener button_listener=new OnClickListener(){
		public void onClick(View v){
			Button temp = (Button)v;
            push_button(Integer.parseInt(temp.getText().toString()));
            if(check_button() == 1){
                initButtons();
                finish();
                stopTimer();
            }
		}
	};
	//start timer of timer service
    private void startTimer(){
    	ms.startTimer();
    	if(isstart == false){
    		new Thread(new GetTime()).start();
    		isstart = true;
    	}
    	

    	running =true;
    }
	//stop timer of timer service
    private void stopTimer(){
    	ms.stopTimer();
    	running =false;
    }
   //check the ending condtion of puzzle game
    private int check_button(){
    	if(row ==1 && col == 1)
    		return 1;
        for(int i=0;i<row*col-1;i++){
            if(!btns[i].getText().toString().equals(Integer.toString(i+1)))
                return 0;
        }
        if(btns[row*col-1].getText().toString().equals("0"))
            return 1;
        return 0;
    }
    private void swap_button(int idx, int i,int target){
        btns[target].setBackgroundResource(android.R.drawable.btn_default);
        btns[target].setText(Integer.toString(idx));
        btns[i].setBackgroundColor(Color.BLACK);
        btns[i].setText("0");
    }

    //when pushed the button,then the idx is buttons's text,
    //and this function decide the swaping button
    private void push_button(int idx){
        if(idx == 0)
            return ;
        for(int i=0;i<row*col;i++){
            if(btns[i].getText().toString().equals(Integer.toString(idx))){
                int r = i /col,c = i % col;
                
                if(r+1 < row){
                    int target= i + col;
                    
                    if(btns[target].getText().toString().equals("0")){
                        swap_button(idx,i,target);
                        break;
                    }
        		}
                if(r-1 >= 0){
                    int target = i -col; 
                    if(btns[target].getText().toString().equals("0")){
                        swap_button(idx,i,target);
                        break;
                    }
                }
                if(c+1 < col){
                    int target = i + 1; 
                    if(btns[target].getText().toString().equals("0")){
                        swap_button(idx,i,target);
                        break;
                    }
                }
                if (c-1 >=0){
                    int target = i - 1; 
                    if(btns[target].getText().toString().equals("0")){
                        swap_button(idx,i,target);
                        break;
                    }
                }
            }
        }
    }

    private void makePuzzle(int idx){
        Random rnd = new Random();
        for(int i=0;i<300;i++){
            int dir = rnd.nextInt(row+3);
            int target=0;
            switch(dir){

            case 0://down
            	target = idx +col;
            	break;
            case 1://right
            	if(idx % col +1 < col)
            		target = idx +1;
            	else
            		target = -1;
            	break;
           
            case 2://left

            	if((idx % col) -1 >= 0)
            		target = idx -1;
            	else
            		target = -1;
            	break;
            default://up
            	target = idx -col;
            	break;
            }
            
            if(0 <= target && target < row*col){
            	idx  = target;
            	push_button(target);
            }
            else
            	--i;
        }
    }
    //when pushed make button
    //this function makes buttons like 1,2,3,.... and black
	private void makeButtons(){
		btns = new Button[row*col];
		
		for(int i= 0 ; i < row; i++){
			LinearLayout test = new LinearLayout(this);
			LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,LinearLayout.LayoutParams.MATCH_PARENT,1);
			test.setLayoutParams(params);
			test.setOrientation(LinearLayout.HORIZONTAL);
			
			for(int j= 0 ; j < col; j++){
				btns[i*col+j] = new Button(this);
				btns[i*col+j].setWidth((int)widthDp/col);
				btns[i*col+j].setHeight((int)heightDp/row);
				btns[i*col+j].setText(Integer.toString(i*col+j+1));
				btns[i*col+j].setOnClickListener(button_listener);
				test.addView(btns[i*col+j]);
			}
			
			buttonLinear.addView(test);
		}
		if(row != 1 || col != 1){
			btns[row*col -1].setText("0");
			btns[row*col -1].setBackgroundColor(Color.BLACK);
		
			Random rnd = new Random();
        	int target = rnd.nextInt(col);
        	for(int i = 1 ; target >= i ; ++i )
        		push_button(row*col -i);
        
        	makePuzzle((row)*col - target-1);
		}
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {

		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_main2);
		makeBtn=(Button)findViewById(R.id.makeButton);
		DisplayMetrics dm = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(dm);
		
		widthDp = dm.widthPixels / dm.density;
		heightDp = (dm.heightPixels / dm.density)-160;
		
		buttonLinear=(LinearLayout)findViewById(R.id.buttonContainer);
		
		timetext=(TextView)findViewById(R.id.timeView);

		data=(EditText)findViewById(R.id.textedit);
		
		OnClickListener make_listener=new OnClickListener(){
			public void onClick(View v){
				String temp=data.getText().toString();
				String datas[] = temp.split(" ");
				row = Integer.parseInt(datas[0]);
				col = Integer.parseInt(datas[1]);
				initButtons();
				makeButtons();
				stopTimer();
                startTimer();
			}
		};
		makeBtn.setOnClickListener(make_listener);
		Intent intent = new Intent(MainActivity2.this, MyCounterService.class);
    	bindService(intent,conn,BIND_AUTO_CREATE);
		
	}

    public void onDestroy() {
    	super.onDestroy();
    	unbindService(conn);
    }
	//its take periodically the time from mycountservice
    //and update time of timetext
	private class GetTime implements Runnable {
		private Handler handler = new Handler();
		public void run() {
			while (running) { 
				if(ms == null) {
					continue;
				}
				 handler.post(new Runnable() { 
					 @Override public void run() { 
						 try { 
							 timetext.setText(ms.getTimeString()); 
						} catch (Exception e) 
						{ e.printStackTrace(); } 
					 } 
				});
				try { Thread.sleep(1000); } catch (InterruptedException e) { e.printStackTrace(); }
				 		 
			}
		}
	}

}
